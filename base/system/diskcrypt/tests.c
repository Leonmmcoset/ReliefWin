/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Guarded filesystem conversion, restart, credentials and plaintext restoration
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"

static unsigned Checks, Failures;
static DC_VOLUME Volumes[DC_MAX_VOLUMES];
static DC_STORE Store;
static RW_CRYPT_DESCRIPTOR OtherVolumes[RW_CRYPT_MAX_VOLUMES];
static volatile LONG Cancel;
static unsigned Pause;
static BOOL SystemProgress;
static uint64_t LastProgress = UINT64_MAX;
#define CHECK(C) do { ++Checks; if (!(C)) { ++Failures; printf("FAIL %u: %s (%lu)\n", (unsigned)__LINE__, #C, GetLastError()); } } while (0)

static void Notify(void *Context, const VC_PROGRESS *Progress)
{
    (void)Context;
    if (Pause && Progress->Completed >= 2 * VC_JOURNAL_CHUNK)
        InterlockedExchange(&Cancel, 1);
    if (SystemProgress && Progress->Completed / (64 * VC_JOURNAL_CHUNK) != LastProgress)
    {
        LastProgress = Progress->Completed / (64 * VC_JOURNAL_CHUNK);
        printf("System conversion: %I64u / %I64u bytes\n", Progress->Completed, Progress->Total);
    }
}

static BOOL ReadFixture(DC_VOLUME *Volume, BYTE Data[4096])
{
    WCHAR Path[DC_PATH];
    StringCchPrintfW(Path, DC_PATH, L"%sknown.bin", Volume->Root);
    return DcReadWhole(Path, Data, 4096) == ERROR_SUCCESS;
}

static BOOL RecoveryText(PCWSTR Backup, const char *Secret)
{
    WCHAR Path[DC_PATH];
    BYTE Data[256] = {0};
    HANDLE File;
    DWORD Bytes;
    BOOL Result;
    StringCchPrintfW(Path, DC_PATH, L"%s.txt", Backup);
    File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE) return FALSE;
    Result = ReadFile(File, Data, sizeof(Data) - 1, &Bytes, NULL) && Bytes &&
             strstr((const char *)Data, Secret) && strstr((const char *)Data, "Recovery key: ");
    CloseHandle(File);
    VcWipe(Data, sizeof(Data));
    return Result;
}

static BOOL Refresh(DC_VOLUME **Target)
{
    DWORD i, Count;
    if (DcEnumerate(Volumes, &Count) || DcReadStore(&Store) || DcBindVolumes(&Store, Volumes, Count))
        return FALSE;
    for (i = 0; i < Count; ++i)
        if (Volumes[i].Signature == 0x52574354 && Volumes[i].Partition.PartitionNumber == 2)
        {
            *Target = Volumes + i;
            return TRUE;
        }
    return FALSE;
}

