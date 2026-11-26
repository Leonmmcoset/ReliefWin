#!/usr/bin/env python3
# PROJECT:     ReliefWin setup tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise actual keyboard registry cleanup with deterministic NT call failures
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = 'base/setup/lib/mui.c'
TOKEN = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*[\s\S]*?\*/')


def function(source, name, declaration):
    clean = TOKEN.sub(lambda match: ' ' * len(match[0]), source)
    match = re.search(r'^' + re.escape(name) + r'\(', clean, re.M)
    if not match:
        raise ValueError(f'Missing function {name}')
    begin = clean.index('{', match.start())
    cursor, depth = begin + 1, 1
    while depth:
        depth += (clean[cursor] == '{') - (clean[cursor] == '}')
        cursor += 1
    return declaration + ' ' + source[match.start():cursor] + '\n'


def constants():
    names = {'STATUS_SUCCESS', 'STATUS_UNSUCCESSFUL', 'KEY_SET_VALUE', 'KEY_CREATE_SUB_KEY',
             'REG_OPTION_NON_VOLATILE', 'REG_SZ', 'OBJ_CASE_INSENSITIVE',
             'REG_CREATED_NEW_KEY', 'REG_OPENED_EXISTING_KEY'}
    definitions = {}
    for name in ('sdk/include/psdk/ntstatus.h', 'sdk/include/xdk/cmtypes.h',
                 'sdk/include/xdk/ntdef.template.h'):
        for line in (ROOT / name).read_text().splitlines():
            match = re.match(r'#define\s+(\w+)\s+(.+)', line)
            if match and match[1] in names:
                definitions[match[1]] = '#define ' + match[1] + ' ' + match[2]
    if definitions.keys() != names:
        raise ValueError('Required NT test constants are missing')
    return '\n'.join(definitions[name] for name in sorted(names)) + '\n'


