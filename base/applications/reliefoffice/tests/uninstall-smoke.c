/*
 * PROJECT:     ReliefOffice API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify installed ownership and document preservation across removal and restart
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#define COBJMACROS
#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <strsafe.h>
#include "../maintenance/manifest.h"

static const WCHAR UninstallKey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ReliefOffice";
static const WCHAR DocumentName[] = L"用户保留文档.txt";
static const WCHAR StateName[] = L".reliefoffice-uninstall-smoke";
static const CHAR Document[] =
    "ReliefOffice uninstall smoke user document.\r\n"
    "用户文档：卸载和重启后应逐字节保留。\r\n";
static const PCWSTR ForeignExtensions[] =
{
    L".doc", L".docx", L".xls", L".xlsx", L".ppt", L".pptx", L".pdf", L".txt",
    L".csv", L".html"
};

typedef struct _REGISTRY_SNAPSHOT
{
    LONG Error;
    DWORD Type, Size;
    BYTE Data[2048];
} REGISTRY_SNAPSHOT;

typedef struct _UNINSTALL_STATE
{
    DWORD Magic, Version;
    WCHAR Directory[MAX_PATH];
    REGISTRY_SNAPSHOT Native[ARRAYSIZE(OfficeApplications)];
    REGISTRY_SNAPSHOT Foreign[ARRAYSIZE(ForeignExtensions)];
} UNINSTALL_STATE;

static UNINSTALL_STATE State;
static ULONG Checks, Failures;

static VOID
Check(BOOL Passed, PCSTR Name, PCWSTR Item)
{
    CHAR Text[MAX_PATH * 3];

    Text[0] = 0;
    WideCharToMultiByte(CP_UTF8, 0, Item ? Item : L"", -1, Text, sizeof(Text), NULL, NULL);
    Checks++;
    if (!Passed)
        Failures++;
    printf("%s %s %s\n", Passed ? "PASS" : "FAIL", Name, Text);
    fflush(stdout);
}

static BOOL
Missing(LONG Error)
{
    return Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND;
}

static BOOL
ReadString(PCWSTR SubKey, PCWSTR Name, PWSTR Value, DWORD Count)
{
    HKEY Key;
    DWORD Type, Size = Count * sizeof(WCHAR);
    LONG Error;

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, SubKey, 0, KEY_QUERY_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return FALSE;
    Error = RegQueryValueExW(Key, Name, NULL, &Type, (PBYTE)Value, &Size);
    RegCloseKey(Key);
    return Error == ERROR_SUCCESS && Type == REG_SZ && Size >= sizeof(WCHAR) &&
           Size <= Count * sizeof(WCHAR) && !(Size % sizeof(WCHAR)) &&
           Value[Size / sizeof(WCHAR) - 1] == UNICODE_NULL;
}

static BOOL
Snapshot(PCWSTR Extension, REGISTRY_SNAPSHOT *Value)
{
    WCHAR KeyPath[MAX_PATH];
    HKEY Key;

    ZeroMemory(Value, sizeof(*Value));
    if (FAILED(StringCchPrintfW(KeyPath, ARRAYSIZE(KeyPath),
                                L"SOFTWARE\\Classes\\%ls", Extension)))
        return FALSE;
    Value->Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KeyPath, 0, KEY_QUERY_VALUE, &Key);
    if (Value->Error == ERROR_SUCCESS)
    {
        Value->Size = sizeof(Value->Data);
        Value->Error = RegQueryValueExW(Key, NULL, NULL, &Value->Type,
                                       Value->Data, &Value->Size);
        RegCloseKey(Key);
    }
    if (Missing(Value->Error))
    {
        Value->Error = ERROR_FILE_NOT_FOUND;
        Value->Type = Value->Size = 0;
        ZeroMemory(Value->Data, sizeof(Value->Data));
        return TRUE;
    }
    return Value->Error == ERROR_SUCCESS && Value->Size <= sizeof(Value->Data);
}

static BOOL
OwnDefault(const REGISTRY_SNAPSHOT *Value, PCWSTR ProgId)
{
    DWORD Size = (DWORD)(wcslen(ProgId) + 1) * sizeof(WCHAR);

    return Value->Error == ERROR_SUCCESS && Value->Type == REG_SZ &&
           Value->Size == Size && !Value->Data[Size - 2] && !Value->Data[Size - 1] &&
           !_wcsicmp((PCWSTR)Value->Data, ProgId);
}

static BOOL
PathMissing(PCWSTR Path)
{
    if (GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES)
        return FALSE;
    return Missing(GetLastError());
}

static BOOL
KeyMissing(PCWSTR KeyPath)
{
    HKEY Key;
    LONG Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, KeyPath, 0, KEY_READ, &Key);

    if (Error == ERROR_SUCCESS)
        RegCloseKey(Key);
    return Missing(Error);
}

static BOOL
WriteNew(PCWSTR Path, const VOID *Data, DWORD Size)
{
    HANDLE File;
    DWORD Written;
    BOOL Result;

    File = CreateFileW(Path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (File == INVALID_HANDLE_VALUE)
        return FALSE;
    Result = WriteFile(File, Data, Size, &Written, NULL) && Written == Size &&
             FlushFileBuffers(File);
    if (!CloseHandle(File))
        Result = FALSE;
    if (!Result)
        DeleteFileW(Path);
    return Result;
}

static BOOL
ReadExact(PCWSTR Path, VOID *Data, DWORD Size)
{
    HANDLE File;
    LARGE_INTEGER Length;
    DWORD Read;
    BOOL Result;

    File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (File == INVALID_HANDLE_VALUE)
        return FALSE;
    Result = GetFileSizeEx(File, &Length) && Length.QuadPart == Size &&
             ReadFile(File, Data, Size, &Read, NULL) && Read == Size;
    if (!CloseHandle(File))
        Result = FALSE;
    return Result;
}

static VOID
CheckDocument(PCWSTR Directory)
{
    WCHAR Path[MAX_PATH * 2];
    CHAR Data[sizeof(Document)];

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, DocumentName);
    Check(ReadExact(Path, Data, sizeof(Document) - 1) &&
          !memcmp(Data, Document, sizeof(Document) - 1),
          "User-document-bytes-preserved", DocumentName);
}

static VOID
CheckLinkTarget(PCWSTR Directory, PCWSTR Programs, const OFFICE_APPLICATION *Application)
{
    WCHAR Path[MAX_PATH * 2], Target[MAX_PATH * 2] = {0}, Expected[MAX_PATH * 2];
    WIN32_FIND_DATAW Data;
    IShellLinkW *Link;
    IPersistFile *File;
    HRESULT Result;

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\ReliefOffice\\%ls.lnk",
                     Programs, Application->Name);
    StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"%ls\\%ls",
                     Directory, Application->Executable);
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
    Check(Result == S_OK && !_wcsicmp(Target, Expected),
          "Owned-start-menu-target", Application->Name);
}

static BOOL
DeleteQueued(PCWSTR Directory)
{
    WCHAR Expected[MAX_PATH * 2], Data[32768];
    HKEY Key;
    DWORD Type, Size = sizeof(Data), Count, Index = 0, Start, Destination;
    LONG Error;

    if (FAILED(StringCchPrintfW(Expected, ARRAYSIZE(Expected),
                                L"\\??\\%ls\\reliefoffice-maintenance.exe", Directory)))
        return FALSE;
    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Control\\Session Manager",
                          0, KEY_QUERY_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return FALSE;
    Error = RegQueryValueExW(Key, L"PendingFileRenameOperations", NULL,
                             &Type, (PBYTE)Data, &Size);
    RegCloseKey(Key);
    if (Error != ERROR_SUCCESS || Type != REG_MULTI_SZ ||
        Size < 2 * sizeof(WCHAR) || Size > sizeof(Data) || Size % sizeof(WCHAR))
        return FALSE;
    Count = Size / sizeof(WCHAR);
    while (Index < Count && Data[Index])
    {
        Start = Index;
        while (Index < Count && Data[Index])
            Index++;
        if (Index == Count || ++Index == Count)
            return FALSE;
        Destination = Index;
        while (Index < Count && Data[Index])
            Index++;
        if (Index == Count)
            return FALSE;
        if (!Data[Destination] && !_wcsicmp(Data + Start, Expected))
            return TRUE;
        Index++;
    }
    return FALSE;
}

static VOID
Prepare(PCWSTR Directory, PCWSTR Programs)
{
    WCHAR Path[MAX_PATH * 2], StatePath[MAX_PATH * 2];
    WCHAR Value[MAX_PATH * 2], Expected[MAX_PATH * 2];
    ULONG Index;
    HRESULT Result;

    Check(ReadString(UninstallKey, L"InstallLocation", Value, ARRAYSIZE(Value)) &&
          !_wcsicmp(Value, Directory), "Installed-directory", Directory);
    for (Index = 0; Index < ARRAYSIZE(OfficeFiles); Index++)
    {
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, OfficeFiles[Index]);
        Check(GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES,
              "Installed-manifest-file", OfficeFiles[Index]);
    }
    Result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    Check(SUCCEEDED(Result), "COM", L"");
    if (FAILED(Result))
        return;
    ZeroMemory(&State, sizeof(State));
    State.Magic = 0x55464F52;
    State.Version = 1;
    StringCchCopyW(State.Directory, ARRAYSIZE(State.Directory), Directory);
    for (Index = 0; Index < ARRAYSIZE(OfficeApplications); Index++)
    {
        const OFFICE_APPLICATION *Application = &OfficeApplications[Index];

        Check(Snapshot(Application->Extension, &State.Native[Index]),
              "Snapshot-native-default", Application->Extension);
        StringCchPrintfW(Path, ARRAYSIZE(Path),
                         L"SOFTWARE\\Classes\\%ls\\shell\\open\\command", Application->ProgId);
        StringCchPrintfW(Expected, ARRAYSIZE(Expected), L"\"%ls\\%ls\" \"%%1\"",
                         Directory, Application->Executable);
        Check(ReadString(Path, NULL, Value, ARRAYSIZE(Value)) && !_wcsicmp(Value, Expected),
              "Owned-open-command", Application->Name);
        CheckLinkTarget(Directory, Programs, Application);
    }
    CoUninitialize();
    for (Index = 0; Index < ARRAYSIZE(ForeignExtensions); Index++)
        Check(Snapshot(ForeignExtensions[Index], &State.Foreign[Index]),
              "Snapshot-foreign-default", ForeignExtensions[Index]);
    if (Failures)
        return;
    StringCchPrintfW(StatePath, ARRAYSIZE(StatePath), L"%ls\\%ls", Directory, StateName);
    Check(WriteNew(StatePath, &State, sizeof(State)), "Prepare-state-created-without-overwrite", StateName);
    if (Failures)
        return;
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, DocumentName);
    if (!WriteNew(Path, Document, sizeof(Document) - 1))
    {
        Check(FALSE, "Prepare-document-created-without-overwrite", DocumentName);
        Check(DeleteFileW(StatePath), "Prepare-state-rollback", StateName);
        return;
    }
    Check(TRUE, "Prepare-document-created-without-overwrite", DocumentName);
    CheckDocument(Directory);
}

static VOID
Verify(PCWSTR Directory, PCWSTR Programs, BOOL AfterReboot)
{
    WCHAR Path[MAX_PATH * 2];
    REGISTRY_SNAPSHOT Current;
    ULONG Index;
    BOOL Loaded;

    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, StateName);
    Loaded = ReadExact(Path, &State, sizeof(State)) && State.Magic == 0x55464F52 &&
             State.Version == 1 && State.Directory[ARRAYSIZE(State.Directory) - 1] == 0 &&
             !_wcsicmp(State.Directory, Directory);
    Check(Loaded, "Prepared-state-preserved", StateName);
    if (!Loaded)
        return;
    CheckDocument(Directory);
    Check(KeyMissing(UninstallKey), "Uninstall-registration-removed", L"ReliefOffice");
    for (Index = 0; Index + 1 < ARRAYSIZE(OfficeFiles); Index++)
    {
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory, OfficeFiles[Index]);
        Check(PathMissing(Path), "Manifest-file-removed", OfficeFiles[Index]);
    }
    StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\%ls", Directory,
                     OfficeFiles[ARRAYSIZE(OfficeFiles) - 1]);
    Check(AfterReboot ? PathMissing(Path) : GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES,
          AfterReboot ? "Maintenance-removed-after-reboot" : "Maintenance-present-before-reboot",
          OfficeFiles[ARRAYSIZE(OfficeFiles) - 1]);
    if (!AfterReboot)
        Check(DeleteQueued(Directory), "Maintenance-delete-queued", L"ReliefOffice");
    for (Index = 0; Index < ARRAYSIZE(OfficeApplications); Index++)
    {
        const OFFICE_APPLICATION *Application = &OfficeApplications[Index];
        BOOL Read = Snapshot(Application->Extension, &Current);

        Check(Read && (OwnDefault(&State.Native[Index], Application->ProgId) ?
                        Missing(Current.Error) :
                        !memcmp(&Current, &State.Native[Index], sizeof(Current))),
              "Native-default-released-or-foreign-preserved", Application->Extension);
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"SOFTWARE\\Classes\\%ls", Application->ProgId);
        Check(KeyMissing(Path), "Owned-ProgID-removed", Application->ProgId);
        StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\ReliefOffice\\%ls.lnk",
                         Programs, Application->Name);
        Check(PathMissing(Path), "Start-menu-link-removed", Application->Name);
    }
    for (Index = 0; Index < ARRAYSIZE(ForeignExtensions); Index++)
        Check(Snapshot(ForeignExtensions[Index], &Current) &&
              !memcmp(&Current, &State.Foreign[Index], sizeof(Current)),
              "Foreign-default-unchanged", ForeignExtensions[Index]);
}

int main(int Argc, char **Argv)
{
    WCHAR Directory[MAX_PATH], Programs[MAX_PATH];
    BOOL Preparing = Argc == 2 && !strcmp(Argv[1], "--prepare");
    BOOL AfterReboot = Argc == 2 && !strcmp(Argv[1], "--after-reboot");
    BOOL Found;

    if (Argc != 2 || (!Preparing && !AfterReboot && strcmp(Argv[1], "--verify")))
    {
        printf("Usage: reliefoffice_uninstall_smoke --prepare|--verify|--after-reboot\n");
        return 2;
    }
    Found = ReadString(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir",
                        Directory, ARRAYSIZE(Directory)) &&
            SUCCEEDED(StringCchCatW(Directory, ARRAYSIZE(Directory), L"\\ReliefOffice"));
    Check(Found, "ProgramFiles-directory", L"ReliefOffice");
    if (!Found)
        return 1;
    Found = SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL,
                                      SHGFP_TYPE_CURRENT, Programs));
    Check(Found, "Common-Programs", L"");
    if (!Found)
        return 1;
    if (Preparing)
        Prepare(Directory, Programs);
    else
        Verify(Directory, Programs, AfterReboot);
    printf("ReliefOffice uninstall smoke %s: %lu checks, %lu failures\n",
           Argv[1], Checks, Failures);
    return Failures ? 1 : 0;
}
