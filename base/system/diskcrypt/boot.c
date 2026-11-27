/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Keep the OS loader outside encrypted system volumes
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"

#define DC_BCD_MOUNT L"ReliefWinEncryptionBCD"
#define DC_BCD_MANAGER L"{9dea862c-5cdd-4e70-acc1-f32b344d4795}"
#define DC_BCD_SYSTEM L"{354fa6e5-5e45-49af-91bf-1fd2754e470a}"
#define DC_BCD_RECOVERY L"{e24552c5-f0e2-5ec0-9f7d-52a05f389a30}"

static BOOL Privilege(PCWSTR Name)
{
    TOKEN_PRIVILEGES Privileges;
    HANDLE Token;
    BOOL Result = FALSE;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token))
        return FALSE;
    Privileges.PrivilegeCount = 1;
    Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (LookupPrivilegeValueW(NULL, Name, &Privileges.Privileges[0].Luid) &&
        AdjustTokenPrivileges(Token, FALSE, &Privileges, 0, NULL, NULL) && GetLastError() == ERROR_SUCCESS)
        Result = TRUE;
    CloseHandle(Token);
    return Result;
}

static BOOL DeviceMatches(const BYTE *Device, DWORD Size, DC_VOLUME *Volume)
{
    DWORD Type, Style, Signature;
    ULONGLONG Offset;
    if (Size != 0x58) return FALSE;
    memcpy(&Type, Device + 0x10, 4);
    memcpy(&Style, Device + 0x34, 4);
    if (Type != 6) return FALSE;
    if (Volume->Partition.PartitionStyle == PARTITION_STYLE_GPT)
        return Style == 0 && !memcmp(Device + 0x20, &Volume->Partition.Gpt.PartitionId, 16) &&
               !memcmp(Device + 0x38, &Volume->DiskGuid, 16);
    memcpy(&Signature, Device + 0x38, 4);
    memcpy(&Offset, Device + 0x20, 8);
    return Style == 1 && Signature == Volume->Signature && Offset == (ULONGLONG)Volume->Partition.StartingOffset.QuadPart;
}

static void PartitionDevice(DC_VOLUME *Volume, BYTE Device[0x58])
{
    DWORD Type = 6, Length = 0x48, Style = 1;
    memset(Device, 0, 0x58);
    memcpy(Device + 0x10, &Type, 4);
    memcpy(Device + 0x18, &Length, 4);
    if (Volume->Partition.PartitionStyle == PARTITION_STYLE_GPT)
    {
        memcpy(Device + 0x20, &Volume->Partition.Gpt.PartitionId, 16);
        memcpy(Device + 0x38, &Volume->DiskGuid, 16);
    }
    else
    {
        memcpy(Device + 0x20, &Volume->Partition.StartingOffset.QuadPart, 8);
        memcpy(Device + 0x34, &Style, 4);
        memcpy(Device + 0x38, &Volume->Signature, 4);
    }
}

static DWORD ReadElement(HKEY Root, PCWSTR Id, DWORD Element, DWORD Mask, void *Buffer, DWORD *Size)
{
    WCHAR Path[128];
    StringCchPrintfW(Path, _countof(Path), L"Objects\\%s\\Elements\\%08lx", Id, Element);
    return RegGetValueW(Root, Path, L"Element", Mask, NULL, Buffer, Size);
}

static BOOL SystemBoot(DC_VOLUME *Boot)
{
    WCHAR Partition[DC_PATH], Path[DC_PATH];
    DWORD Size = sizeof(Partition), Done;
    HANDLE File;
    STORAGE_DEVICE_NUMBER Number;
    BOOL Result;
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SYSTEM\\Setup", L"SystemPartition", RRF_RT_REG_SZ,
                     NULL, Partition, &Size)) return FALSE;
    StringCchPrintfW(Path, DC_PATH, L"\\\\?\\GLOBALROOT%s", Partition);
    File = CreateFileW(Path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE) return FALSE;
    Result = DeviceIoControl(File, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &Number, sizeof(Number), &Done, NULL) &&
             Number.DeviceNumber == Boot->Disk && Number.PartitionNumber == Boot->Partition.PartitionNumber;
    CloseHandle(File);
    return Result;
}

