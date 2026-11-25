/*
 * PROJECT:     ReliefOffice API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify installed registration, shortcuts and application startup
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#define COBJMACROS
#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <strsafe.h>
#include "../maintenance/manifest.h"

static ULONG Checks, Failures;

static VOID
Check(BOOL Passed, PCSTR Name, PCWSTR Item)
{
    CHAR Text[MAX_PATH * 3];

    WideCharToMultiByte(CP_UTF8, 0, Item ? Item : L"", -1, Text, sizeof(Text), NULL, NULL);
    Checks++;
    if (!Passed)
        Failures++;
    printf("%s %s %s\n", Passed ? "PASS" : "FAIL", Name, Text);
    fflush(stdout);
}

static BOOL
ReadString(PCWSTR KeyPath, PCWSTR Name, PWSTR Value, DWORD Count)
{
    HKEY Key;
    DWORD Type, Size = Count * sizeof(WCHAR);
    LONG Error;

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KeyPath, 0, KEY_QUERY_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return FALSE;
    Error = RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)Value, &Size);
    RegCloseKey(Key);
    return Error == ERROR_SUCCESS && Type == REG_SZ && Size >= sizeof(WCHAR) &&
           Size <= Count * sizeof(WCHAR) && !(Size % sizeof(WCHAR)) &&
           Value[Size / sizeof(WCHAR) - 1] == UNICODE_NULL;
}

static VOID
CheckLink(PCWSTR Directory, PCWSTR Programs, const OFFICE_APPLICATION *Application)
{
    WCHAR Path[MAX_PATH], Target[MAX_PATH] = {0}, Expected[MAX_PATH];
    WIN32_FIND_DATAW Data;
    IShellLinkW *Link;
    IPersistFile *File;
    HRESULT Result;

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\ReliefOffice\\%ls.lnk", Programs, Application->Name);
    StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"%ls\\%ls", Directory, Application->Executable);
    Result = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                              &IID_IShellLinkW, (PVOID *)&Link);
    if (SUCCEEDED(Result))
    {
        Result = IShellLinkW_QueryInterface(Link, &IID_IPersistFile, (PVOID *)&File);
        if (SUCCEEDED(Result))
        {
            Result = IPersistFile_Load(File, Path, STGM_READ);
            if (SUCCEEDED(Result))
                Result = IShellLinkW_GetPath(Link, Target, ARRAYSIZE(Target), &Data, SLGP_RAWPATH);
            IPersistFile_Release(File);
        }
        IShellLinkW_Release(Link);
    }
    Check(Result == S_OK && !_wcsicmp(Target, Expected), "Start-menu-target", Application->Name);
}

typedef struct _OFFICE_WINDOW
{
    DWORD ProcessId;
    HWND Window;
    PCWSTR Title;
} OFFICE_WINDOW;

static BOOL CALLBACK
FindWindowForProcess(HWND Window, LPARAM Context)
{
    OFFICE_WINDOW *Search = (OFFICE_WINDOW *)Context;
    WCHAR Title[256];
    DWORD ProcessId;

    GetWindowThreadProcessId(Window, &ProcessId);
    if (ProcessId != Search->ProcessId || !IsWindowVisible(Window) || GetWindow(Window, GW_OWNER))
        return TRUE;
    GetWindowTextW(Window, Title, ARRAYSIZE(Title));
    if (!wcsstr(Title, Search->Title))
        return TRUE;
    Search->Window = Window;
    printf("WINDOW %lu ", ProcessId);
    {
        CHAR Text[768];
        WideCharToMultiByte(CP_UTF8, 0, Title, -1, Text, sizeof(Text), NULL, NULL);
        printf("%s\n", Text);
    }
    return FALSE;
}

static VOID
CheckApplication(PCWSTR Directory, const OFFICE_APPLICATION *Application)
{
    WCHAR Path[MAX_PATH], Command[MAX_PATH * 2];
    STARTUPINFOW Startup;
    PROCESS_INFORMATION Process;
    OFFICE_WINDOW Search;
    DWORD Wait, Started, ExitCode = STILL_ACTIVE;
    BOOL Created;

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, Application->Executable);
    StringCchPrintfW(Command, ARRAYSIZE(Command), L"\"%ls\"", Path);
    ZeroMemory(&Startup, sizeof(Startup));
    Startup.cb = sizeof(Startup);
    Created = CreateProcessW(Path, Command, NULL, NULL, FALSE, 0, NULL, Directory, &Startup, &Process);
    Check(Created, "CreateProcess", Application->Name);
    if (!Created)
    {
        printf("CREATE_ERROR %lu\n", GetLastError());
        return;
    }
    Wait = WaitForInputIdle(Process.hProcess, 5000);
    Check(Wait == 0, "Input-idle", Application->Name);
    Search.ProcessId = Process.dwProcessId;
    Search.Window = NULL;
    Search.Title = !_wcsicmp(Application->Name, L"ReliefMathPP") ? L"ReliefMath++" : Application->Name;
    Started = GetTickCount();
    do
    {
        EnumWindows(FindWindowForProcess, (LPARAM)&Search);
        if (Search.Window || WaitForSingleObject(Process.hProcess, 0) == WAIT_OBJECT_0)
            break;
        Sleep(20);
    } while (GetTickCount() - Started < 2000);
    Check(Search.Window != NULL, "Visible-titled-window", Application->Name);
    if (Search.Window)
        PostMessageW(Search.Window, WM_CLOSE, 0, 0);
    Wait = WaitForSingleObject(Process.hProcess, 3000);
    if (Wait == WAIT_OBJECT_0)
        GetExitCodeProcess(Process.hProcess, &ExitCode);
    Check(Wait == WAIT_OBJECT_0 && ExitCode == 0, "Clean-window-close", Application->Name);
    if (Wait != WAIT_OBJECT_0)
        printf("PROCESS_LEFT_FOR_DIAGNOSIS %lu\n", Process.dwProcessId);
    CloseHandle(Process.hThread);
    CloseHandle(Process.hProcess);
}

int main(void)
{
    static const PCWSTR ForeignExtensions[] =
    {
        L".doc", L".docx", L".xls", L".xlsx", L".ppt", L".pptx", L".pdf", L".txt",
        L".csv", L".html"
    };
    static const int Desktops[] = {CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY};
    WCHAR Directory[MAX_PATH], Expected[MAX_PATH], Programs[MAX_PATH], Path[MAX_PATH];
    WCHAR Value[MAX_PATH * 2], KeyPath[MAX_PATH], Desktop[MAX_PATH];
    DWORD Index, Folder;
    HRESULT Result;
    BOOL Found;

    Found = ReadString(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ReliefOffice",
                        L"InstallLocation", Directory, ARRAYSIZE(Directory));
    Check(Found, "InstallLocation", L"ReliefOffice");
    if (!Found)
        return 1;
    Value[0] = UNICODE_NULL;
    Found = ReadString(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir",
                        Value, ARRAYSIZE(Value));
    StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"%ls\\ReliefOffice", Value);
    Check(Found && !_wcsicmp(Directory, Expected), "ProgramFiles-directory", Directory);
    Result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    Check(SUCCEEDED(Result), "COM", L"");
    if (FAILED(Result))
        return 1;
    Result = SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, Programs);
    Check(SUCCEEDED(Result), "Common-Programs", L"");
    for (Index = 0; Index < ARRAYSIZE(OfficeFiles); Index++)
    {
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, OfficeFiles[Index]);
        Check(GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES, "Installed-manifest-file", OfficeFiles[Index]);
    }
    for (Index = 0; Index < ARRAYSIZE(OfficeApplications); Index++)
    {
        const OFFICE_APPLICATION *Application = &OfficeApplications[Index];
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, Application->Executable);
        Check(GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES, "Installed-executable", Application->Name);
        StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath), L"SOFTWARE\\Classes\\%ls", Application->Extension);
        Found = ReadString(KeyPath, NULL, Value, ARRAYSIZE(Value));
        Check(Found && !_wcsicmp(Value, Application->ProgId), "Native-extension", Application->Extension);
        StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath), L"SOFTWARE\\Classes\\%ls\\shell\\open\\command", Application->ProgId);
        Found = ReadString(KeyPath, NULL, Value, ARRAYSIZE(Value));
        StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"\"%ls\\%ls\" \"%%1\"", Directory, Application->Executable);
        Check(Found && !_wcsicmp(Value, Expected), "Native-open-command", Application->Name);
        if (SUCCEEDED(Result))
            CheckLink(Directory, Programs, Application);
        for (Folder = 0; Folder < ARRAYSIZE(Desktops); Folder++)
        {
            if (SUCCEEDED(SHGetFolderPathW(NULL, Desktops[Folder], NULL, SHGFP_TYPE_CURRENT, Desktop)))
            {
                StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls.lnk", Desktop, Application->Name);
                Check(GetFileAttributesW(Path) == INVALID_FILE_ATTRIBUTES, "No-desktop-shortcut", Application->Name);
            }
        }
    }
    for (Index = 0; Index < ARRAYSIZE(ForeignExtensions); Index++)
    {
        StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath), L"SOFTWARE\\Classes\\%ls", ForeignExtensions[Index]);
        Found = ReadString(KeyPath, NULL, Value, ARRAYSIZE(Value));
        Check(!Found || !wcsstr(Value, L"ReliefOffice"), "Foreign-extension-unclaimed", ForeignExtensions[Index]);
    }
    for (Index = 0; Index < ARRAYSIZE(OfficeApplications); Index++)
        CheckApplication(Directory, &OfficeApplications[Index]);
    CoUninitialize();
    printf("ReliefOffice install smoke: %lu checks, %lu failures\n", Checks, Failures);
    return Failures ? 1 : 0;
}
