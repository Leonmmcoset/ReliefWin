/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verify file and directory reopening and documented flag handling
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "precomp.h"

static VOID
CheckIdentity(HANDLE Original, HANDLE Reopened, BOOL Directory)
{
    BY_HANDLE_FILE_INFORMATION Before, After;
    BOOL HaveBefore, HaveAfter;

    HaveBefore = GetFileInformationByHandle(Original, &Before);
    ok(HaveBefore, "Original metadata failed: %lu\n", GetLastError());
    HaveAfter = GetFileInformationByHandle(Reopened, &After);
    ok(HaveAfter, "Reopened metadata failed: %lu\n", GetLastError());
    if (!HaveBefore || !HaveAfter)
        return;
    ok(!!(After.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == Directory,
       "Unexpected object attributes: %#lx\n", After.dwFileAttributes);
    ok_hex(After.dwVolumeSerialNumber, Before.dwVolumeSerialNumber);
    ok_hex(After.nFileIndexHigh, Before.nFileIndexHigh);
    ok_hex(After.nFileIndexLow, Before.nFileIndexLow);
    ok_hex(After.nFileSizeHigh, Before.nFileSizeHigh);
    ok_hex(After.nFileSizeLow, Before.nFileSizeLow);
}

static VOID
CheckFile(HANDLE Original, DWORD Flags)
{
    static const CHAR Expected[] = "reopen";
    CHAR Data[sizeof(Expected)] = { 0 };
    HANDLE Reopened, Event = NULL;
    OVERLAPPED Overlapped = { 0 };
    DWORD Count = 0, Error;
    BOOL Result;

    Reopened = ReOpenFile(Original, GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          Flags);
    ok(Reopened != INVALID_HANDLE_VALUE, "ReOpenFile flags %#lx failed: %lu\n", Flags, GetLastError());
    if (Reopened == INVALID_HANDLE_VALUE)
        return;
    CheckIdentity(Original, Reopened, FALSE);
    if (Flags & FILE_FLAG_OVERLAPPED)
    {
        Event = CreateEventW(NULL, TRUE, FALSE, NULL);
        ok(Event != NULL, "CreateEventW failed: %lu\n", GetLastError());
        if (!Event)
        {
            CloseHandle(Reopened);
            return;
        }
        Overlapped.hEvent = Event;
        Result = ReadFile(Reopened, Data, sizeof(Expected), NULL, &Overlapped);
        Error = GetLastError();
        ok(Result || Error == ERROR_IO_PENDING, "Overlapped ReadFile failed: %lu\n", Error);
        if (Result || Error == ERROR_IO_PENDING)
        {
            Result = GetOverlappedResult(Reopened, &Overlapped, &Count, TRUE);
            ok(Result, "GetOverlappedResult failed: %lu\n", GetLastError());
        }
    }
    else
    {
        ok_int(SetFilePointer(Reopened, 0, NULL, FILE_BEGIN), 0);
        Result = ReadFile(Reopened, Data, sizeof(Expected), &Count, NULL);
        ok(Result, "ReadFile failed: %lu\n", GetLastError());
    }
    if (Result)
    {
        ok_int(Count, sizeof(Expected));
        ok(!memcmp(Data, Expected, sizeof(Expected)), "Reopened file contents differ\n");
    }
    if (Event)
        CloseHandle(Event);
    ok(CloseHandle(Reopened), "CloseHandle reopened file failed: %lu\n", GetLastError());
}

START_TEST(ReOpenFile)
{
    static const CHAR Contents[] = "reopen";
    WCHAR Temporary[MAX_PATH], Directory[MAX_PATH], Path[MAX_PATH];
    HANDLE File = INVALID_HANDLE_VALUE, Folder = INVALID_HANDLE_VALUE, Reopened;
    DWORD Count = 0, Error;
    BOOL Result;

    if (!GetTempPathW(ARRAYSIZE(Temporary), Temporary) ||
        !GetTempFileNameW(Temporary, L"rop", 0, Directory))
    {
        skip("Temporary path unavailable: %lu\n", GetLastError());
        return;
    }
    Result = DeleteFileW(Directory);
    ok(Result, "Temporary file removal failed: %lu\n", GetLastError());
    if (!Result)
        return;
    Result = CreateDirectoryW(Directory, NULL);
    ok(Result, "CreateDirectoryW failed: %lu\n", GetLastError());
    if (!Result)
        return;
    if (FAILED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"%ls\\data.tmp", Directory)))
    {
        ok(0, "Temporary filename too long\n");
        goto Cleanup;
    }
    File = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    ok(File != INVALID_HANDLE_VALUE, "CreateFileW failed: %lu\n", GetLastError());
    if (File == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Result = WriteFile(File, Contents, sizeof(Contents), &Count, NULL);
    ok(Result, "WriteFile failed: %lu\n", GetLastError());
    ok_int(Count, sizeof(Contents));
    if (!Result || Count != sizeof(Contents))
        goto Cleanup;
    CheckFile(File, 0);
    CheckFile(File, FILE_FLAG_BACKUP_SEMANTICS);
    CheckFile(File, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    CheckFile(File, FILE_FLAG_OVERLAPPED);
    SetLastError(0xdeadbeef);
    Reopened = ReOpenFile(File, GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          FILE_ATTRIBUTE_NORMAL);
    Error = GetLastError();
    ok(Reopened == INVALID_HANDLE_VALUE, "File attribute flags were accepted\n");
    ok_int(Error, ERROR_INVALID_PARAMETER);
    if (Reopened != INVALID_HANDLE_VALUE)
        CloseHandle(Reopened);
    Folder = CreateFileW(Directory, FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES | READ_CONTROL,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    ok(Folder != INVALID_HANDLE_VALUE, "Directory CreateFileW failed: %lu\n", GetLastError());
    if (Folder == INVALID_HANDLE_VALUE)
        goto Cleanup;
    Reopened = ReOpenFile(Folder, FILE_READ_ATTRIBUTES,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          FILE_FLAG_BACKUP_SEMANTICS);
    ok(Reopened != INVALID_HANDLE_VALUE, "Directory ReOpenFile failed: %lu\n", GetLastError());
    if (Reopened != INVALID_HANDLE_VALUE)
    {
        CheckIdentity(Folder, Reopened, TRUE);
        ok(CloseHandle(Reopened), "CloseHandle reopened directory failed: %lu\n", GetLastError());
    }
    Reopened = ReOpenFile(Folder, WRITE_DAC | FILE_READ_ATTRIBUTES,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    ok(Reopened != INVALID_HANDLE_VALUE, "Directory WRITE_DAC reopen failed: %lu\n", GetLastError());
    if (Reopened != INVALID_HANDLE_VALUE)
    {
        CheckIdentity(Folder, Reopened, TRUE);
        ok(CloseHandle(Reopened), "CloseHandle WRITE_DAC directory failed: %lu\n", GetLastError());
    }
    Reopened = ReOpenFile(Folder, FILE_READ_ATTRIBUTES,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0);
    ok(Reopened == INVALID_HANDLE_VALUE, "Directory reopened without backup semantics\n");
    if (Reopened != INVALID_HANDLE_VALUE)
        CloseHandle(Reopened);

Cleanup:
    if (Folder != INVALID_HANDLE_VALUE)
        ok(CloseHandle(Folder), "CloseHandle directory failed: %lu\n", GetLastError());
    if (File != INVALID_HANDLE_VALUE)
    {
        ok(CloseHandle(File), "CloseHandle file failed: %lu\n", GetLastError());
        ok(DeleteFileW(Path), "DeleteFileW failed: %lu\n", GetLastError());
    }
    ok(RemoveDirectoryW(Directory), "RemoveDirectoryW failed: %lu\n", GetLastError());
}
