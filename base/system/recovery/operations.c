/*
 * PROJECT:     ReliefWin Recovery Center
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Offline driver changes and validated boot configuration backups
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "recovery.h"
#include <strsafe.h>

static DWORD
RcLoadHive(PCWSTR Path, REGSAM Access, PHKEY Hive)
{
    ULONGLONG Deadline = GetTickCount64() + 2000;
    DWORD Error;

    do
    {
        Error = RegLoadAppKeyW(Path, Hive, Access, REG_PROCESS_APPKEY, 0);
        if (Error != ERROR_SHARING_VIOLATION) break;
        Sleep(10);
    } while (GetTickCount64() < Deadline);
    return Error;
}

static DWORD
RcWaitForHiveClose(PCWSTR Path)
{
    ULONGLONG Deadline = GetTickCount64() + 2000;
    HANDLE File;
    DWORD Error;

    do
    {
        File = CreateFileW(Path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (File != INVALID_HANDLE_VALUE)
        {
            CloseHandle(File);
            return ERROR_SUCCESS;
        }
        Error = GetLastError();
        if (Error != ERROR_SHARING_VIOLATION) break;
        Sleep(10);
    } while (GetTickCount64() < Deadline);
    return Error;
}

static DWORD
RcDword(HKEY Key, PCWSTR Name, PDWORD Value)
{
    DWORD Type, Size = sizeof(*Value);
    LONG Error = RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)Value, &Size);

    if (Error == ERROR_SUCCESS && (Type != REG_DWORD || Size != sizeof(*Value)))
        return ERROR_INVALID_DATA;
    return Error;
}

DWORD
RcOpenSystem(PCWSTR Windows, BOOL Write, PHKEY Hive, PHKEY Services)
{
    WCHAR Path[MAX_PATH], Current[MAX_PATH], ControlSet[64];
    HKEY Select;
    DWORD Number = 0, Error;
    HANDLE File, Running;
    BY_HANDLE_FILE_INFORMATION Candidate, Active;

    *Hive = NULL;
    *Services = NULL;
    if (FAILED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"%s\\system32\\config\\SYSTEM", Windows)) ||
        !GetWindowsDirectoryW(Current, ARRAYSIZE(Current)) ||
        FAILED(StringCchCatW(Current, ARRAYSIZE(Current), L"\\system32\\config\\SYSTEM")))
        return ERROR_INVALID_NAME;
    File = CreateFileW(Path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (!GetFileInformationByHandle(File, &Candidate))
    {
        Error = GetLastError();
        CloseHandle(File);
        return Error;
    }
    CloseHandle(File);
    Running = CreateFileW(Current, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          NULL, OPEN_EXISTING, 0, NULL);
    if (Running == INVALID_HANDLE_VALUE)
        return GetLastError();
    Error = GetFileInformationByHandle(Running, &Active) ? ERROR_SUCCESS : GetLastError();
    CloseHandle(Running);
    if (Error != ERROR_SUCCESS)
        return Error;
    if (Candidate.dwVolumeSerialNumber == Active.dwVolumeSerialNumber &&
        Candidate.nFileIndexHigh == Active.nFileIndexHigh && Candidate.nFileIndexLow == Active.nFileIndexLow)
        return ERROR_ACCESS_DENIED;
    Error = RcLoadHive(Path, Write ? KEY_ALL_ACCESS : KEY_READ, Hive);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegOpenKeyExW(*Hive, L"Select", 0, KEY_QUERY_VALUE, &Select);
    if (Error == ERROR_SUCCESS)
    {
        Error = RcDword(Select, L"Default", &Number);
        RegCloseKey(Select);
    }
    if (Error == ERROR_SUCCESS && (Number == 0 || Number > 999))
        Error = ERROR_INVALID_DATA;
    if (Error == ERROR_SUCCESS)
    {
        StringCchPrintfW(ControlSet, ARRAYSIZE(ControlSet), L"ControlSet%03lu\\Services", Number);
        Error = RegOpenKeyExW(*Hive, ControlSet, 0, Write ? KEY_ALL_ACCESS : KEY_READ, Services);
    }
    if (Error != ERROR_SUCCESS)
    {
        RegCloseKey(*Hive);
        *Hive = NULL;
    }
    return Error;
}

DWORD
RcReadDriver(HKEY Services, PCWSTR Name, RC_DRIVER *Driver)
{
    HKEY Key, Overrides;
    DWORD Error, Type, Size, Original;

    ZeroMemory(Driver, sizeof(*Driver));
    if (wcschr(Name, L'\\') || wcschr(Name, L'/') || !Name[0] ||
        FAILED(StringCchCopyW(Driver->Name, ARRAYSIZE(Driver->Name), Name)))
        return ERROR_INVALID_NAME;
    Error = RegOpenKeyExW(Services, Name, 0, KEY_READ, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RcDword(Key, L"Type", &Type);
    if (Error == ERROR_FILE_NOT_FOUND)
        Error = ERROR_NOT_SUPPORTED;
    if (Error == ERROR_SUCCESS && !(Type & SERVICE_DRIVER))
        Error = ERROR_NOT_SUPPORTED;
    if (Error == ERROR_SUCCESS)
        Error = RcDword(Key, L"Start", &Driver->Start);
    if (Error == ERROR_SUCCESS && Driver->Start > SERVICE_DISABLED)
        Error = ERROR_INVALID_DATA;
    if (Error == ERROR_SUCCESS)
        Error = RcDword(Key, L"ErrorControl", &Driver->ErrorControl);
    if (Error == ERROR_SUCCESS)
    {
        Driver->Protected = Driver->Start == SERVICE_BOOT_START || Driver->ErrorControl >= SERVICE_ERROR_SEVERE;
        Error = RegOpenKeyExW(Key, L"StartOverride", 0, KEY_READ, &Overrides);
        if (Error == ERROR_SUCCESS)
        {
            Driver->Protected = TRUE;
            RegCloseKey(Overrides);
        }
        else if (Error != ERROR_FILE_NOT_FOUND)
            Driver->Protected = TRUE;
        Error = ERROR_SUCCESS;
        Driver->CanUndo = RcDword(Key, RC_BACKUP_VALUE, &Original) == ERROR_SUCCESS &&
                          Original <= SERVICE_DEMAND_START &&
                          (Driver->Start == SERVICE_DISABLED || Driver->Start == Original);
        Size = sizeof(Driver->Display);
        if (RegQueryValueExW(Key, L"DisplayName", NULL, &Type, (PBYTE)Driver->Display, &Size) != ERROR_SUCCESS ||
            Type != REG_SZ || Size % sizeof(WCHAR))
            Driver->Display[0] = 0;
        Driver->Display[ARRAYSIZE(Driver->Display) - 1] = 0;
        if (!Driver->Display[0])
            StringCchCopyW(Driver->Display, ARRAYSIZE(Driver->Display), Name);
    }
    RegCloseKey(Key);
    return Error;
}

DWORD
RcChangeDriver(HKEY Services, PCWSTR Name, BOOL Undo)
{
    RC_DRIVER Driver;
    HKEY Key;
    DWORD Error, Original, Start, Size, Type;

    Error = RcReadDriver(Services, Name, &Driver);
    if (Error != ERROR_SUCCESS)
        return Error;
    if ((!Undo && Driver.Protected) || (Undo && !Driver.CanUndo))
        return ERROR_ACCESS_DENIED;
    if (!Undo && Driver.Start == SERVICE_DISABLED)
        return ERROR_ALREADY_EXISTS;
    Error = RegOpenKeyExW(Services, Name, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RcDword(Key, L"Start", &Start);
    if (Error == ERROR_SUCCESS && Start != Driver.Start)
        Error = ERROR_RETRY;
    if (Error == ERROR_SUCCESS && Undo)
    {
        Error = RcDword(Key, RC_BACKUP_VALUE, &Original);
        if (Error == ERROR_SUCCESS && (Original > SERVICE_DEMAND_START ||
            (Start != SERVICE_DISABLED && Start != Original)))
            Error = ERROR_INVALID_DATA;
        if (Error == ERROR_SUCCESS)
            Error = RegSetValueExW(Key, L"Start", 0, REG_DWORD, (PBYTE)&Original, sizeof(Original));
        if (Error == ERROR_SUCCESS)
            Error = RegFlushKey(Key);
        if (Error == ERROR_SUCCESS)
            Error = RegDeleteValueW(Key, RC_BACKUP_VALUE);
    }
    else if (Error == ERROR_SUCCESS)
    {
        Size = sizeof(Original);
        Error = RegQueryValueExW(Key, RC_BACKUP_VALUE, NULL, &Type, (PBYTE)&Original, &Size);
        if (Error == ERROR_SUCCESS)
        {
            if (Type != REG_DWORD || Size != sizeof(Original) || Original != Start)
                Error = ERROR_ALREADY_EXISTS;
        }
        else if (Error == ERROR_FILE_NOT_FOUND)
        {
            Original = Start;
            Error = RegSetValueExW(Key, RC_BACKUP_VALUE, 0, REG_DWORD, (PBYTE)&Original, sizeof(Original));
        }
        if (Error == ERROR_SUCCESS)
            Error = RegFlushKey(Key);
        if (Error == ERROR_SUCCESS)
        {
            Start = SERVICE_DISABLED;
            Error = RegSetValueExW(Key, L"Start", 0, REG_DWORD, (PBYTE)&Start, sizeof(Start));
        }
    }
    if (Error == ERROR_SUCCESS)
        Error = RegFlushKey(Key);
    RegCloseKey(Key);
    return Error;
}

static DWORD
RcInspectBcd(PCWSTR Path)
{
    HKEY Hive, Key;
    WCHAR Default[40], Name[128], Mount[64];
    DWORD Error, UnloadError, Type, Size = sizeof(Default), ObjectType, PreviousSize;
    HANDLE Token;
    struct { DWORD PrivilegeCount; LUID_AND_ATTRIBUTES Privileges[2]; } Privilege = {0}, Previous;
    ULONG Index;
    BOOL Adjusted;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token))
        return GetLastError();
    Privilege.PrivilegeCount = 2;
    if (!LookupPrivilegeValueW(NULL, SE_RESTORE_NAME, &Privilege.Privileges[0].Luid) ||
        !LookupPrivilegeValueW(NULL, SE_BACKUP_NAME, &Privilege.Privileges[1].Luid))
    {
        Error = GetLastError();
        CloseHandle(Token);
        return Error;
    }
    Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    Privilege.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;
    PreviousSize = sizeof(Previous);
    SetLastError(ERROR_SUCCESS);
    Adjusted = AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Privilege, sizeof(Previous), (PTOKEN_PRIVILEGES)&Previous, &PreviousSize);
    Error = GetLastError();
    if (!Adjusted || Error != ERROR_SUCCESS)
    {
        if (Adjusted) AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Previous, 0, NULL, NULL);
        CloseHandle(Token);
        return Error;
    }
    StringCchPrintfW(Mount, ARRAYSIZE(Mount), L"ReliefWinRecoveryBcd-%lu-%lu", GetCurrentProcessId(), GetCurrentThreadId());
    Error = RegLoadKeyW(HKEY_LOCAL_MACHINE, Mount, Path);
    if (Error != ERROR_SUCCESS)
    {
        AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Previous, 0, NULL, NULL);
        CloseHandle(Token);
        return Error;
    }
    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, Mount, REG_OPTION_BACKUP_RESTORE, KEY_READ, &Hive);
    if (Error != ERROR_SUCCESS)
    {
        RegUnLoadKeyW(HKEY_LOCAL_MACHINE, Mount);
        AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Previous, 0, NULL, NULL);
        CloseHandle(Token);
        return Error;
    }
    Error = RegOpenKeyExW(Hive, L"Objects\\" RC_BCD_MANAGER L"\\Elements\\23000003", 0, KEY_QUERY_VALUE, &Key);
    if (Error == ERROR_SUCCESS)
    {
        Error = RegQueryValueExW(Key, L"Element", NULL, &Type, (PBYTE)Default, &Size);
        RegCloseKey(Key);
        if (Error == ERROR_SUCCESS && (Type != REG_SZ || Size != 39 * sizeof(WCHAR) || Default[38] != 0 ||
                                      Default[0] != L'{' || Default[37] != L'}'))
            Error = ERROR_INVALID_DATA;
    }
    for (Index = 1; Error == ERROR_SUCCESS && Index < 37; Index++)
    {
        WCHAR Ch = Default[Index];
        if (Index == 9 || Index == 14 || Index == 19 || Index == 24)
        {
            if (Ch != L'-') Error = ERROR_INVALID_DATA;
        }
        else if (!((Ch >= L'0' && Ch <= L'9') || (Ch >= L'a' && Ch <= L'f') || (Ch >= L'A' && Ch <= L'F')))
            Error = ERROR_INVALID_DATA;
    }
    if (Error == ERROR_SUCCESS)
    {
        StringCchPrintfW(Name, ARRAYSIZE(Name), L"Objects\\%s\\Description", Default);
        Error = RegOpenKeyExW(Hive, Name, 0, KEY_QUERY_VALUE, &Key);
        if (Error == ERROR_SUCCESS)
        {
            Error = RcDword(Key, L"Type", &ObjectType);
            RegCloseKey(Key);
            if (Error == ERROR_SUCCESS && ObjectType != 0x10200003)
                Error = ERROR_INVALID_DATA;
        }
    }
    RegCloseKey(Hive);
    UnloadError = RegUnLoadKeyW(HKEY_LOCAL_MACHINE, Mount);
    if (Error == ERROR_SUCCESS) Error = UnloadError;
    AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Previous, 0, NULL, NULL);
    CloseHandle(Token);
    return Error;
}

DWORD
RcValidateBcd(PCWSTR Path)
{
    const PCWSTR Suffixes[] = {L"", L".LOG", L".LOG1", L".LOG2"};
    WCHAR Directory[MAX_PATH], Temp[MAX_PATH], File[MAX_PATH + 8];
    DWORD Length, Error, CloseError, Index;

    Length = GetTempPathW(ARRAYSIZE(Directory), Directory);
    if (!Length || Length >= ARRAYSIZE(Directory)) return ERROR_INVALID_NAME;
    if (!GetTempFileNameW(Directory, L"rwc", 0, Temp)) return GetLastError();
    Error = CopyFileW(Path, Temp, FALSE) ? ERROR_SUCCESS : GetLastError();
    if (Error == ERROR_SUCCESS && !SetFileAttributesW(Temp, FILE_ATTRIBUTE_NORMAL))
        Error = GetLastError();
    if (Error == ERROR_SUCCESS)
        Error = RcInspectBcd(Temp);
    CloseError = RcWaitForHiveClose(Temp);
    if (Error == ERROR_SUCCESS) Error = CloseError;
    for (Index = 0; Index < ARRAYSIZE(Suffixes); Index++)
    {
        StringCchPrintfW(File, ARRAYSIZE(File), L"%s%s", Temp, Suffixes[Index]);
        if (!DeleteFileW(File) && GetLastError() != ERROR_FILE_NOT_FOUND && Error == ERROR_SUCCESS)
            Error = GetLastError();
    }
    return Error;
}

static DWORD
RcSameFile(PCWSTR First, PCWSTR Second, BOOL *Same)
{
    WCHAR From[MAX_PATH], To[MAX_PATH];
    BY_HANDLE_FILE_INFORMATION One, Two;
    HANDLE File;
    DWORD Length, Error;

    *Same = FALSE;
    Length = GetFullPathNameW(First, ARRAYSIZE(From), From, NULL);
    if (!Length || Length >= ARRAYSIZE(From))
        return ERROR_INVALID_NAME;
    Length = GetFullPathNameW(Second, ARRAYSIZE(To), To, NULL);
    if (!Length || Length >= ARRAYSIZE(To))
        return ERROR_INVALID_NAME;
    if (!_wcsicmp(From, To))
    {
        *Same = TRUE;
        return ERROR_SUCCESS;
    }
    File = CreateFileW(First, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND ? ERROR_SUCCESS : Error;
    }
    Error = GetFileInformationByHandle(File, &One) ? ERROR_SUCCESS : GetLastError();
    CloseHandle(File);
    if (Error != ERROR_SUCCESS) return Error;
    File = CreateFileW(Second, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND ? ERROR_SUCCESS : Error;
    }
    Error = GetFileInformationByHandle(File, &Two) ? ERROR_SUCCESS : GetLastError();
    CloseHandle(File);
    if (Error == ERROR_SUCCESS)
        *Same = One.dwVolumeSerialNumber == Two.dwVolumeSerialNumber &&
                One.nFileIndexHigh == Two.nFileIndexHigh && One.nFileIndexLow == Two.nFileIndexLow;
    return Error;
}

static DWORD
RcCopyAtomic(PCWSTR Source, PCWSTR Destination, BOOL Validate, BOOL SaveCurrent)
{
    WCHAR Temp[MAX_PATH], Previous[MAX_PATH];
    HANDLE Lock, File;
    DWORD Error;
    BOOL Same;

    Error = RcSameFile(Source, Destination, &Same);
    if (Error != ERROR_SUCCESS) return Error;
    if (Same)
        return ERROR_INVALID_PARAMETER;
    if (FAILED(StringCchPrintfW(Temp, ARRAYSIZE(Temp), L"%s.ReliefWin.tmp", Destination)))
        return ERROR_INVALID_NAME;
    Lock = CreateFileW(Source, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (Lock == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (!CopyFileW(Source, Temp, TRUE))
    {
        Error = GetLastError();
        CloseHandle(Lock);
        return Error;
    }
    CloseHandle(Lock);
    Error = SetFileAttributesW(Temp, FILE_ATTRIBUTE_NORMAL) ? ERROR_SUCCESS : GetLastError();
    if (Error == ERROR_SUCCESS && Validate)
        Error = RcValidateBcd(Temp);
    if (Error == ERROR_SUCCESS)
    {
        File = CreateFileW(Temp, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (File == INVALID_HANDLE_VALUE)
            Error = GetLastError();
        else
        {
            if (!FlushFileBuffers(File)) Error = GetLastError();
            CloseHandle(File);
        }
    }
    if (Error == ERROR_SUCCESS && SaveCurrent)
    {
        if (FAILED(StringCchPrintfW(Previous, ARRAYSIZE(Previous), L"%s.ReliefWin.before-restore", Destination)))
            Error = ERROR_INVALID_NAME;
        else
        {
            Error = RcCopyAtomic(Destination, Previous, FALSE, FALSE);
            if (Error == ERROR_FILE_NOT_FOUND) Error = ERROR_SUCCESS;
        }
    }
    if (Error == ERROR_SUCCESS && !MoveFileExW(Temp, Destination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        Error = GetLastError();
    if (Error != ERROR_SUCCESS)
        DeleteFileW(Temp);
    return Error;
}

DWORD
RcBackupBcd(PCWSTR Path, PCWSTR Backup)
{
    return RcCopyAtomic(Path, Backup, TRUE, FALSE);
}

DWORD
RcRestoreBcd(PCWSTR Path, PCWSTR Backup)
{
    return RcCopyAtomic(Backup, Path, TRUE, TRUE);
}

DWORD
RcReadLog(PCWSTR Path, PWSTR *Text, BOOL *Truncated)
{
    const DWORD Limit = 256 * 1024;
    HANDLE File;
    LARGE_INTEGER Size, Offset;
    PBYTE Bytes;
    BYTE Header[3];
    DWORD Read, HeaderSize, Error = ERROR_SUCCESS, Skip = 0;
    int Count;
    UINT Encoding = CP_UTF8;
    BOOL Unicode;

    *Text = NULL;
    *Truncated = FALSE;
    File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (File == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (!GetFileSizeEx(File, &Size))
    {
        Error = GetLastError();
        CloseHandle(File);
        return Error;
    }
    *Truncated = Size.QuadPart > Limit;
    if (!ReadFile(File, Header, sizeof(Header), &HeaderSize, NULL))
    {
        Error = GetLastError();
        CloseHandle(File);
        return Error;
    }
    Unicode = HeaderSize >= 2 && Header[0] == 0xff && Header[1] == 0xfe;
    Offset.QuadPart = *Truncated ? Size.QuadPart - Limit : 0;
    if (Unicode) Offset.QuadPart &= ~(LONGLONG)1;
    if (!SetFilePointerEx(File, Offset, NULL, FILE_BEGIN))
    {
        Error = GetLastError();
        CloseHandle(File);
        return Error;
    }
    Bytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Limit + 2);
    if (!Bytes)
    {
        CloseHandle(File);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    if (!ReadFile(File, Bytes, Limit, &Read, NULL))
        Error = GetLastError();
    CloseHandle(File);
    if (Error != ERROR_SUCCESS)
    {
        HeapFree(GetProcessHeap(), 0, Bytes);
        return Error;
    }
    if (Unicode)
    {
        Skip = !*Truncated && Read >= 2 ? 2 : 0;
        *Text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Read + sizeof(WCHAR));
        if (*Text) CopyMemory(*Text, Bytes + Skip, (Read - Skip) & ~1u);
    }
    else
    {
        if (Read >= 3 && Bytes[0] == 0xef && Bytes[1] == 0xbb && Bytes[2] == 0xbf) Skip = 3;
        Count = Read > Skip ? MultiByteToWideChar(Encoding, MB_ERR_INVALID_CHARS, (PCSTR)Bytes + Skip, Read - Skip, NULL, 0) : 0;
        if (!Count && Read > Skip)
        {
            Encoding = CP_ACP;
            Count = MultiByteToWideChar(Encoding, 0, (PCSTR)Bytes + Skip, Read - Skip, NULL, 0);
        }
        *Text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (Count + 1) * sizeof(WCHAR));
        if (*Text && Count)
            MultiByteToWideChar(Encoding, 0, (PCSTR)Bytes + Skip, Read - Skip, *Text, Count);
    }
    HeapFree(GetProcessHeap(), 0, Bytes);
    return *Text ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
}
