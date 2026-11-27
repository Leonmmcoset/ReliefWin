/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Volume filter notification forwarding and mounted-file regression tests
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <windows.h>
#include <winioctl.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <reliefwin/drivers/volumecrypt.h>
#include <stdio.h>

static unsigned Checks, Failures;
#define CHECK(C) do { ++Checks; if (!(C)) { ++Failures; \
    printf("FAIL %u: %s (%lu)\n", (unsigned)__LINE__, #C, GetLastError()); } } while (0)

extern int CryptTestData(LPCWSTR Path);

int main(int Argc, char **Argv)
{
    WCHAR Path[MAX_PATH], Volume[] = L"\\\\.\\C:";
    HANDLE File, Control, Event;
    RW_CRYPT_STATUS Status;
    OVERLAPPED Overlapped = {0};
    BYTE Buffer[4096];
    DWORD Bytes, Error;
    BOOL Result;
    if (Argc == 3 && !strcmp(Argv[1], "--data"))
    {
        MultiByteToWideChar(CP_ACP, 0, Argv[2], -1, Path, MAX_PATH);
        return CryptTestData(Path);
    }
    CHECK(GetWindowsDirectoryW(Path, MAX_PATH) > 0);
    Volume[4] = Path[0];
    wcscat(Path, L"\\system32\\kernel32.dll");
    File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, 0, NULL);
    CHECK(File != INVALID_HANDLE_VALUE);
    if (File != INVALID_HANDLE_VALUE)
    {
        CHECK(ReadFile(File, Buffer, sizeof(Buffer), &Bytes, NULL) && Bytes == sizeof(Buffer));
        CHECK(Buffer[0] == 'M' && Buffer[1] == 'Z');
        CloseHandle(File);
    }
    Control = CreateFileW(L"\\\\.\\ReliefWinEncryption", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    CHECK(Control != INVALID_HANDLE_VALUE);
    if (Control != INVALID_HANDLE_VALUE)
    {
        CHECK(DeviceIoControl(Control, RW_CRYPT_IOCTL_STATUS, NULL, 0, &Status, sizeof(Status), &Bytes, NULL));
        CHECK(Bytes == sizeof(Status) && Status.Version == RW_CRYPT_VERSION &&
              Status.Count <= RW_CRYPT_MAX_VOLUMES);
        CloseHandle(Control);
    }
    File = CreateFileW(Volume, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
    CHECK(File != INVALID_HANDLE_VALUE);
    if (File != INVALID_HANDLE_VALUE)
    {
        Event = CreateEventW(NULL, TRUE, FALSE, NULL);
        CHECK(Event != NULL);
        Overlapped.hEvent = Event;
        Result = DeviceIoControl(File, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                                  Buffer, sizeof(Buffer), &Bytes, &Overlapped);
        Error = Result ? ERROR_SUCCESS : GetLastError();
        CHECK(Result || Error == ERROR_IO_PENDING);
        if (Error == ERROR_IO_PENDING)
        {
            CHECK(WaitForSingleObject(Event, 5000) == WAIT_OBJECT_0);
            CHECK(GetOverlappedResult(File, &Overlapped, &Bytes, FALSE));
        }
        CHECK(Bytes >= sizeof(STORAGE_DEVICE_NUMBER));
        CloseHandle(Event);
        CloseHandle(File);
    }
    printf("%u checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