PREFIX = r'''
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cwchar>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using BOOLEAN = bool;
using NTSTATUS = int32_t;
using ULONG = uint32_t;
using USHORT = uint16_t;
using KLID = uint32_t;
using WCHAR = wchar_t;
using PCWSTR = const WCHAR *;
using PVOID = void *;
using HANDLE = void *;
struct MUI_LAYOUTS { USHORT LangID; KLID LayoutID; };
struct UNICODE_STRING { USHORT Length, MaximumLength; WCHAR *Buffer; };
struct OBJECT_ATTRIBUTES { UNICODE_STRING *ObjectName; HANDLE RootDirectory; ULONG Attributes; };
#define IN
#define _In_
#define TRUE true
#define FALSE false
#define NT_SUCCESS(status) (NTSTATUS(status) >= 0)
#define _countof(array) (sizeof(array) / sizeof((array)[0]))
#define DPRINT1(...) ((void)0)
#define InitializeObjectAttributes(output,name,attributes,root,security) \
    (*(output) = OBJECT_ATTRIBUTES{name,root,attributes})
static const HANDLE RootHandle = reinterpret_cast<HANDLE>(uintptr_t(1));
static constexpr unsigned HKEY_USERS = 3;
static unsigned Checks, Failures, Cases, InvalidCloses, UnexpectedCalls;
static unsigned SubstituteFailureInvalidCloses;
static unsigned Fault, CallIndex;
static bool Failed, ExistingKeys;
static std::string Case;
#define EXPECT(condition) do { ++Checks; if (!(condition)) { \
    if (Failures < 12) std::cerr << Case << ":" << __LINE__ << ": " << #condition << "\n"; \
    ++Failures; } } while (0)
struct Operation {
    char Kind;
    std::wstring Key, Name, Value;
    bool operator==(const Operation &other) const {
        return Kind == other.Kind && Key == other.Key && Name == other.Name && Value == other.Value;
    }
};
struct Owned { std::wstring Key; bool Live; unsigned Closes; };
static std::vector<Operation> Operations, Writes;
static std::map<HANDLE, Owned> Handles;
static std::wstring text(const UNICODE_STRING *value)
{
    EXPECT(value && value->Buffer && value->Length % sizeof(WCHAR) == 0);
    return std::wstring(value->Buffer, value->Length / sizeof(WCHAR));
}
static bool operation(const Operation &value)
{
    if (Failed) {
        ++UnexpectedCalls;
        EXPECT(false);
    }
    Operations.push_back(value);
    ++CallIndex;
    if (Fault && Fault == CallIndex) {
        Failed = true;
        return false;
    }
    return true;
}
static HANDLE GetRootKeyByPredefKey(unsigned key, PVOID reserved)
{
    EXPECT(key == HKEY_USERS && !reserved);
    return RootHandle;
}
static void RtlInitUnicodeString(UNICODE_STRING *output, PCWSTR value)
{
    output->Length = USHORT(std::wcslen(value) * sizeof(WCHAR));
    output->MaximumLength = USHORT(output->Length + sizeof(WCHAR));
    output->Buffer = const_cast<WCHAR *>(value);
}
static NTSTATUS RtlAppendUnicodeToString(UNICODE_STRING *output, PCWSTR suffix)
{
    if (!operation({'A', text(output), L"", suffix}))
        return STATUS_UNSUCCESSFUL;
    size_t count = output->Length / sizeof(WCHAR);
    size_t append = std::wcslen(suffix);
    EXPECT((count + append + 1) * sizeof(WCHAR) <= output->MaximumLength);
    std::wmemcpy(output->Buffer + count, suffix, append + 1);
    output->Length = USHORT((count + append) * sizeof(WCHAR));
    return STATUS_SUCCESS;
}
static NTSTATUS RtlStringCchPrintfW(WCHAR *output, size_t count, PCWSTR format, ...)
{
    va_list args;
    va_start(args, format);
    ULONG value = va_arg(args, ULONG);
    va_end(args);
    EXPECT(std::wcscmp(format, L"%u") == 0 || std::wcscmp(format, L"%08lx") == 0);
    int written = std::swprintf(output, count, std::wcscmp(format, L"%u") == 0 ? L"%u" : L"%08x", value);
    EXPECT(written >= 0 && size_t(written) < count);
    return written >= 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}
static NTSTATUS NtCreateKey(HANDLE *output, ULONG access, OBJECT_ATTRIBUTES *attributes,
                            ULONG title, UNICODE_STRING *keyClass, ULONG options, ULONG *disposition)
{
    EXPECT(attributes->RootDirectory == RootHandle && attributes->Attributes == OBJ_CASE_INSENSITIVE);
    EXPECT(!title && !keyClass && options == REG_OPTION_NON_VOLATILE);
    EXPECT(access == KEY_SET_VALUE || access == KEY_CREATE_SUB_KEY);
    std::wstring name = text(attributes->ObjectName);
    if (!operation({'C', name, L"", L""}))
        return STATUS_UNSUCCESSFUL;
    HANDLE handle = reinterpret_cast<HANDLE>(uintptr_t(100 + Handles.size()));
    EXPECT(!Handles.count(handle));
    Handles.emplace(handle, Owned{name, true, 0});
    *output = handle;
    *disposition = ExistingKeys ? REG_OPENED_EXISTING_KEY : REG_CREATED_NEW_KEY;
    return STATUS_SUCCESS;
}
static NTSTATUS NtSetValueKey(HANDLE handle, UNICODE_STRING *name, ULONG title, ULONG type,
                              PVOID data, ULONG size)
{
    auto owned = Handles.find(handle);
    bool valid = owned != Handles.end() && owned->second.Live;
    EXPECT(valid);
    EXPECT(!title && type == REG_SZ && data);
    PCWSTR value = static_cast<PCWSTR>(data);
    EXPECT(size == (std::wcslen(value) + 1) * sizeof(WCHAR));
    Operation write = {'S', valid ? owned->second.Key : L"invalid", text(name), value};
    if (!operation(write))
        return STATUS_UNSUCCESSFUL;
    Writes.push_back(write);
    return STATUS_SUCCESS;
}
static NTSTATUS NtClose(HANDLE handle)
{
    auto owned = Handles.find(handle);
    bool valid = owned != Handles.end() && owned->second.Live;
    if (!valid)
        ++InvalidCloses;
    EXPECT(valid);
    if (valid) {
        owned->second.Live = false;
        ++owned->second.Closes;
    }
    return valid ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}
'''