static int SystemFixture(BOOL Resume, BOOL Verify, BOOL Decrypt)
{
    DC_VOLUME *Target = NULL, *Backup = NULL, *Boot = NULL;
    BYTE Data[4096], Actual[4096];
    WCHAR Path[DC_PATH], BackupPath[DC_PATH];
    DWORD Count, i, Result;
    HANDLE File, Mapping;
    BYTE *View;
    CHECK(DcRecoveryEnvironment() == !Verify);
    CHECK(DcEnumerate(Volumes, &Count) == ERROR_SUCCESS);
    for (i = 0; i < Count; ++i)
    {
        if (Volumes[i].Signature == 0x568d0fc1 && Volumes[i].Partition.PartitionNumber == 2 &&
            Volumes[i].Partition.PartitionLength.QuadPart == UINT64_C(4226809856))
            Target = Volumes + i;
        if (Volumes[i].Signature == 0x52574354 && Volumes[i].Partition.PartitionNumber == 1)
            Backup = Volumes + i;
        if (Volumes[i].Boot && Volumes[i].Signature == 0x568d0fc1) Boot = Volumes + i;
    }
    CHECK(Target && Backup && Boot);
    if (Failures || !Target || !Backup || !Boot) goto Done;
    StringCchCopyW(Store.Root, DC_PATH, Boot->Root);
    CHECK(DcReadStore(&Store) == ERROR_SUCCESS);
    CHECK(DcBindVolumes(&Store, Volumes, Count) == ERROR_SUCCESS);
    CHECK(Target->Running == Verify);
    CHECK((Resume || Verify || Decrypt) ? (Store.Count == 1 && Target->Index == 0) :
                                        (Store.Count == 0 && Target->Index < 0 && Target->System));
    if (Failures) goto Done;
    for (i = 0; i < sizeof(Data); ++i) Data[i] = (BYTE)(i * 7);
    memcpy(Data, "RWCRYPTSYS1", 11);
    StringCchPrintfW(Path, DC_PATH, L"%sWindows\\rwcrypt-test.bin", Target->Root);
    if (Verify)
    {
        CHECK((Target->Flags & (RW_CRYPT_FLAG_CONFIGURED | RW_CRYPT_FLAG_UNLOCKED | RW_CRYPT_FLAG_SYSTEM)) ==
              (RW_CRYPT_FLAG_CONFIGURED | RW_CRYPT_FLAG_UNLOCKED | RW_CRYPT_FLAG_SYSTEM));
        CHECK(DcReadWhole(Path, Actual, sizeof(Actual)) == ERROR_SUCCESS);
        CHECK(!memcmp(Data, Actual, sizeof(Data)));
        File = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        CHECK(File != INVALID_HANDLE_VALUE);
        if (File != INVALID_HANDLE_VALUE)
        {
            Mapping = CreateFileMappingW(File, NULL, PAGE_READWRITE, 0, 0, NULL);
            CHECK(Mapping != NULL);
            if (Mapping)
            {
                View = MapViewOfFile(Mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Data));
                CHECK(View != NULL);
                if (View)
                {
                    CHECK(!memcmp(View, Data, sizeof(Data)));
                    View[100] ^= 0xff;
                    CHECK(FlushViewOfFile(View, sizeof(Data)));
                    View[100] ^= 0xff;
                    CHECK(FlushViewOfFile(View, sizeof(Data)));
                    CHECK(UnmapViewOfFile(View));
                }
                CloseHandle(Mapping);
            }
            CHECK(FlushFileBuffers(File));
            CloseHandle(File);
        }
        CHECK(DcReadWhole(Path, Actual, sizeof(Actual)) == ERROR_SUCCESS);
        CHECK(!memcmp(Data, Actual, sizeof(Data)));
        CHECK(DcWriteWhole(Path, Data, sizeof(Data), OPEN_EXISTING) == ERROR_SUCCESS);
        CHECK(DcReadWhole(Path, Actual, sizeof(Actual)) == ERROR_SUCCESS);
        CHECK(!memcmp(Data, Actual, sizeof(Data)));
        goto Done;
    }
    if (Resume || Decrypt)
    {
        SystemProgress = TRUE;
        Result = DcConvert(&Store, Target, "fixture-password", NULL, NULL, Decrypt, &Cancel, Notify, NULL);
        printf("System conversion returned %lu\n", Result);
        CHECK(Result == ERROR_SUCCESS);
        CHECK(DcReadStore(&Store) == ERROR_SUCCESS);
        CHECK(Store.Count == (Decrypt ? 0 : 1));
        goto Done;
    }
    Result = DcWriteWhole(Path, Data, sizeof(Data), CREATE_NEW);
    CHECK(Result == ERROR_SUCCESS || Result == ERROR_FILE_EXISTS);
    StringCchPrintfW(BackupPath, DC_PATH, L"%ssystem.rwbackup", Backup->Root);
    if (Failures) goto Done;
    Pause = 1;
    Result = DcConvert(&Store, Target, "fixture-password",
                       "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",
                       BackupPath, FALSE, &Cancel, Notify, NULL);
    printf("System conversion returned %lu\n", Result);
    CHECK(Result == ERROR_OPERATION_ABORTED);
    CHECK(DcReadStore(&Store) == ERROR_SUCCESS);
    CHECK(Store.Count == 1);
