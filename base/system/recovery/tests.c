/*
 * PROJECT:     ReliefWin Recovery tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify offline recovery state and failure handling
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "recovery.h"
#include <stdio.h>
#include <strsafe.h>

static unsigned Checks, Failures;
#define CHECK(Condition) do { ++Checks; if (!(Condition)) { ++Failures; printf("FAIL line %u: %s (error %lu)\n", __LINE__, #Condition, GetLastError()); } } while (0)

static VOID
SetDword(HKEY Key, PCWSTR Name, DWORD Value)
{
    CHECK(RegSetValueExW(Key, Name, 0, REG_DWORD, (PBYTE)&Value, sizeof(Value)) == ERROR_SUCCESS);
}

static DWORD
GetDword(HKEY Key, PCWSTR Name)
{
    DWORD Value = 0xdeadbeef, Type, Size = sizeof(Value);
    CHECK(RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)&Value, &Size) == ERROR_SUCCESS);
    CHECK(Type == REG_DWORD && Size == sizeof(Value));
    return Value;
}

static HKEY
MakeDriver(HKEY Services, PCWSTR Name, DWORD Start, DWORD ErrorControl)
{
    HKEY Key = NULL;
    CHECK(RegCreateKeyExW(Services, Name, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Key, NULL) == ERROR_SUCCESS);
    if (!Key) return NULL;
    SetDword(Key, L"Type", SERVICE_KERNEL_DRIVER);
    SetDword(Key, L"Start", Start);
    SetDword(Key, L"ErrorControl", ErrorControl);
    SetDword(Key, L"Unrelated", 0x12345678);
    return Key;
}

static VOID
DriverTests(HKEY Hive)
{
    HKEY Services, Driver, Override;
    RC_DRIVER Info;
    DWORD Index;

    CHECK(RegCreateKeyExW(Hive, L"ControlSet002\\Services", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Services, NULL) == ERROR_SUCCESS);
    for (Index = SERVICE_SYSTEM_START; Index <= SERVICE_DEMAND_START; Index++)
    {
        Driver = MakeDriver(Services, L"TestDriver", Index, SERVICE_ERROR_NORMAL);
        CHECK(RcReadDriver(Services, L"TestDriver", &Info) == ERROR_SUCCESS);
        CHECK(!Info.Protected && !Info.CanUndo);
        CHECK(RcChangeDriver(Services, L"TestDriver", FALSE) == ERROR_SUCCESS);
        CHECK(GetDword(Driver, L"Start") == SERVICE_DISABLED);
        CHECK(GetDword(Driver, RC_BACKUP_VALUE) == Index);
        CHECK(RcChangeDriver(Services, L"TestDriver", FALSE) == ERROR_ALREADY_EXISTS);
        CHECK(GetDword(Driver, RC_BACKUP_VALUE) == Index);
        CHECK(RcReadDriver(Services, L"TestDriver", &Info) == ERROR_SUCCESS && Info.CanUndo);
        CHECK(RcChangeDriver(Services, L"TestDriver", TRUE) == ERROR_SUCCESS);
        CHECK(GetDword(Driver, L"Start") == Index);
        CHECK(GetDword(Driver, L"Unrelated") == 0x12345678);
        CHECK(RcChangeDriver(Services, L"TestDriver", TRUE) == ERROR_ACCESS_DENIED);
        RegCloseKey(Driver);
    }
    Driver = MakeDriver(Services, L"BootDriver", SERVICE_BOOT_START, SERVICE_ERROR_NORMAL);
    CHECK(RcChangeDriver(Services, L"BootDriver", FALSE) == ERROR_ACCESS_DENIED);
    CHECK(GetDword(Driver, L"Start") == SERVICE_BOOT_START);
    RegCloseKey(Driver);
    Driver = MakeDriver(Services, L"SevereDriver", SERVICE_DEMAND_START, SERVICE_ERROR_SEVERE);
    CHECK(RcChangeDriver(Services, L"SevereDriver", FALSE) == ERROR_ACCESS_DENIED);
    RegCloseKey(Driver);
    Driver = MakeDriver(Services, L"Overridden", SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL);
    CHECK(RegCreateKeyExW(Driver, L"StartOverride", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Override, NULL) == ERROR_SUCCESS);
    SetDword(Override, L"0", SERVICE_BOOT_START);
    CHECK(RcChangeDriver(Services, L"Overridden", FALSE) == ERROR_ACCESS_DENIED);
    RegCloseKey(Override);
    RegCloseKey(Driver);
    Driver = MakeDriver(Services, L"Disabled", SERVICE_DISABLED, SERVICE_ERROR_NORMAL);
    CHECK(RcChangeDriver(Services, L"Disabled", FALSE) == ERROR_ALREADY_EXISTS);
    CHECK(!RcReadDriver(Services, L"Disabled", &Info) && !Info.CanUndo);
    RegCloseKey(Driver);
    Driver = MakeDriver(Services, L"Interrupted", SERVICE_SYSTEM_START, SERVICE_ERROR_NORMAL);
    SetDword(Driver, RC_BACKUP_VALUE, SERVICE_SYSTEM_START);
    CHECK(RcChangeDriver(Services, L"Interrupted", FALSE) == ERROR_SUCCESS);
    SetDword(Driver, L"Start", SERVICE_AUTO_START);
    CHECK(RcChangeDriver(Services, L"Interrupted", TRUE) == ERROR_ACCESS_DENIED);
    CHECK(GetDword(Driver, L"Start") == SERVICE_AUTO_START);
    SetDword(Driver, L"Start", SERVICE_DISABLED);
    CHECK(RcChangeDriver(Services, L"Interrupted", TRUE) == ERROR_SUCCESS);
    RegCloseKey(Driver);
    CHECK(RcReadDriver(Services, L"..\\Select", &Info) == ERROR_INVALID_NAME);
    CHECK(RcReadDriver(Services, L"", &Info) == ERROR_INVALID_NAME);
    CHECK(RcChangeDriver(Services, L"Missing", FALSE) == ERROR_FILE_NOT_FOUND);
    CHECK(RegCreateKeyExW(Services, L"Configuration", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Driver, NULL) == ERROR_SUCCESS);
    CHECK(RcReadDriver(Services, L"Configuration", &Info) == ERROR_NOT_SUPPORTED);
    CHECK(RcChangeDriver(Services, L"Configuration", FALSE) == ERROR_NOT_SUPPORTED);
    RegCloseKey(Driver);
    RegCloseKey(Services);
}

static BOOL
SameFiles(PCWSTR First, PCWSTR Second)
{
    HANDLE A, B;
    BYTE One[4096], Two[4096];
    DWORD CountA, CountB;
    BOOL Same = TRUE;
    A = CreateFileW(First, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    B = CreateFileW(Second, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (A == INVALID_HANDLE_VALUE || B == INVALID_HANDLE_VALUE) Same = FALSE;
    while (Same)
    {
        if (!ReadFile(A, One, sizeof(One), &CountA, NULL) || !ReadFile(B, Two, sizeof(Two), &CountB, NULL) ||
            CountA != CountB || memcmp(One, Two, CountA))
            Same = FALSE;
        if (!Same || CountA == 0) break;
    }
    if (A != INVALID_HANDLE_VALUE) CloseHandle(A);
    if (B != INVALID_HANDLE_VALUE) CloseHandle(B);
    return Same;
}

static VOID
DeleteDirectory(PCWSTR Path)
{
    WCHAR Pattern[MAX_PATH], Child[MAX_PATH];
    WIN32_FIND_DATAW Data;
    HANDLE Find;
    StringCchPrintfW(Pattern, ARRAYSIZE(Pattern), L"%s\\*", Path);
    Find = FindFirstFileW(Pattern, &Data);
    if (Find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!wcscmp(Data.cFileName, L".") || !wcscmp(Data.cFileName, L"..")) continue;
            StringCchPrintfW(Child, ARRAYSIZE(Child), L"%s\\%s", Path, Data.cFileName);
            if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) DeleteDirectory(Child);
            else DeleteFileW(Child);
        } while (FindNextFileW(Find, &Data));
        FindClose(Find);
    }
    RemoveDirectoryW(Path);
}

int wmain(int Count, WCHAR **Arguments)
{
    WCHAR KeyName[128], Temp[MAX_PATH], Root[MAX_PATH], Windows[MAX_PATH], Path[MAX_PATH];
    WCHAR Bcd[MAX_PATH], Backup[MAX_PATH], Previous[MAX_PATH], Log[MAX_PATH], Current[MAX_PATH];
    const WCHAR OsId[] = L"{354fa6e5-5e45-49af-91bf-1fd2754e470a}";
    HKEY Hive = NULL, Key, Loaded, Services;
    HANDLE Token, File;
    TOKEN_PRIVILEGES Privileges = {0};
    DWORD Error, Written, Disposition;
    PWSTR Contents;
    BOOL Truncated;
    BYTE LongLog[4096];
    unsigned Index;

    for (Index = 1; Index < (unsigned)Count; Index++)
        CHECK(RcValidateBcd(Arguments[Index]) == ERROR_SUCCESS);
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token)) return 2;
    Privileges.PrivilegeCount = 1;
    LookupPrivilegeValueW(NULL, SE_BACKUP_NAME, &Privileges.Privileges[0].Luid);
    Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    SetLastError(ERROR_SUCCESS);
    AdjustTokenPrivileges(Token, FALSE, &Privileges, 0, NULL, NULL);
    Error = GetLastError();
    CloseHandle(Token);
    if (Error != ERROR_SUCCESS) { printf("Backup privilege required: %lu\n", Error); return 2; }
    GetTempPathW(ARRAYSIZE(Temp), Temp);
    if (!GetTempFileNameW(Temp, L"rwc", 0, Root)) return 2;
    DeleteFileW(Root);
    if (!CreateDirectoryW(Root, NULL)) return 2;
    StringCchPrintfW(KeyName, ARRAYSIZE(KeyName), L"Software\\ReliefWin\\RecoveryTests-%lu", GetCurrentProcessId());
    Error = RegCreateKeyExW(HKEY_CURRENT_USER, KeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Hive, &Disposition);
    if (Error != ERROR_SUCCESS || Disposition != REG_CREATED_NEW_KEY) return 2;
    DriverTests(Hive);
    CHECK(RegCreateKeyExW(Hive, L"Select", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Key, NULL) == ERROR_SUCCESS);
    SetDword(Key, L"Current", 1);
    SetDword(Key, L"Default", 2);
    RegCloseKey(Key);
    StringCchPrintfW(Windows, ARRAYSIZE(Windows), L"%s\\Windows", Root);
    CreateDirectoryW(Windows, NULL);
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%s\\system32", Windows); CreateDirectoryW(Path, NULL);
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%s\\system32\\config", Windows); CreateDirectoryW(Path, NULL);
    StringCchCatW(Path, ARRAYSIZE(Path), L"\\SYSTEM");
    CHECK(RegSaveKeyW(Hive, Path, NULL) == ERROR_SUCCESS);
    Error = RcOpenSystem(Windows, TRUE, &Loaded, &Services);
    CHECK(Error == ERROR_SUCCESS);
    if (Error == ERROR_SUCCESS)
    {
        CHECK(RcChangeDriver(Services, L"TestDriver", FALSE) == ERROR_SUCCESS);
        RegCloseKey(Services);
        RegCloseKey(Loaded);
    }
    Error = RcOpenSystem(Windows, TRUE, &Loaded, &Services);
    CHECK(Error == ERROR_SUCCESS);
    if (Error == ERROR_SUCCESS)
    {
        CHECK(RegOpenKeyExW(Services, L"TestDriver", 0, KEY_READ, &Key) == ERROR_SUCCESS);
        CHECK(GetDword(Key, L"Start") == SERVICE_DISABLED);
        CHECK(GetDword(Key, RC_BACKUP_VALUE) == SERVICE_DEMAND_START);
        RegCloseKey(Key);
        CHECK(RcChangeDriver(Services, L"TestDriver", TRUE) == ERROR_SUCCESS);
        RegCloseKey(Services);
        RegCloseKey(Loaded);
    }
    GetWindowsDirectoryW(Current, ARRAYSIZE(Current));
    CHECK(RcOpenSystem(Current, TRUE, &Loaded, &Services) == ERROR_ACCESS_DENIED);
    CHECK(Loaded == NULL && Services == NULL);

    CHECK(RegCreateKeyExW(Hive, L"Objects\\" RC_BCD_MANAGER L"\\Elements\\23000003", 0, NULL, 0,
                           KEY_ALL_ACCESS, NULL, &Key, NULL) == ERROR_SUCCESS);
    CHECK(RegSetValueExW(Key, L"Element", 0, REG_SZ, (PBYTE)OsId, sizeof(OsId)) == ERROR_SUCCESS);
    RegCloseKey(Key);
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"Objects\\%s\\Description", OsId);
    CHECK(RegCreateKeyExW(Hive, Path, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Key, NULL) == ERROR_SUCCESS);
    SetDword(Key, L"Type", 0x10200003);
    RegCloseKey(Key);
    StringCchPrintfW(Bcd, ARRAYSIZE(Bcd), L"%s\\BCD", Root);
    StringCchPrintfW(Backup, ARRAYSIZE(Backup), L"%s\\BCD.backup", Root);
    StringCchPrintfW(Previous, ARRAYSIZE(Previous), L"%s.ReliefWin.before-restore", Bcd);
    CHECK(RegSaveKeyW(Hive, Bcd, NULL) == ERROR_SUCCESS);
    CHECK(RcValidateBcd(Bcd) == ERROR_SUCCESS);
    CHECK(RcBackupBcd(Bcd, Backup) == ERROR_SUCCESS);
    CHECK(SameFiles(Bcd, Backup));
    CHECK(RcBackupBcd(Bcd, Bcd) == ERROR_INVALID_PARAMETER);
    File = CreateFileW(Bcd, GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 0, NULL);
    CHECK(File != INVALID_HANDLE_VALUE);
    CHECK(WriteFile(File, "corrupt", 7, &Written, NULL));
    CloseHandle(File);
    CHECK(RcValidateBcd(Bcd) != ERROR_SUCCESS);
    CHECK(RcRestoreBcd(Bcd, Backup) == ERROR_SUCCESS);
    CHECK(SameFiles(Bcd, Backup));
    CHECK(RcValidateBcd(Previous) != ERROR_SUCCESS);
    CHECK(RcRestoreBcd(Bcd, Previous) != ERROR_SUCCESS);
    CHECK(SameFiles(Bcd, Backup));
    CHECK(RcRestoreBcd(Bcd, Bcd) == ERROR_INVALID_PARAMETER);
    CHECK(RcBackupBcd(Backup, Previous) == ERROR_SUCCESS);
    File = CreateFileW(Bcd, GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 0, NULL);
    CHECK(File != INVALID_HANDLE_VALUE);
    CHECK(WriteFile(File, "corrupt", 7, &Written, NULL));
    CloseHandle(File);
    CHECK(RcRestoreBcd(Bcd, Previous) == ERROR_SUCCESS);
    CHECK(SameFiles(Bcd, Backup));
    CHECK(RcValidateBcd(Previous) != ERROR_SUCCESS);

    StringCchPrintfW(Log, ARRAYSIZE(Log), L"%s\\log.txt", Root);
    File = CreateFileW(Log, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    CHECK(WriteFile(File, "\xef\xbb\xbfHello\xe4\xb8\xad\xe6\x96\x87", 14, &Written, NULL));
    CloseHandle(File);
    CHECK(RcReadLog(Log, &Contents, &Truncated) == ERROR_SUCCESS);
    CHECK(!Truncated && !wcscmp(Contents, L"Hello\x4e2d\x6587"));
    HeapFree(GetProcessHeap(), 0, Contents);
    File = CreateFileW(Log, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    CHECK(WriteFile(File, "\xff\xfe", 2, &Written, NULL));
    for (Index = 0; Index < sizeof(LongLog); Index += 2) { LongLog[Index] = 'x'; LongLog[Index + 1] = 0; }
    for (Index = 0; Index < 70; Index++) CHECK(WriteFile(File, LongLog, sizeof(LongLog), &Written, NULL));
    CloseHandle(File);
    CHECK(RcReadLog(Log, &Contents, &Truncated) == ERROR_SUCCESS);
    CHECK(Truncated && wcslen(Contents) == 128 * 1024 && Contents[0] == L'x');
    HeapFree(GetProcessHeap(), 0, Contents);
    RegCloseKey(Hive);
    CHECK(RegDeleteTreeW(HKEY_CURRENT_USER, KeyName) == ERROR_SUCCESS);
    DeleteDirectory(Root);
    CHECK(GetFileAttributesW(Root) == INVALID_FILE_ATTRIBUTES);
    printf("Recovery: %u checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
