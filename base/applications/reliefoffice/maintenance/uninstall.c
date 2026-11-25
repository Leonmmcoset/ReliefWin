/*
 * PROJECT:     ReliefOffice maintenance
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Remove the installed suite while preserving user documents
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#define COBJMACROS
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include "manifest.h"

static const WCHAR OfficeUninstallKey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ReliefOffice";

static DWORD
OfficeReadString(
    _In_ HKEY Root,
    _In_ PCWSTR SubKey,
    _In_opt_ PCWSTR Name,
    _Out_writes_(Count) PWSTR Value,
    _In_ DWORD Count)
{
    HKEY Key;
    DWORD Error, Type, Size = Count * sizeof(WCHAR);

    Error = RegOpenKeyExW(Root, SubKey, 0, KEY_QUERY_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)Value, &Size);
    RegCloseKey(Key);
    if (Error == ERROR_SUCCESS &&
        (Type != REG_SZ || Size < sizeof(WCHAR) || Size > Count * sizeof(WCHAR) ||
         Size % sizeof(WCHAR) || Value[Size / sizeof(WCHAR) - 1] != UNICODE_NULL))
        Error = ERROR_INVALID_DATA;
    return Error;
}

static DWORD
OfficeDeleteFile(
    _In_ PCWSTR Directory,
    _In_ PCWSTR RelativeName)
{
    WCHAR Path[MAX_PATH];
    DWORD Error;

    if (FAILED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, RelativeName)))
        return ERROR_FILENAME_EXCED_RANGE;
    if (DeleteFileW(Path))
        return ERROR_SUCCESS;
    Error = GetLastError();
    return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND ? ERROR_SUCCESS : Error;
}

static DWORD
OfficeRemoveAssociation(
    _In_ PCWSTR Directory,
    _In_ const OFFICE_APPLICATION *Application)
{
    WCHAR KeyPath[MAX_PATH], Expected[MAX_PATH * 2], Value[MAX_PATH * 2];
    HKEY Key;
    DWORD Error;

    if (FAILED(StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath),
                                L"SOFTWARE\\Classes\\%ls\\shell\\open\\command", Application->ProgId)) ||
        FAILED(StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"\"%ls\\%ls\" \"%%1\"",
                                Directory, Application->Executable)))
        return ERROR_FILENAME_EXCED_RANGE;
    Error = OfficeReadString(HKEY_LOCAL_MACHINE, KeyPath, NULL, Value, ARRAYSIZE(Value));
    if (Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND)
        return ERROR_SUCCESS;
    if (Error != ERROR_SUCCESS)
        return Error;
    if (_wcsicmp(Value, Expected))
        return ERROR_SUCCESS;

    if (FAILED(StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath),
                                L"SOFTWARE\\Classes\\%ls", Application->Extension)))
        return ERROR_FILENAME_EXCED_RANGE;
    Error = OfficeReadString(HKEY_LOCAL_MACHINE, KeyPath, NULL, Value, ARRAYSIZE(Value));
    if (Error == ERROR_SUCCESS && !_wcsicmp(Value, Application->ProgId))
    {
        Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KeyPath, 0, KEY_SET_VALUE, &Key);
        if (Error != ERROR_SUCCESS)
            return Error;
        Error = RegDeleteValueW(Key, NULL);
        RegCloseKey(Key);
        if (Error != ERROR_SUCCESS && Error != ERROR_FILE_NOT_FOUND)
            return Error;
    }
    else if (Error != ERROR_SUCCESS && Error != ERROR_FILE_NOT_FOUND && Error != ERROR_PATH_NOT_FOUND)
    {
        return Error;
    }

    StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath), L"SOFTWARE\\Classes\\%ls", Application->ProgId);
    Error = RegDeleteTreeW(HKEY_LOCAL_MACHINE, KeyPath);
    return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND ? ERROR_SUCCESS : Error;
}

static DWORD
OfficeRemoveShortcut(
    _In_ PCWSTR Directory,
    _In_ PCWSTR Programs,
    _In_ const OFFICE_APPLICATION *Application)
{
    WCHAR LinkPath[MAX_PATH], Target[MAX_PATH], Expected[MAX_PATH];
    WIN32_FIND_DATAW Data;
    IShellLinkW *Link;
    IPersistFile *File;
    HRESULT Result;

    if (FAILED(StringCchPrintfW(LinkPath, ARRAYSIZE(LinkPath), L"%ls\\ReliefOffice\\%ls.lnk",
                                Programs, Application->Name)) ||
        FAILED(StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"%ls\\%ls",
                                Directory, Application->Executable)))
        return ERROR_FILENAME_EXCED_RANGE;
    if (GetFileAttributesW(LinkPath) == INVALID_FILE_ATTRIBUTES)
    {
        DWORD Error = GetLastError();
        return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND ? ERROR_SUCCESS : Error;
    }
    Result = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                              &IID_IShellLinkW, (PVOID *)&Link);
    if (FAILED(Result))
        return ERROR_GEN_FAILURE;
    Result = IShellLinkW_QueryInterface(Link, &IID_IPersistFile, (PVOID *)&File);
    if (SUCCEEDED(Result))
    {
        Result = IPersistFile_Load(File, LinkPath, STGM_READ);
        if (SUCCEEDED(Result))
            Result = IShellLinkW_GetPath(Link, Target, ARRAYSIZE(Target), &Data, SLGP_RAWPATH);
        IPersistFile_Release(File);
    }
    IShellLinkW_Release(Link);
    if (FAILED(Result))
        return ERROR_GEN_FAILURE;
    if (_wcsicmp(Target, Expected))
        return ERROR_SUCCESS;
    if (!DeleteFileW(LinkPath))
        return GetLastError();
    return ERROR_SUCCESS;
}

static DWORD
OfficeRestoreUninstall(
    _In_ PCWSTR Directory)
{
    static const WCHAR *const Names[] =
    {
        L"DisplayName", L"DisplayVersion", L"Publisher", L"InstallLocation",
        L"DisplayIcon", L"UninstallString", L"QuietUninstallString"
    };
    WCHAR Values[7][MAX_PATH * 2];
    HKEY Key;
    DWORD Error, Index, Enabled = 1;

    StringCchCopyW(Values[0], ARRAYSIZE(Values[0]), L"ReliefOffice");
    StringCchCopyW(Values[1], ARRAYSIZE(Values[1]), L"1.0");
    StringCchCopyW(Values[2], ARRAYSIZE(Values[2]), L"ReliefWin contributors");
    StringCchCopyW(Values[3], ARRAYSIZE(Values[3]), Directory);
    StringCchPrintfW(Values[4], ARRAYSIZE(Values[4]), L"%ls\\reliefword.exe", Directory);
    StringCchPrintfW(Values[5], ARRAYSIZE(Values[5]),
                     L"\"%ls\\reliefoffice-maintenance.exe\" /uninstall", Directory);
    StringCchPrintfW(Values[6], ARRAYSIZE(Values[6]), L"%ls /quiet", Values[5]);
    Error = RegCreateKeyExW(HKEY_LOCAL_MACHINE, OfficeUninstallKey, 0, NULL, 0,
                            KEY_SET_VALUE, NULL, &Key, NULL);
    if (Error != ERROR_SUCCESS)
        return Error;
    for (Index = 0; Index < ARRAYSIZE(Names); Index++)
    {
        Error = RegSetValueExW(Key, Names[Index], 0, REG_SZ, (const BYTE *)Values[Index],
                               (DWORD)(wcslen(Values[Index]) + 1) * sizeof(WCHAR));
        if (Error != ERROR_SUCCESS)
            break;
    }
    if (Error == ERROR_SUCCESS)
        Error = RegSetValueExW(Key, L"NoModify", 0, REG_DWORD, (const BYTE *)&Enabled, sizeof(Enabled));
    if (Error == ERROR_SUCCESS)
        Error = RegSetValueExW(Key, L"NoRepair", 0, REG_DWORD, (const BYTE *)&Enabled, sizeof(Enabled));
    RegCloseKey(Key);
    return Error;
}

static DWORD
OfficeUninstall(
    _In_ PCWSTR Executable,
    _In_ PCWSTR Directory)
{
    WCHAR Registered[MAX_PATH], Programs[MAX_PATH], Path[MAX_PATH];
    DWORD Error, FirstError = ERROR_SUCCESS;
    ULONG Index;
    HRESULT Result;

    Error = OfficeReadString(HKEY_LOCAL_MACHINE, OfficeUninstallKey, L"InstallLocation",
                             Registered, ARRAYSIZE(Registered));
    if (Error != ERROR_SUCCESS)
        return Error;
    if (_wcsicmp(Registered, Directory))
        return ERROR_INVALID_DATA;

    for (Index = 0; Index + 1 < ARRAYSIZE(OfficeFiles); Index++)
    {
        Error = OfficeDeleteFile(Directory, OfficeFiles[Index]);
        if (Error != ERROR_SUCCESS && FirstError == ERROR_SUCCESS)
            FirstError = Error;
    }
    if (FirstError != ERROR_SUCCESS)
        return FirstError;

    Result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(Result))
        return ERROR_GEN_FAILURE;
    Result = SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, Programs);
    if (FAILED(Result))
    {
        CoUninitialize();
        return ERROR_PATH_NOT_FOUND;
    }
    for (Index = 0; Index < ARRAYSIZE(OfficeApplications); Index++)
    {
        Error = OfficeRemoveShortcut(Directory, Programs, &OfficeApplications[Index]);
        if (Error == ERROR_SUCCESS)
            Error = OfficeRemoveAssociation(Directory, &OfficeApplications[Index]);
        if (Error != ERROR_SUCCESS && FirstError == ERROR_SUCCESS)
            FirstError = Error;
    }
    CoUninitialize();
    if (FirstError != ERROR_SUCCESS)
        return FirstError;

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\ReliefOffice", Programs);
    RemoveDirectoryW(Path);
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\ReliefMath", Directory);
    RemoveDirectoryW(Path);
    Error = RegDeleteTreeW(HKEY_LOCAL_MACHINE, OfficeUninstallKey);
    if (Error != ERROR_SUCCESS && Error != ERROR_FILE_NOT_FOUND)
    {
        OfficeRestoreUninstall(Directory);
        return Error;
    }
    if (!MoveFileExW(Executable, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
    {
        Error = GetLastError();
        FirstError = OfficeRestoreUninstall(Directory);
        return FirstError != ERROR_SUCCESS ? FirstError : Error;
    }
    MoveFileExW(Directory, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return ERROR_SUCCESS;
}

int WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE Previous, PWSTR CommandLine, int Show)
{
    WCHAR Executable[MAX_PATH], Directory[MAX_PATH], Message[256];
    PWSTR Name;
    DWORD Length, Error;
    BOOL Quiet = wcsstr(CommandLine, L"/quiet") != NULL;

    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(Previous);
    UNREFERENCED_PARAMETER(Show);
    if (!wcsstr(CommandLine, L"/uninstall"))
        return ERROR_INVALID_PARAMETER;
    if (!IsUserAnAdmin())
    {
        if (!Quiet)
            MessageBoxW(NULL, L"请以管理员身份运行 ReliefOffice 卸载程序。", L"ReliefOffice", MB_OK | MB_ICONERROR);
        return ERROR_ACCESS_DENIED;
    }
    if (!Quiet && MessageBoxW(NULL, L"卸载 ReliefOffice？你的文档将会保留。", L"ReliefOffice",
                              MB_YESNO | MB_ICONQUESTION) != IDYES)
        return ERROR_CANCELLED;
    Length = GetModuleFileNameW(NULL, Executable, ARRAYSIZE(Executable));
    if (!Length || Length >= ARRAYSIZE(Executable))
        return ERROR_FILENAME_EXCED_RANGE;
    StringCchCopyW(Directory, ARRAYSIZE(Directory), Executable);
    Name = wcsrchr(Directory, L'\\');
    if (!Name || _wcsicmp(Name + 1, L"reliefoffice-maintenance.exe"))
        return ERROR_INVALID_NAME;
    *Name = UNICODE_NULL;
    Error = OfficeUninstall(Executable, Directory);
    if (!Quiet)
    {
        if (Error == ERROR_SUCCESS)
            MessageBoxW(NULL, L"ReliefOffice 已卸载。重启后将移除卸载程序。", L"ReliefOffice", MB_OK);
        else
        {
            StringCchPrintfW(Message, ARRAYSIZE(Message),
                             L"卸载未完成（错误 %lu）。请关闭 ReliefOffice 后重试。卸载入口和你的文档已保留。", Error);
            MessageBoxW(NULL, Message, L"ReliefOffice", MB_OK | MB_ICONERROR);
        }
    }
    return Error;
}