Done:
    printf("%u system conversion checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}

int wmain(int Argc, wchar_t **Argv)
{
    DC_VOLUME *Target = NULL, *Backup = NULL, *Boot = NULL;
    BYTE Before[4096], After[4096], *Header = NULL, Key[VC_KEY_SIZE];
    VC_META Meta;
    WCHAR Path[DC_PATH], BackupPath[DC_PATH], RotationPath[DC_PATH];
    const char *Password = "fixture-password", *NewPassword = "fixture-replacement";
    const char *Recovery = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff";
    const char *NewRecovery = "ffeeddccbbaa99887766554433221100ffeeddccbbaa99887766554433221100";
    DWORD Count, i, Result, Baseline = 0;
    const char *CleanupSecret = Password;
    HANDLE Hold;
    setvbuf(stdout, NULL, _IONBF, 0);
    VcCryptoInitialize();
    if (Argc == 2 && !wcscmp(Argv[1], L"--system")) return SystemFixture(FALSE, FALSE, FALSE);
    if (Argc == 2 && !wcscmp(Argv[1], L"--system-resume")) return SystemFixture(TRUE, FALSE, FALSE);
    if (Argc == 2 && !wcscmp(Argv[1], L"--system-verify")) return SystemFixture(FALSE, TRUE, FALSE);
    if (Argc == 2 && !wcscmp(Argv[1], L"--system-decrypt")) return SystemFixture(FALSE, FALSE, TRUE);
    CHECK(DcEnumerate(Volumes, &Count) == ERROR_SUCCESS);
    for (i = 0; i < Count; ++i)
    {
        if (Volumes[i].Signature == 0x52574354 && Volumes[i].Partition.PartitionNumber == 2 &&
            Volumes[i].Partition.PartitionLength.QuadPart == 48 * 1024 * 1024)
            Target = Volumes + i;
        if (Volumes[i].Signature == 0x52574354 && Volumes[i].Partition.PartitionNumber == 1)
            Backup = Volumes + i;
        if (Volumes[i].Boot && Volumes[i].Signature == 0x568d0fc1) Boot = Volumes + i;
    }
    CHECK(Target && Backup && Boot);
    if (!Target || !Backup || !Boot) goto Done;
    StringCchPrintfW(Path, DC_PATH, L"\\\\.\\PhysicalDrive%lu", Target->Disk);
    Hold = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
    CHECK(Hold != INVALID_HANDLE_VALUE);
    if (Hold != INVALID_HANDLE_VALUE)
    {
        CHECK(FlushFileBuffers(Hold));
        CloseHandle(Hold);
    }
    if (Failures) goto Done;
    StringCchCopyW(Store.Root, DC_PATH, Boot->Root);
    StringCchPrintfW(BackupPath, DC_PATH, L"%sconversion.rwbackup", Backup->Root);
    StringCchPrintfW(RotationPath, DC_PATH, L"%srotation.rwbackup", Backup->Root);
    CHECK(DcReadStore(&Store) == ERROR_SUCCESS);
    CHECK(DcBindVolumes(&Store, Volumes, Count) == ERROR_SUCCESS);
    if (Failures) goto Done;
    for (i = 0; i < Store.Count; ++i)
        if ((int)i != Target->Index) OtherVolumes[Baseline++] = Store.Volumes[i];
    if (Target->Index >= 0)
    {
        Result = DcSecret(&Store, Store.Volumes + Target->Index, CleanupSecret, &Header, &Meta, Key);
        if (Result == ERROR_INVALID_PASSWORD)
        {
            CleanupSecret = NewPassword;
            Result = DcSecret(&Store, Store.Volumes + Target->Index, CleanupSecret, &Header, &Meta, Key);
        }
        CHECK(Result == ERROR_SUCCESS);
        if (Header) DcOps.Free(NULL, Header, VC_METADATA_SIZE);
        Header = NULL;
        VcWipe(Key, sizeof(Key));
        if (Failures) goto Done;
        printf("Recovering interrupted fixture conversion\n");
        Result = DcConvert(&Store, Target, CleanupSecret, NULL, NULL, TRUE, &Cancel, Notify, NULL);
        printf("Fixture recovery returned %lu\n", Result);
        CHECK(Result == ERROR_SUCCESS);
        if (Failures) goto Done;
        CHECK(Refresh(&Target));
        if (!Failures && Target->Index >= 0)
        {
            CHECK(DcConvert(&Store, Target, CleanupSecret, NULL, NULL, TRUE, &Cancel, Notify, NULL) == ERROR_SUCCESS);
            CHECK(Refresh(&Target));
        }
    }
    CHECK(Store.Count == Baseline);
    CHECK(ReadFixture(Target, Before));
    if (Failures) goto Done;
    for (i = 0; i < sizeof(After); ++i) After[i] = (BYTE)(i * 7);
    memcpy(After, "RWCRYPTFS01", 11);
    After[100] = Before[100];
    CHECK(!memcmp(Before, After, sizeof(Before)) &&
          (Before[100] == (BYTE)(100 * 7) || Before[100] == (BYTE)((100 * 7) ^ 0xff)));
    if (Failures) goto Done;
    DeleteFileW(BackupPath);
    DeleteFileW(RotationPath);
    StringCchPrintfW(Path, DC_PATH, L"%s.txt", BackupPath);
    DeleteFileW(Path);
    StringCchPrintfW(Path, DC_PATH, L"%s.txt", RotationPath);
    DeleteFileW(Path);
    CHECK(DcBindVolumes(&Store, Volumes, Count) == ERROR_SUCCESS);
    for (i = 0; i < 3; ++i)
    {
        Result = DcExclusive(Target, &Hold);
        CHECK(Result == ERROR_SUCCESS);
        if (Hold != INVALID_HANDLE_VALUE) CloseHandle(Hold);
        CHECK(ReadFixture(Target, After) && !memcmp(Before, After, sizeof(Before)));
        if (Failures) goto Done;
    }
    printf("Repeated mount and dismount passed\n");
    Result = DcExclusive(Target, &Hold);
    CHECK(Result == ERROR_SUCCESS);
    if (Hold != INVALID_HANDLE_VALUE) CloseHandle(Hold);
    if (Failures) goto Done;
    Pause = 1;
    Result = DcConvert(&Store, Target, Password, Recovery, BackupPath, FALSE, &Cancel, Notify, NULL);
    CHECK(Result == ERROR_OPERATION_ABORTED);
    if (Result != ERROR_OPERATION_ABORTED) { printf("encrypt returned %lu\n", Result); goto Done; }
    CHECK(RecoveryText(BackupPath, Recovery));
    CHECK(Refresh(&Target));
    CHECK(Target->Flags & RW_CRYPT_FLAG_OFFLINE);
    CHECK(!ReadFixture(Target, After));
    if (Failures) goto Done;
    printf("Encryption paused safely\n");
    Pause = 0;
    InterlockedExchange(&Cancel, 0);
    CHECK(DcConvert(&Store, Target, Recovery, NULL, NULL, FALSE, &Cancel, Notify, NULL) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    CHECK(!(Target->Flags & RW_CRYPT_FLAG_OFFLINE));
    if (Failures) goto Done;
    printf("Recovery key resumed encryption\n");
    CHECK(DcSetLock(&Store, Target, "wrong-password", TRUE) == ERROR_INVALID_PASSWORD);
    CHECK(DcSetLock(&Store, Target, Password, TRUE) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    CHECK(ReadFixture(Target, After) && !memcmp(Before, After, sizeof(Before)));
    if (Failures) goto Done;
    After[100] ^= 0xff;
    StringCchPrintfW(Path, DC_PATH, L"%sknown.bin", Target->Root);
    CHECK(DcWriteWhole(Path, After, sizeof(After), OPEN_EXISTING) == ERROR_SUCCESS);
    CHECK(DcChange(&Store, Target, Password, NewPassword, FALSE, NULL) == ERROR_SUCCESS);
    CHECK(DcChange(&Store, Target, NewPassword, NewRecovery, TRUE, RotationPath) == ERROR_SUCCESS);
    CHECK(RecoveryText(RotationPath, NewRecovery));
    CHECK(DcSetLock(&Store, Target, NULL, FALSE) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    if (Failures) goto Done;
    printf("Credentials rotated and volume locked\n");
    CHECK(DcImport(&Store, Backup, NewRecovery, RotationPath) == ERROR_INVALID_DATA);
    CHECK(DcImport(&Store, Target, NewRecovery, RotationPath) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    CHECK(DcConvert(&Store, Target, NewRecovery, NULL, NULL, FALSE, &Cancel, Notify, NULL) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    if (Failures) goto Done;
    printf("Recovery backup restored to the bound volume\n");
    CHECK(DcSecret(&Store, Store.Volumes + Target->Index, Password, &Header, &Meta, Key) == ERROR_INVALID_PASSWORD);
    CHECK(DcSecret(&Store, Store.Volumes + Target->Index, Recovery, &Header, &Meta, Key) == ERROR_INVALID_PASSWORD);
    Pause = 1;
    InterlockedExchange(&Cancel, 0);
    CHECK(DcConvert(&Store, Target, NewRecovery, NULL, NULL, TRUE, &Cancel, Notify, NULL) == ERROR_OPERATION_ABORTED);
    CHECK(Refresh(&Target));
    CHECK(Target->Flags & RW_CRYPT_FLAG_OFFLINE);
    Pause = 0;
    InterlockedExchange(&Cancel, 0);
    CHECK(DcConvert(&Store, Target, NewPassword, NULL, NULL, FALSE, &Cancel, Notify, NULL) == ERROR_SUCCESS);
    CHECK(Refresh(&Target));
    CHECK(Store.Count == Baseline && !(Target->Flags & RW_CRYPT_FLAG_CONFIGURED));
    CHECK(!memcmp(Store.Volumes, OtherVolumes, Baseline * sizeof(OtherVolumes[0])));
    CHECK(ReadFixture(Target, Before) && !memcmp(Before, After, sizeof(After)));
Done:
    if (Header) DcOps.Free(NULL, Header, VC_METADATA_SIZE);
    VcWipe(Key, sizeof(Key));
    printf("%u management checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
