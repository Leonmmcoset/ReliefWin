#!/usr/bin/env python3
"""
PROJECT:     ReliefWin installation tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Verify Office installation ACLs through the actual NTFS policy tool
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess


ROOT = Path(__file__).resolve().parents[1]
OFFICE = "Program Files/ReliefOffice"
FILES = (
    "reliefword.exe", "reliefexcel.exe", "reliefppt.exe", "relieftodo.exe",
    "reliefdiagram.exe", "reliefmathpp.exe", "reliefmath.exe", "reliefmath-render.exe",
    "ReliefMath/STIXTwoMath-Regular.otf", "reliefoffice.inf", "UPSTREAM_NOTICE",
    "UPSTREAM_LICENSE", "THIRD_PARTY_LICENSES.txt", "reliefoffice-maintenance.exe",
)
DIRECTORIES = (
    "Program Files", OFFICE, OFFICE + "/ReliefMath", OFFICE + "/用户目录",
    "Program Files/ReliefOfficeExtra", "Program Files/ReliefOffice.old",
    "Program Files/Common Files", "Windows", "Windows/system32",
)
OTHER_FILES = (
    OFFICE + "/用户目录/文档.txt",
    "Program Files/ReliefOfficeExtra/fixture.exe",
    "Program Files/ReliefOffice.old/fixture.exe",
    "Program Files/Common Files/fixture.dll", "Windows/system32/fixture.dll",
)


def run(arguments):
    result = subprocess.run([str(value) for value in arguments], capture_output=True)
    assert result.returncode == 0, repr(arguments) + ":\n" + result.stderr.decode(errors="replace")
    return result.stdout


def sid(data, offset):
    revision, count = struct.unpack_from("BB", data, offset)
    authority = int.from_bytes(data[offset + 2:offset + 8], "big")
    values = struct.unpack_from("<" + "I" * count, data, offset + 8)
    return "S-" + "-".join(map(str, (revision, authority) + values))


def descriptor(data):
    revision, _, control, owner, group, sacl, dacl = struct.unpack_from("<BBHIIII", data)
    assert revision == 1 and control & 0x8004 == 0x8004 and dacl and not sacl
    _, _, size, count, _ = struct.unpack_from("<BBHHH", data, dacl)
    offset, allowed = dacl + 8, {}
    for _ in range(count):
        kind, flags, length, mask = struct.unpack_from("<BBHI", data, offset)
        assert kind == 0 and length >= 16 and offset + length <= dacl + size
        if not flags & 0x08:
            trustee = sid(data, offset + 8)
            allowed[trustee] = allowed.get(trustee, 0) | mask
        offset += length
    assert offset == dacl + size
    return {"owner": sid(data, owner), "group": sid(data, group), "allowed": allowed}


def actual_matcher(work, policies, queries):
    source = (ROOT / "base/setup/reliefwin/filesec.c").read_text()
    structures = source[source.index("typedef struct _FILESEC_DEFINE"):source.index("typedef struct _FILESEC_WALK")]
    parser = source[source.index("static PWSTR\nFileSecDuplicate"):source.index("DWORD\nFileSecurityLoadPolicy")]
    start = source.index("PSECURITY_DESCRIPTOR\nFileSecurityFindDescriptor")
    end = source.index("\nstatic NTSTATUS\nFileSecApplyToHandle", start)
    matcher = source[start:end]
    harness = work / "policy-matcher.c"
    harness.write_text(r'''
#define _GNU_SOURCE
#include <ctype.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#define _In_
#define _Inout_
#define _In_opt_
#define _In_reads_(x)
#define _Out_writes_(x)
#define _Inout_updates_(x)
#define TRUE 1
#define FALSE 0
#define UNICODE_NULL 0
#define HEAP_ZERO_MEMORY 8
#define SDDL_REVISION_1 1
#define SE_DACL_AUTO_INHERITED 0x400
#define SE_DACL_AUTO_INHERIT_REQ 0x100
#define _wcsicmp wcscasecmp
#define _wcsnicmp wcsncasecmp
#define CopyMemory memcpy
#define DPRINT1(...) ((void)0)
#define GetProcessHeap() NULL
#define HeapAlloc(heap, flags, size) calloc(1, size)
#define HeapReAlloc(heap, flags, pointer, size) realloc(pointer, size)
#define HeapFree(heap, flags, pointer) free(pointer)
#define LocalFree free
#define GetLastError() 0
#define FILESEC_MAX_VALUE 8192
typedef void VOID, *PVOID;
typedef wchar_t WCHAR, *PWSTR;
typedef const wchar_t *PCWSTR;
typedef size_t SIZE_T;
typedef uint32_t ULONG, *PULONG, DWORD;
typedef int BOOLEAN;
typedef uint16_t SECURITY_DESCRIPTOR_CONTROL;
typedef PWSTR PSECURITY_DESCRIPTOR;
typedef struct _FILESEC_POLICY FILESEC_POLICY, *PFILESEC_POLICY;
static int ConvertStringSecurityDescriptorToSecurityDescriptorW(
    PCWSTR text, DWORD revision, PSECURITY_DESCRIPTOR *result, void *size)
{
    (void)revision;
    (void)size;
    *result = wcsdup(text);
    return *result != NULL;
}
static int GetSecurityDescriptorControl(PSECURITY_DESCRIPTOR descriptor,
    SECURITY_DESCRIPTOR_CONTROL *control, DWORD *revision)
{
    (void)descriptor;
    *control = 0;
    *revision = 1;
    return 1;
}
static void SetSecurityDescriptorControl(PSECURITY_DESCRIPTOR descriptor,
    SECURITY_DESCRIPTOR_CONTROL bits, SECURITY_DESCRIPTOR_CONTROL value)
{
    (void)descriptor;
    (void)bits;
    (void)value;
}
''' + structures + parser + matcher + r'''
int main(int argc, char **argv)
{
    PFILESEC_POLICY policy = calloc(1, sizeof(*policy));
    FILE *input;
    char *line = NULL;
    size_t capacity = 0;
    wchar_t text[16384], expanded[FILESEC_MAX_VALUE];
    if (argc != 3 || !policy || !setlocale(LC_ALL, "C.UTF-8"))
        return 2;
    input = fopen(argv[1], "rb");
    if (!input)
        return 3;
    while (getline(&line, &capacity, input) >= 0)
    {
        line[strcspn(line, "\n")] = 0;
        if (mbstowcs(text, line, 16384) == (size_t)-1 ||
            !FileSecParseLine(policy, text, expanded))
            return 4;
    }
    fclose(input);
    input = fopen(argv[2], "rb");
    if (!input)
        return 5;
    while (getline(&line, &capacity, input) >= 0)
    {
        PSECURITY_DESCRIPTOR result;
        line[strcspn(line, "\r\n")] = 0;
        if (mbstowcs(text, line + 2, 16384) == (size_t)-1)
            return 6;
        FileSecNormalizePath(text);
        result = FileSecurityFindDescriptor(policy, text, line[0] == 'D');
        wprintf(L"%ls\n", result ? result : L"(none)");
    }
    fclose(input);
    free(line);
    FileSecurityFreePolicy(policy);
    return 0;
}
''')
    program = work / "policy-matcher"
    run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", harness, "-o", program])
    query_file = work / "queries.txt"
    query_file.write_text("".join(("D" if directory else "F") + "|" + path + "\n"
                                  for path, directory in queries))
    results = []
    for policy in policies:
        results.append(run([program, policy, query_file]).decode().splitlines())
    return results


def definitions(text):
    result = {}
    for line in text.splitlines():
        if line.startswith("define "):
            name, value = line[7:].split("=", 1)
            result[name] = re.sub(r"\{([^}]+)\}", lambda match: result[match[1]], value)
    return result


def main():
    arguments = argparse.ArgumentParser(description=__doc__)
    arguments.add_argument("--ntfsimg", type=Path,
                           default=ROOT / "output-Clang-amd64-debug/host-tools/bin/ntfsimg")
    arguments.add_argument("--work-dir", type=Path,
                           default=Path("/tmp/reliefwin-brand-host/office-security"))
    arguments.add_argument("--report", type=Path)
    args = arguments.parse_args()
    work = args.work_dir.resolve()
    work.mkdir(parents=True, exist_ok=True)
    image = work / "security.ntfs"
    current = ROOT / "image/data/filesecurity.txt"
    text = current.read_text()
    old_text, removed = re.subn(r"^(?:dir|dirs|files) Program Files/ReliefOffice=.*\n", "", text, flags=re.M)
    assert removed == 3, "Office root, recursive directory and recursive file rules"
    old = work / "old-policy.txt"
    old.write_text(old_text)
    paths = [(path, True) for path in DIRECTORIES]
    paths += [(OFFICE + "/" + name, False) for name in FILES]
    paths += [(path, False) for path in OTHER_FILES]
    queries = paths + [("/pROGRAM fILES\\rELIEFOffice/", True),
                       ("/Program Files\\ReliefOffice\\ReliefMath/", True),
                       ("/Program Files\\ReliefOffice\\reliefword.exe", False),
                       ("Program Files/ReliefOffice", False)]
    old_matches, new_matches = actual_matcher(work, (old, current), queries)
    assert len(old_matches) == len(new_matches) == len(queries)
    values = definitions(text)
    checks = 0
    for (path, directory), before, after in zip(queries, old_matches, new_matches):
        normalized = path.replace("\\", "/").strip("/").lower()
        inside = normalized == OFFICE.lower() if directory else False
        inside |= normalized.startswith(OFFICE.lower() + "/")
        if inside:
            wanted = values["CREATEDDIR" if directory else "CREATEDFILE"]
            assert after == wanted and before != wanted, "actual installer matching: " + path
        else:
            assert after == before, "unrelated installer policy changed: " + path
        checks += 1
    run([args.ntfsimg, "--format", image, 131072, 0, 8, "OFFICEACL"])
    for path in DIRECTORIES:
        run([args.ntfsimg, "--create-dir", image, path])
    expected_data = {}
    for path, directory in paths:
        if not directory:
            payload = ("Office ACL fixture: " + path + "\n").encode()
            source = work / "payload.bin"
            source.write_bytes(payload)
            run([args.ntfsimg, "--create-file", image, path])
            run([args.ntfsimg, "--write", image, path, 0, source])
            expected_data[path] = hashlib.sha256(payload).hexdigest()
    run([args.ntfsimg, "--apply-security", image, old])
    before = {path: run([args.ntfsimg, "--security", image, path]) for path, _ in paths}
    run([args.ntfsimg, "--apply-security", image, current])
    final = {}
    for path, directory in paths:
        after = run([args.ntfsimg, "--security", image, path])
        final[path] = after
        inside = path == OFFICE or path.startswith(OFFICE + "/")
        if inside:
            was, now = descriptor(before[path]), descriptor(after)
            assert now["owner"] == now["group"] == "S-1-5-18", path
            for trustee in ("S-1-5-18", "S-1-5-32-544"):
                assert now["allowed"][trustee] == 0x1F01FF, path
                denied = 0x40 if directory else 0x10000
                assert not was["allowed"][trustee] & denied, "old regression did not deny deletion: " + path
                checks += 2
            assert now["allowed"]["S-1-5-32-545"] == 0x1200A9, path
            checks += 3
        else:
            assert after == before[path], "unrelated on-disk ACL changed: " + path
            checks += 1
        if not directory:
            actual = run([args.ntfsimg, "--cat", image, path])
            assert hashlib.sha256(actual).hexdigest() == expected_data[path], path
            checks += 1
    run([args.ntfsimg, "--apply-security", image, current])
    for path, _ in paths:
        assert run([args.ntfsimg, "--security", image, path]) == final[path], "policy not idempotent: " + path
        checks += 1
    report = {"checks": checks, "failures": 0, "fixture": str(image),
              "native_tool": str(args.ntfsimg.resolve()),
              "native_tool_sha256": hashlib.sha256(args.ntfsimg.read_bytes()).hexdigest(),
              "policy_sha256": hashlib.sha256(current.read_bytes()).hexdigest(),
              "old_policy_denies_delete": True,
              "office_directories": 3, "office_files": 15,
              "unrelated_descriptors_unchanged": 10,
              "all_payload_hashes_unchanged": True, "repeat_application_idempotent": True}
    if args.report:
        args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