TESTS = r'''
static const std::wstring Base = L".DEFAULT\\Keyboard Layout";
static const std::wstring Preload = Base + L"\\Preload";
static const std::wstring Substitutes = Base + L"\\Substitutes";
static const std::wstring Toggle = Base + L"\\Toggle";
static void reset(const std::string &name, unsigned fault)
{
    ++Cases;
    Case = name;
    Fault = fault;
    Failed = false;
    CallIndex = InvalidCloses = UnexpectedCalls = 0;
    Operations.clear();
    Writes.clear();
    Handles.clear();
}
static void cleanup()
{
    EXPECT(!UnexpectedCalls && !InvalidCloses);
    for (const auto &entry : Handles)
        EXPECT(!entry.second.Live && entry.second.Closes == 1);
}
static void exercise(const char *name, const MUI_LAYOUTS *layouts, std::vector<Operation> expectedWrites)
{
    expectedWrites.push_back({'S', Toggle, L"Hotkey", L"1"});
    expectedWrites.push_back({'S', Toggle, L"Language Hotkey", L"1"});
    expectedWrites.push_back({'S', Toggle, L"Layout Hotkey", L"2"});
    reset(name, 0);
    EXPECT(AddKbLayoutsToRegistry(layouts));
    EXPECT(!Failed && Handles.size() == 4);
    EXPECT(Writes == expectedWrites);
    cleanup();
    const auto successful = Operations;
    for (unsigned fault = 1; fault <= successful.size(); ++fault) {
        reset(std::string(name) + " failure " + std::to_string(fault), fault);
        EXPECT(!AddKbLayoutsToRegistry(layouts));
        EXPECT(Failed && CallIndex == fault);
        EXPECT(Operations == std::vector<Operation>(successful.begin(), successful.begin() + fault));
        std::vector<Operation> expectedPrefix;
        unsigned acquired = 0;
        for (unsigned index = 0; index + 1 < fault; ++index) {
            if (successful[index].Kind == 'S')
                expectedPrefix.push_back(successful[index]);
            acquired += successful[index].Kind == 'C';
        }
        EXPECT(Writes == expectedPrefix);
        EXPECT(Handles.size() == acquired);
        if (successful[fault - 1].Kind == 'C' && successful[fault - 1].Key == Substitutes)
            SubstituteFailureInvalidCloses += InvalidCloses;
        cleanup();
    }
    reset(std::string(name) + " repeated success", 0);
    EXPECT(AddKbLayoutsToRegistry(layouts));
    EXPECT(Operations == successful && Writes == expectedWrites);
    cleanup();
}
int main()
{
    const MUI_LAYOUTS empty[] = {{0, 0}};
    const MUI_LAYOUTS native[] = {{0x0409, 0x00000409}, {0, 0}};
    const MUI_LAYOUTS substitute[] = {{0x0409, 0x00010409}, {0, 0}};
    const MUI_LAYOUTS mixed[] = {{0x0409, 0x00000409}, {0x0409, 0x00010409}, {0x0411, 0x00000411}, {0, 0}};
    for (bool existing : {false, true}) {
        ExistingKeys = existing;
        exercise("empty", empty, {});
        exercise("native", native, {{'S', Preload, L"1", L"00000409"}});
        exercise("substitute", substitute, {{'S', Preload, L"1", L"d0000409"},
                                             {'S', Substitutes, L"d0000409", L"00010409"}});
        exercise("mixed", mixed, {{'S', Preload, L"1", L"00000409"},
                                   {'S', Preload, L"2", L"d0000409"},
                                   {'S', Substitutes, L"d0000409", L"00010409"},
                                   {'S', Preload, L"3", L"00000411"}});
    }
    std::cout << "Keyboard registry cleanup: " << Cases << " cases, " << Checks << " checks, "
              << Failures << " failures\n";
    if (SubstituteFailureInvalidCloses)
        std::cout << "Unowned Substitutes cleanup failures: " << SubstituteFailureInvalidCloses << "\n";
    return Failures ? 1 : 0;
}
'''


def program(source):
    marker = '#define IN\n'
    prefix = PREFIX.replace(marker, constants() + marker, 1)
    return prefix + function(source, 'AddHotkeySettings', 'static BOOLEAN') + function(
        source, 'AddKbLayoutsToRegistry', 'BOOLEAN') + TESTS


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default='clang++')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--verify-regression', nargs='?', const='HEAD')
    args = parser.parse_args()
    sources = [('current', (ROOT / SOURCE).read_text())]
    if args.verify_regression:
        prior = subprocess.check_output(['git', 'show', f'{args.verify_regression}:{SOURCE}'], cwd=ROOT).decode()
        sources.append((args.verify_regression, prior))
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        for name, source in sources:
            (path / 'test.cpp').write_text(program(source))
            command = [args.cxx, '-std=c++17', '-Wall', '-Wextra', '-Werror',
                       '-ftrivial-auto-var-init=pattern', str(path / 'test.cpp'), '-o', str(path / 'test')]
            if args.sanitize:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
            subprocess.run(command, check=True)
            result = subprocess.run([str(path / 'test')], text=True, capture_output=True)
            print(f'{name}: {result.stdout}', end='')
            if result.stderr:
                print(result.stderr, end='')
            if name == 'current' and result.returncode:
                return result.returncode
            if name != 'current':
                if result.returncode != 1 or 'Unowned Substitutes cleanup failures: 8\n' not in result.stdout:
                    raise RuntimeError('Original code did not fail for the unowned Substitutes handle')
                print('Original unowned-handle cleanup failed as expected')
    print('Host model only; Windows 11 registry behavior was not compared.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
