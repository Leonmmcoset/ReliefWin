#!/usr/bin/env python3
# PROJECT:     ReliefWin host-native tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise installed-release parsing and unconfigured updates with Windows I/O mocks
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


def function(source, name):
    start = source.index(name + '(')
    begin = source.rfind('static ', 0, start)
    cursor = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[cursor] == '{') - (source[cursor] == '}')
        cursor += 1
    return source[begin:cursor]


PREFIX = r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <string>
using WCHAR = wchar_t;
using CHAR = char;
using ULONG = uint32_t;
using DWORD = uint32_t;
using PCSTR = const char *;
using HANDLE = intptr_t;
#define MAX_PATH 260
#define _countof(a) (sizeof(a) / sizeof((a)[0]))
#define UPDATE_INVENTORY L"\\servicing\\ReliefWin.files"
#define GENERIC_READ 0x80000000
#define FILE_SHARE_READ 1
#define FILE_SHARE_WRITE 2
#define OPEN_EXISTING 3
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
static std::string Payload;
static bool Missing, FailRead, FailWindowsDirectory;
static unsigned Opened, Closed;
static unsigned GetWindowsDirectoryW(WCHAR *path, unsigned count)
{
    if (FailWindowsDirectory) return 0;
    const WCHAR windows[] = L"C:\\Windows";
    assert(count >= _countof(windows));
    std::wcscpy(path, windows);
    return _countof(windows) - 1;
}
static int wcscat_s(WCHAR *path, size_t count, const WCHAR *suffix)
{
    if (std::wcslen(path) + std::wcslen(suffix) + 1 > count) return 1;
    std::wcscat(path, suffix);
    return 0;
}
static HANDLE CreateFileW(const WCHAR *path, DWORD, DWORD, void *, DWORD, DWORD, void *)
{
    assert(std::wstring(path) == L"C:\\Windows\\servicing\\ReliefWin.files");
    if (Missing) return INVALID_HANDLE_VALUE;
    Opened++;
    return 1;
}
static bool ReadFile(HANDLE file, char *text, DWORD size, DWORD *read, void *)
{
    assert(file == 1);
    if (FailRead) return false;
    *read = std::min(size, static_cast<DWORD>(Payload.size()));
    std::memcpy(text, Payload.data(), *read);
    return true;
}
static void CloseHandle(HANDLE file)
{
    assert(file == 1);
    Closed++;
}
'''

MAIN = r'''
static void check(const char *name, const std::string &payload,
                  const std::wstring &expectedVersion, ULONG expectedBuild)
{
    std::wstring version = L"stale version";
    ULONG build = 99999;
    Payload = payload;
    Opened = Closed = 0;
    ReadInstalledRelease(version, build);
    if (version != expectedVersion || build != expectedBuild)
    {
        std::wcerr << L"FAIL " << name << L": version=[" << version
                   << L"] build=" << build << L", expected=[" << expectedVersion
                   << L"] build=" << expectedBuild << L"\n";
        std::exit(1);
    }
    assert(Opened == Closed);
}
int main()
{
    check("LF", "; ReliefWin 1.2.3\n; Build 42\n", L"1.2.3", 42);
    check("CRLF", "; ReliefWin 10.0-preview\r\n; Build 1007\r\n", L"10.0-preview", 1007);
    check("one-digit", "; ReliefWin 1\n; Build 7", L"1", 7);
    check("no-build", "; ReliefWin 2.0", L"2.0", 0);
    check("empty", "", L"", 0);
    check("other-product", "; OtherOS 3.0\n; Build 56", L"", 56);
    check("invalid-build", "; ReliefWin 4.0\n; Build no-number", L"4.0", 0);
    check("bounded-read", "; ReliefWin " + std::string(700, 'v'), std::wstring(499, L'v'), 0);
    Missing = true;
    check("missing-file", "; ReliefWin 1.0\n; Build 99", L"", 0);
    Missing = false;
    FailRead = true;
    check("failed-read", "; ReliefWin 1.0\n; Build 99", L"", 0);
    FailRead = false;
    FailWindowsDirectory = true;
    check("failed-system-directory", "; ReliefWin 1.0\n; Build 99", L"", 0);
    std::cout << "Installed-release parsing: 11 cases passed\n";
}
'''


UPDATER_PREFIX = r'''
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <string>
using BOOL = int;
using WCHAR = wchar_t;
using PWSTR = WCHAR *;
using ULONG = uint32_t;
using DWORD = uint32_t;
using ULONGLONG = uint64_t;
using PSTR = char *;
using HRESULT = int32_t;
#define _Out_
#define MAX_PATH 260
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#define HKEY_LOCAL_MACHINE 1
#define UPDATE_KEY L"SOFTWARE\\ReliefWin\\Update"
#define RRF_RT_REG_SZ 2
#define UPDATE_ID_CHARS 39
#define UPDATE_NOTES_MAX (64 * 1024)
#define REG_QWORD 11
#define REG_DWORD 4
#define REG_SZ 1
#define ERROR_SUCCESS 0
#define ERROR_NOT_SUPPORTED 50
#define ERROR_BUFFER_OVERFLOW 111
#define ERROR_INVALID_DATA 13
#define FAILED(hr) ((hr) < 0)
#define ZeroMemory(p, n) std::memset((p), 0, (n))
#define DPRINT1(...) ((void)0)
static constexpr HRESULT DownloadFailure = static_cast<HRESULT>(0x80004005u);
static int ProcessHeap;
static ULONG LocalBuild;
static DWORD RegistryError;
static std::wstring ConfiguredServer;
static unsigned Freed, NetworkCalls, SettingsWrites;
static bool HeapFree(int, DWORD, void *notes)
{
    assert(notes);
    Freed++;
    std::free(notes);
    return true;
}
static DWORD RegGetValueW(int, const WCHAR *, const WCHAR *name, DWORD, void *, void *value, DWORD *size)
{
    assert(std::wstring(name) == L"Server");
    if (RegistryError) return RegistryError;
    assert(*size >= (ConfiguredServer.size() + 1) * sizeof(WCHAR));
    std::wcscpy(static_cast<WCHAR *>(value), ConfiguredServer.c_str());
    return ERROR_SUCCESS;
}
static int StringCchPrintfW(WCHAR *output, size_t size, const WCHAR *, ...)
{
    assert(size > 4);
    std::wcscpy(output, L"mock");
    return 0;
}
static const WCHAR *GetArchitectureName() { return L"amd64"; }
static DWORD GetTempPathW(DWORD, WCHAR *output) { output[0] = 0; return 1; }
static HRESULT DownloadFile(const WCHAR *, const WCHAR *, void *, void *)
{
    NetworkCalls++;
    return DownloadFailure;
}
static PSTR ReadSmallFile(const WCHAR *) { return nullptr; }
static void DeleteFileW(const WCHAR *) {}
static ULONGLONG JsonNumber(PSTR, const char *) { return 0; }
static void CopyJsonString(PSTR, const char *, WCHAR *, size_t) {}
static PWSTR JsonString(PSTR, const char *, size_t) { return nullptr; }
static BOOL JsonBoolean(PSTR, const char *) { return false; }
static BOOL IsSha256(const WCHAR *) { return false; }
static BOOL IsStageIntact() { return false; }
static BOOL ReadStagedRequest(ULONG *, WCHAR *, WCHAR *) { return false; }
static ULONGLONG CurrentTime() { return 0; }
static void WriteSetting(const WCHAR *, DWORD, const void *, DWORD) { SettingsWrites++; }
'''

UPDATER_MAIN = r'''
static void check(const char *name, DWORD registryError, const WCHAR *server,
                  DWORD expectedError, unsigned expectedNetwork)
{
    UPDATE_INFO info{};
    info.Available = true;
    info.Staged = true;
    info.TagId = info.Build = 42;
    info.Size = 1024;
    info.Notes = static_cast<PWSTR>(std::malloc(sizeof(WCHAR)));
    assert(info.Notes);
    RegistryError = registryError;
    ConfiguredServer = server;
    Freed = NetworkCalls = SettingsWrites = 0;
    DWORD error = CheckForUpdate(&info);
    if (error != expectedError || NetworkCalls != expectedNetwork || SettingsWrites || Freed != 1)
    {
        std::cerr << "FAIL update " << name << ": error=" << error << " network="
                  << NetworkCalls << " settings=" << SettingsWrites << " freed=" << Freed << "\n";
        std::exit(1);
    }
    UPDATE_INFO empty{};
    assert(std::memcmp(&info, &empty, sizeof(info)) == 0);
}
int main()
{
    check("missing-server", 2, L"", ERROR_NOT_SUPPORTED, 0);
    check("invalid-server", ERROR_INVALID_DATA, L"", ERROR_NOT_SUPPORTED, 0);
    check("empty-server", ERROR_SUCCESS, L"", ERROR_NOT_SUPPORTED, 0);
    check("configured-server", ERROR_SUCCESS, L"https://example.invalid/update",
          static_cast<DWORD>(DownloadFailure), 1);
    std::cout << "Update configuration: 4 cases passed\n";
}
'''


def run(cxx, directory, source, prefix=PREFIX, main=MAIN):
    path = directory / 'branding.cpp'
    executable = directory / 'branding-test'
    path.write_text(prefix + source + main)
    subprocess.run([cxx, '-std=c++17', '-Wall', '-Wextra', '-Werror',
                    str(path), '-o', str(executable)], check=True)
    return subprocess.run([str(executable)], text=True, capture_output=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--verify-regression', action='store_true')
    args = parser.parse_args()
    source = function((args.root / 'base/applications/systemsettings/page_misc.cpp').read_text(),
                      'ReadInstalledRelease')
    with tempfile.TemporaryDirectory() as temporary:
        directory = Path(temporary)
        if args.verify_regression:
            old = source.replace('sizeof(ReleasePrefix) - 1', '10')
            result = run(args.cxx, directory, old)
            if result.returncode == 0 or 'FAIL LF: version=[n 1.2.3]' not in result.stderr:
                raise RuntimeError('The old product-prefix bug did not fail for the expected reason: ' + result.stderr)
            print('Old fixed product-prefix length: expected failure confirmed')
            old = source.replace('sizeof(BuildPrefix) - 1', '10')
            result = run(args.cxx, directory, old)
            if result.returncode == 0 or 'build=2' not in result.stderr:
                raise RuntimeError('The old build-prefix bug did not fail for the expected reason: ' + result.stderr)
            print('Old fixed build-prefix length: expected failure confirmed')
        result = run(args.cxx, directory, source)
        print(result.stdout, end='')
        if result.returncode:
            raise RuntimeError(result.stderr)
        updater = (args.root / 'base/setup/reliefwin/updagent.c').read_text()
        structure = re.search(r'typedef struct _UPDATE_INFO\b.*?\} UPDATE_INFO, \*PUPDATE_INFO;',
                              updater, re.DOTALL).group()
        source = structure + '\n' + function(updater, 'CheckForUpdate')
        if args.verify_regression:
            old = source.replace('return ERROR_NOT_SUPPORTED;', "Server[0] = L'x'; Server[1] = 0;")
            result = run(args.cxx, directory, old, UPDATER_PREFIX, UPDATER_MAIN)
            if result.returncode == 0 or 'FAIL update missing-server:' not in result.stderr or 'network=1' not in result.stderr:
                raise RuntimeError('The missing-server guard did not fail for the expected reason: ' + result.stderr)
            print('Missing-server guard removed: expected network-call failure confirmed')
        result = run(args.cxx, directory, source, UPDATER_PREFIX, UPDATER_MAIN)
        print(result.stdout, end='')
        if result.returncode:
            raise RuntimeError(result.stderr)


if __name__ == '__main__':
    main()