static DWORD CopyLoader(DC_STORE *Store, PCWSTR Name, PCWSTR Destination)
{
    WCHAR Source[DC_PATH], Path[DC_PATH], Temporary[DC_PATH];
    HANDLE File;
    LARGE_INTEGER Size;
    BYTE *Data = NULL, *Check = NULL;
    DWORD Error;
    if (!GetSystemDirectoryW(Source, DC_PATH)) return GetLastError();
    StringCchCatW(Source, DC_PATH, L"\\");
    StringCchCatW(Source, DC_PATH, Name);
    File = CreateFileW(Source, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE) return GetLastError();
    if (!GetFileSizeEx(File, &Size) || Size.QuadPart <= 0 || Size.QuadPart > 64 * 1024 * 1024)
    {
        CloseHandle(File);
        return ERROR_INVALID_DATA;
    }
    CloseHandle(File);
    Data = DcOps.Allocate(NULL, (size_t)Size.QuadPart);
    Check = DcOps.Allocate(NULL, (size_t)Size.QuadPart);
    if (!Data || !Check)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Done;
    }
    Error = DcReadWhole(Source, Data, (DWORD)Size.QuadPart);
    if (Error) goto Done;
    StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root, Destination);
    StringCchPrintfW(Temporary, DC_PATH, L"%s.new", Path);
    Error = DcWriteWhole(Temporary, Data, (DWORD)Size.QuadPart, CREATE_ALWAYS);
    if (!Error) Error = DcReadWhole(Temporary, Check, (DWORD)Size.QuadPart);
    if (!Error && memcmp(Data, Check, (size_t)Size.QuadPart)) Error = ERROR_CRC;
    if (!Error && !MoveFileExW(Temporary, Path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        Error = GetLastError();
Done:
    if (Data) DcOps.Free(NULL, Data, (size_t)Size.QuadPart);
    if (Check) DcOps.Free(NULL, Check, (size_t)Size.QuadPart);
    return Error;
}

static DWORD PrepareStore(DC_STORE *Store, DC_VOLUME *Boot, DC_VOLUME *Volume, BOOL Bios)
{
    WCHAR Path[DC_PATH], Backup[DC_PATH], Id[64], KeyPath[128], Loader[128];
    BYTE Device[0x58], Existing[0x58];
    HKEY Root = NULL, Objects = NULL, Key = NULL;
    DWORD Error, Size, Index, Type, Matches = 0;
    BOOL Loaded = FALSE, Ours;
    StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root, Bios ? L"Boot\\BCD" : L"EFI\\ReliefWin\\Boot\\BCD");
    Error = RegLoadKeyW(HKEY_LOCAL_MACHINE, DC_BCD_MOUNT, Path);
    if (!Error)
    {
        Loaded = TRUE;
        Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, DC_BCD_MOUNT, 0, KEY_ALL_ACCESS, &Root);
    }
    else if (SystemBoot(Boot))
        Error = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"BCD00000000", 0, NULL,
                                REG_OPTION_BACKUP_RESTORE, KEY_READ | KEY_WRITE, NULL, &Root, NULL);
    if (Error) goto Done;
    Size = sizeof(Existing);
    Error = ReadElement(Root, DC_BCD_MANAGER, 0x11000001, RRF_RT_REG_BINARY, Existing, &Size);
    if (!Error && !DeviceMatches(Existing, Size, Boot) &&
        !(Size == sizeof(Existing) && Existing[0x10] == 5 && Existing[0x11] == 0 &&
          Existing[0x12] == 0 && Existing[0x13] == 0)) Error = ERROR_INVALID_DATA;
    if (Error) goto Done;
    StringCchPrintfW(Backup, DC_PATH, L"%s.before-encryption", Path);
    if (GetFileAttributesW(Backup) == INVALID_FILE_ATTRIBUTES)
    {
        Error = RegSaveKeyW(Root, Backup, NULL);
        if (Error) goto Done;
    }
    Error = RegOpenKeyExW(Root, L"Objects", 0, KEY_ENUMERATE_SUB_KEYS, &Objects);
    if (Error) goto Done;
    PartitionDevice(Boot, Device);
    for (Index = 0; ; ++Index)
    {
        Size = _countof(Id);
        Error = RegEnumKeyExW(Objects, Index, Id, &Size, NULL, NULL, NULL, NULL);
        if (Error == ERROR_NO_MORE_ITEMS)
        {
            Error = Matches ? ERROR_SUCCESS : ERROR_NOT_FOUND;
            break;
        }
        if (Error) break;
        StringCchPrintfW(KeyPath, _countof(KeyPath), L"Objects\\%s\\Description", Id);
        Size = sizeof(Type);
        if (RegGetValueW(Root, KeyPath, L"Type", RRF_RT_REG_DWORD, NULL, &Type, &Size) || Type != 0x10200003)
            continue;
        Ours = !_wcsicmp(Id, DC_BCD_SYSTEM) || !_wcsicmp(Id, DC_BCD_RECOVERY);
        Size = sizeof(Existing);
        if (Volume->System && !ReadElement(Root, Id, 0x21000001, RRF_RT_REG_BINARY, Existing, &Size) &&
            DeviceMatches(Existing, Size, Volume))
            Ours = TRUE;
        if (!Ours) continue;
        Size = sizeof(Loader);
        Error = ReadElement(Root, Id, 0x12000002, RRF_RT_REG_SZ, Loader, &Size);
        if (Error) break;
        if (_wcsicmp(Loader, Bios ? L"\\Windows\\system32\\winload.exe" : L"\\Windows\\system32\\winload.efi") &&
            _wcsicmp(Loader, Bios ? L"\\Boot\\winload.exe" : L"\\EFI\\ReliefWin\\Boot\\winload.efi"))
        {
            Error = ERROR_NOT_SUPPORTED;
            break;
        }
        Error = CopyLoader(Store, Bios ? L"winload.exe" : L"winload.efi", Loader + 1);
        if (Error) break;
        StringCchPrintfW(KeyPath, _countof(KeyPath), L"Objects\\%s\\Elements\\11000001", Id);
        Error = RegCreateKeyExW(Root, KeyPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &Key, NULL);
        if (!Error) Error = RegSetValueExW(Key, L"Element", 0, REG_BINARY, Device, sizeof(Device));
        if (Key) RegCloseKey(Key);
        Key = NULL;
        if (Error) break;
        ++Matches;
    }
    if (!Error) Error = RegFlushKey(Root);
