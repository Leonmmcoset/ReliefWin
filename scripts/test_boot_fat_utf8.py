#!/usr/bin/env python3
"""
PROJECT:     ReliefWin boot loader tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Check actual RAM FAT writer Unicode names and rejected-input state
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = "bootloader/ramdisk/fatwrite.c"


HEADER = r'''
#pragma once
#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define IN
#define OUT
#define _In_
#define _Inout_
#define _Out_
#define _Out_opt_
#define _In_opt_
#define _In_reads_(n)
#define _In_reads_bytes_(n)
#define _Inout_updates_(n)
#define _Out_writes_(n)
#define _Out_writes_z_(n)
#define _Out_writes_bytes_(n)
#define _Out_writes_bytes_to_(n,m)
#define _Out_writes_to_(n,m)
#define _Success_(x)
#define FORCEINLINE static inline
#define NTAPI
#define TRUE 1
#define FALSE 0
#define ANSI_NULL 0
#define UNICODE_NULL 0
#define MAXULONG UINT32_MAX
#define PARTITION_FAT32_XINT13 0x0c
#define STATUS_SUCCESS ((NTSTATUS)0)
#define STATUS_SOME_NOT_MAPPED ((NTSTATUS)0x107)
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xc0000023)
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xc000000d)
#define STATUS_INVALID_PARAMETER_4 ((NTSTATUS)0xc00000f2)
#define STATUS_INVALID_PARAMETER_5 ((NTSTATUS)0xc00000f3)
#define NT_SUCCESS(s) ((NTSTATUS)(s) >= 0)
#define ASSERT assert
#define C_ASSERT(x) _Static_assert(x, #x)
#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))
#define RTL_NUMBER_OF ARRAYSIZE
#define RtlCopyMemory memcpy
#define RtlZeroMemory(p,n) memset(p,0,n)
#define RtlFillMemory(p,n,v) memset(p,v,n)
#define RtlEqualMemory(p,q,n) (!memcmp(p,q,n))
#define TRACE(...) ((void)0)
#define WARN(...) ((void)0)
#define _stricmp strcasecmp
typedef uint8_t BOOLEAN, UCHAR, BYTE, *PUCHAR, *PBOOLEAN;
typedef uint16_t USHORT, WCHAR, *PWCHAR, *PWSTR;
typedef const WCHAR *PCWSTR;
typedef uint32_t ULONG, *PULONG;
typedef int32_t LONG, NTSTATUS;
typedef int16_t SHORT;
typedef uint64_t ULONGLONG;
typedef int64_t LONGLONG;
typedef uintptr_t ULONG_PTR;
typedef size_t SIZE_T;
typedef char CHAR, *PCHAR, *PSTR;
typedef const char *PCSTR;
typedef void VOID, *PVOID;
'''


HARNESS = r'''
#define VOLUME_BYTES (64U * 1024U * 1024U)
static unsigned long Checks;
static unsigned char *Volume, *Pristine, *Snapshot;
static RAMDISK_FAT32_LAYOUT Layout;
static FAT32_WRITER Writer;

static void check(int passed, const char *message, const char *name)
{
    Checks++;
    if (!passed)
    {
        fprintf(stderr, "FAIL: %s: %s\n", message, name);
        exit(1);
    }
}

static unsigned char *cluster(ULONG number)
{
    uint64_t offset = (uint64_t)Layout.FirstDataSector * 512 +
                      (uint64_t)(number - 2) * Layout.SectorsPerCluster * 512;
    check(number >= 2 && offset + Layout.SectorsPerCluster * 512 <= VOLUME_BYTES,
          "independent directory bounds", "");
    return Volume + 512 + offset;
}

static ULONG next_cluster(ULONG number)
{
    uint32_t value;
    memcpy(&value, Volume + 512 + (uint64_t)Layout.ReservedSectors * 512 + number * 4, 4);
    return value >= 0x0ffffff8 ? 0 : value;
}

static int independent_name(ULONG directory, const uint16_t *wanted, size_t length,
                            unsigned char *short_name, ULONG *file_cluster)
{
    uint16_t units[260];
    size_t lfn_count = 0;
    unsigned guard = 0;
    memset(units, 0xff, sizeof(units));
    while (directory)
    {
        unsigned char *entries = cluster(directory);
        size_t offset;
        check(++guard < 1000, "independent directory chain terminates", "");
        for (offset = 0; offset < Layout.SectorsPerCluster * 512; offset += 32)
        {
            unsigned char *entry = entries + offset;
            if (!entry[0])
                return 0;
            if (entry[0] == 0xe5)
            {
                lfn_count = 0;
                continue;
            }
            if (entry[11] == 0x0f)
            {
                static const unsigned fields[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
                unsigned sequence = entry[0] & 0x1f;
                size_t index;
                if (entry[0] & 0x40)
                {
                    memset(units, 0xff, sizeof(units));
                    lfn_count = sequence;
                }
                check(sequence && sequence <= 20, "LFN sequence range", "");
                for (index = 0; index < 13; index++)
                    units[(sequence - 1) * 13 + index] =
                        entry[fields[index]] | ((uint16_t)entry[fields[index] + 1] << 8);
                continue;
            }
            if (!(entry[11] & 0x08))
            {
                size_t count = 0;
                if (lfn_count)
                {
                    while (count < lfn_count * 13 && units[count] && units[count] != 0xffff)
                        count++;
                }
                else
                {
                    size_t index;
                    for (index = 0; index < 8 && entry[index] != ' '; index++)
                        units[count++] = (entry[12] & 0x08) ? tolower(entry[index]) : entry[index];
                    if (entry[8] != ' ')
                    {
                        units[count++] = '.';
                        for (index = 8; index < 11 && entry[index] != ' '; index++)
                            units[count++] = (entry[12] & 0x10) ? tolower(entry[index]) : entry[index];
                    }
                }
                if (count == length && !memcmp(units, wanted, length * sizeof(uint16_t)))
                {
                    if (lfn_count)
                    {
                        size_t index;
                        check(lfn_count == (length + 12) / 13, "LFN slots use UTF-16 units", "");
                        if (length < lfn_count * 13)
                        {
                            check(units[length] == 0, "LFN terminator", "");
                            for (index = length + 1; index < lfn_count * 13; index++)
                                check(units[index] == 0xffff, "LFN padding", "");
                        }
                    }
                    if (short_name)
                        memcpy(short_name, entry, 11);
                    if (file_cluster)
                        *file_cluster = entry[26] | ((ULONG)entry[27] << 8) |
                                        ((ULONG)entry[20] << 16) | ((ULONG)entry[21] << 24);
                    return 1;
                }
            }
            lfn_count = 0;
        }
        directory = next_cluster(directory);
    }
    return 0;
}

static void reset_volume(void)
{
    memcpy(Volume, Pristine, VOLUME_BYTES);
    check(Fat32WriterInit(&Writer, Volume + 512, VOLUME_BYTES - 512, &Layout), "writer init", "");
}

static void valid_name(const char *name, const uint16_t *expected, size_t units)
{
    unsigned char *data = NULL, *found;
    ULONG size = 0, allocated;
    char upper[256];
    size_t index;
    reset_volume();
    check(Fat32CreateFileEx(&Writer, name, 4, &data), "create valid UTF-8", name);
    check(data != NULL, "create supplies data pointer", name);
    memcpy(data, "TEST", 4);
    check(independent_name(2, expected, units, NULL, &allocated), "unicode LFN mismatch", name);
    found = Fat32FindFile(&Writer, name, &size);
    check(found && size == 4 && !memcmp(found, "TEST", 4), "unicode file lookup", name);
    check(cluster(allocated) == found, "lookup points at independently decoded entry", name);
    for (index = 0; name[index]; index++)
        upper[index] = name[index] >= 'a' && name[index] <= 'z' ? name[index] - ('a' - 'A') : name[index];
    upper[index] = 0;
    found = Fat32FindFile(&Writer, upper, &size);
    check(found && size == 4 && !memcmp(found, "TEST", 4), "ASCII case-folded UTF-8 lookup", name);
}

static void invalid_name(const char *name)
{
    unsigned char *data = (unsigned char *)(uintptr_t)1;
    FAT32_WRITER before;
    reset_volume();
    memcpy(Snapshot, Volume, VOLUME_BYTES);
    before = Writer;
    check(!Fat32CreateFileEx(&Writer, name, 4, &data), "invalid UTF-8 file accepted", "invalid input");
    check(data == NULL, "failed create clears output", "invalid input");
    check(!memcmp(&before, &Writer, sizeof(Writer)), "failed file create mutates allocation state", "invalid input");
    check(!memcmp(Snapshot, Volume, VOLUME_BYTES), "failed file create mutates volume", "invalid input");
    check(!Fat32CreateDirectory(&Writer, name), "invalid UTF-8 directory accepted", "invalid input");
    check(!memcmp(&before, &Writer, sizeof(Writer)), "failed directory create mutates allocation state", "invalid input");
    check(!memcmp(Snapshot, Volume, VOLUME_BYTES), "failed directory create mutates volume", "invalid input");
}

static void nested_and_collisions(void)
{
    static const uint16_t first[] = {0x4e2d,0x6587,0x76ee,0x5f55};
    static const uint16_t a[] = {0x6587,0x4ef6,0x4e00,0x2e,0x74,0x78,0x74};
    static const uint16_t b[] = {0x6587,0x4ef6,0x4e8c,0x2e,0x74,0x78,0x74};
    unsigned char short_a[11], short_b[11], *data, *found;
    ULONG directory, size, allocated;
    reset_volume();
    check(Fat32CreateDirectory(&Writer, "中文目录"), "create unicode directory", "中文目录");
    check(independent_name(2, first, ARRAYSIZE(first), NULL, &directory), "unicode directory name", "中文目录");
    allocated = Writer.NextFreeCluster;
    memcpy(Snapshot, Volume, VOLUME_BYTES);
    check(Fat32CreateDirectory(&Writer, "中文目录"), "repeat unicode directory", "中文目录");
    check(Writer.NextFreeCluster == allocated && !memcmp(Snapshot, Volume, VOLUME_BYTES),
          "repeat directory allocates or duplicates entry", "中文目录");
    check(Fat32CreateFileEx(&Writer, "中文目录/文件一.txt", 1, &data), "nested UTF-8 file", "文件一.txt");
    *data = 17;
    check(Fat32CreateFileEx(&Writer, "中文目录/文件二.txt", 1, &data), "colliding UTF-8 file", "文件二.txt");
    *data = 23;
    check(independent_name(directory, a, ARRAYSIZE(a), short_a, NULL), "first collision LFN", "文件一.txt");
    check(independent_name(directory, b, ARRAYSIZE(b), short_b, NULL), "second collision LFN", "文件二.txt");
    check(memcmp(short_a, short_b, 11) != 0, "short name collision not resolved", "");
    found = Fat32FindFile(&Writer, "中文目录/文件一.txt", &size);
    check(found && size == 1 && *found == 17, "first collision lookup", "");
    found = Fat32FindFile(&Writer, "中文目录\\文件二.txt", &size);
    check(found && size == 1 && *found == 23, "second collision lookup", "");
    memcpy(Snapshot, Volume, VOLUME_BYTES);
    allocated = Writer.NextFreeCluster;
    check(!Fat32CreateFileEx(&Writer, "中文目录/\xe0\x80\x80.txt", 1, &data), "invalid nested file accepted", "");
    check(Writer.NextFreeCluster == allocated && !memcmp(Snapshot, Volume, VOLUME_BYTES),
          "invalid nested file mutates existing directory", "");
    check(Fat32CreateFileEx(&Writer, "中文目录/丁.txt", 1, &data), "first shared-low-byte name", "");
    *data = 31;
    check(Fat32CreateFileEx(&Writer, "中文目录/企.txt", 1, &data), "second shared-low-byte name", "");
    *data = 47;
    found = Fat32FindFile(&Writer, "中文目录/丁.txt", &size);
    check(found && size == 1 && *found == 31, "first UTF-16 low-byte collision lookup", "");
    found = Fat32FindFile(&Writer, "中文目录/企.txt", &size);
    check(found && size == 1 && *found == 47, "second UTF-16 low-byte collision lookup", "");
}

int main(void)
{
    Volume = malloc(VOLUME_BYTES);
    Pristine = malloc(VOLUME_BYTES);
    Snapshot = malloc(VOLUME_BYTES);
    check(Volume && Pristine && Snapshot, "host allocations", "");
    check(RamDiskFormatFat32(Volume, VOLUME_BYTES, &Layout), "actual RAM FAT formatter", "");
    memcpy(Pristine, Volume, VOLUME_BYTES);
    VALID_CASES
    nested_and_collisions();
    INVALID_CASES
    printf("RAM FAT UTF-8 checks: %lu, failures: 0\n", Checks);
    free(Snapshot);
    free(Pristine);
    free(Volume);
    return 0;
}
'''


def byte_string(value):
    return '"' + "".join("\\%03o" % byte for byte in value) + '"'


def cases():
    valid = ["安装 ReliefWin.lnk", "使用说明.lnk", "readme.txt", "MiXeD.txt", "启动😀说明.lnk",
             "😀.txt", "abcé.txt", "中文资料目录", "replacement-�.txt", "a" * 255, "中" * 85, "😀" * 63]
    valid += ["a" * count for count in (12, 13, 14, 25, 26, 27)]
    valid += ["a" * count + "😀" for count in (11, 12, 13, 24, 25, 26)]
    generated = []
    for name in valid:
        units = name.encode("utf-16le")
        values = [str(int.from_bytes(units[i:i + 2], "little")) for i in range(0, len(units), 2)]
        generated.append("{static const uint16_t expected[]={" + ",".join(values) + "};valid_name(" +
                         byte_string(name.encode()) + ",expected,ARRAYSIZE(expected));}")
    bad = [b"\x80", b"\xbf", b"\xc0\x80", b"\xc1\xbf", b"\xc2", b"\xc2A", b"\xdf",
           b"\xe0\x80\x80", b"\xe0\x9f\xbf", b"\xe1\x80", b"\xe1\x80A", b"\xed\xa0\x80",
           b"\xed\xbf\xbf", b"\xf0\x80\x80\x80", b"\xf0\x8f\xbf\xbf", b"\xf1\x80\x80",
           b"\xf4\x90\x80\x80", b"\xf5\x80\x80\x80", b"\xff", b"\xfe", b"\xe2(\xa1", b"\xef\xbf\xbf",
           b"a" * 256, ("中" * 86).encode(), ("😀" * 64).encode()]
    invalid = ["invalid_name(" + byte_string(value + b".txt" if len(value) < 250 else value) + ");"
               for value in bad]
    return "\n".join(generated), "\n".join(invalid)


def compile_harness(work, source, sanitize=False):
    (work / "bl.h").write_text(HEADER)
    (work / "rtl_native.h").write_text('#include "bl.h"\n')
    (work / "debug.h").write_text("")
    actual = work / "fatwrite-under-test.c"
    actual.write_text(source)
    valid, invalid = cases()
    harness = work / "boot-fat-utf8.c"
    includes = ['#include "bl.h"', '#include "ramdisk.h"',
                '#include "' + str(ROOT / "sdk/lib/rtl/utf8.c") + '"',
                '#include "' + str(ROOT / "bootloader/ramdisk/fatformat.c") + '"',
                ]
    casefold = (ROOT / "sdk/lib/rtl/boot/nlsboot.c").read_text()
    begin = casefold.index("WCHAR\nNTAPI\nRtlpUpcaseUnicodeChar")
    end = casefold.index("_Use_decl_annotations_", begin)
    includes += [casefold[begin:end], '#include "fatwrite-under-test.c"']
    harness.write_text("\n".join(includes) + "\n" + HARNESS.replace("INVALID_CASES", invalid).replace("VALID_CASES", valid))
    program = work / ("boot-fat-utf8-asan" if sanitize else "boot-fat-utf8")
    command = ["cc", "-std=c11", "-g", "-O1", "-Wall", "-Wextra", "-Werror",
               "-I", str(work), "-I", str(ROOT / "bootloader/include"), str(harness), "-o", str(program)]
    if sanitize:
        command[1:1] = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, result.stdout + result.stderr
    return subprocess.run([str(program)], capture_output=True, text=True)


def main():
    arguments = argparse.ArgumentParser(description=__doc__)
    arguments.add_argument("--baseline", help="exercise this Git revision without changing the checkout")
    arguments.add_argument("--verify-regression", action="store_true")
    arguments.add_argument("--sanitize", action="store_true")
    args = arguments.parse_args()
    work = Path(tempfile.gettempdir()) / "reliefwin-brand-host/boot-fat-utf8"
    work.mkdir(parents=True, exist_ok=True)
    if args.verify_regression:
        previous = subprocess.check_output(["git", "show", "e01ee711:" + SOURCE], cwd=ROOT).decode()
        result = compile_harness(work, previous)
        assert result.returncode != 0 and "FAIL: unicode LFN mismatch:" in result.stderr, result.stdout + result.stderr
        print("Previous writer rejected by independent UTF-16 LFN check")
    source = (subprocess.check_output(["git", "show", args.baseline + ":" + SOURCE], cwd=ROOT).decode()
              if args.baseline else (ROOT / SOURCE).read_text())
    result = compile_harness(work, source, args.sanitize)
    print(result.stdout, end="")
    if result.returncode:
        print(result.stderr, end="")
        raise SystemExit(result.returncode)
    print(json.dumps({"actual_writer": SOURCE, "actual_formatter": "bootloader/ramdisk/fatformat.c",
                      "actual_conversion": "sdk/lib/rtl/utf8.c",
                      "actual_casefold": "sdk/lib/rtl/boot/nlsboot.c",
                      "writer_sha256": hashlib.sha256(source.encode()).hexdigest(),
                      "sanitizers": args.sanitize}))


if __name__ == "__main__":
    main()