Done:
    if (Objects) RegCloseKey(Objects);
    if (Root) RegCloseKey(Root);
    if (Loaded)
    {
        DWORD Unload = RegUnLoadKeyW(HKEY_LOCAL_MACHINE, DC_BCD_MOUNT);
        if (!Error) Error = Unload;
    }
    return Error;
}

DWORD DcPrepareBoot(DC_STORE *Store, DC_VOLUME *Volume)
{
    DC_VOLUME *Volumes = DcOps.Allocate(NULL, sizeof(DC_VOLUME) * DC_MAX_VOLUMES), *Boot = NULL;
    WCHAR Path[DC_PATH];
    DWORD Count, i, Error, Found = 0;
    if (!Volumes) return ERROR_NOT_ENOUGH_MEMORY;
    Error = DcEnumerate(Volumes, &Count);
    if (Error) goto Done;
    for (i = 0; i < Count; ++i)
        if (!_wcsicmp(Volumes[i].Root, Store->Root)) Boot = Volumes + i;
    if (!Boot || !Boot->Boot)
    {
        Error = ERROR_NOT_FOUND;
        goto Done;
    }
    if (!Privilege(SE_BACKUP_NAME) || !Privilege(SE_RESTORE_NAME))
    {
        Error = ERROR_PRIVILEGE_NOT_HELD;
        goto Done;
    }
    StringCchPrintfW(Path, DC_PATH, L"%sWindows", Store->Root);
    CreateDirectoryW(Path, NULL);
    StringCchCatW(Path, DC_PATH, L"\\system32");
    if (!CreateDirectoryW(Path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        Error = GetLastError();
        goto Done;
    }
    for (i = 0; i != 2; ++i)
    {
        StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root, i ? L"Boot\\BCD" : L"EFI\\ReliefWin\\Boot\\BCD");
        if (GetFileAttributesW(Path) == INVALID_FILE_ATTRIBUTES) continue;
        ++Found;
        Error = PrepareStore(Store, Boot, Volume, i != 0);
        if (Error) goto Done;
    }
    if (!Found) Error = ERROR_FILE_NOT_FOUND;
Done:
    DcOps.Free(NULL, Volumes, sizeof(DC_VOLUME) * DC_MAX_VOLUMES);
    return Error;
}
