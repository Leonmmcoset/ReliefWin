# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise BIOS preference I/O through the actual FAT adapter
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[2]
work = Path('/tmp/reliefwin-bootfile-test')
work.mkdir(exist_ok=True)
header = r'''
#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <ff.h>
typedef void VOID, *PVOID;
typedef char CHAR, *PCHAR;
typedef const char *PCSTR;
typedef const wchar_t *PCWSTR;
typedef uint8_t UCHAR, *PUCHAR;
typedef uint32_t ULONG, *PULONG;
typedef uint64_t ULONGLONG, *PULONGLONG;
typedef size_t SIZE_T;
typedef int BOOLEAN, *PBOOLEAN, NTSTATUS;
typedef struct _DK_DEVICE {void *Firmware; PUCHAR Memory; ULONGLONG Length, Offset;} DK_DEVICE, *PDK_DEVICE;
typedef void *PNTFS_VOLUME_INFO;
#define _In_
#define _Out_
#define _In_reads_bytes_(x)
#define _Out_writes_bytes_(x)
#define _Out_opt_
#define _In_opt_
#define TRUE 1
#define FALSE 0
#define MAXULONG UINT32_MAX
#define ANSI_NULL 0
#define UNICODE_NULL 0
#define BM_PATH_CHARS 520
#define NT_SUCCESS(x) ((x) >= 0)
#define RtlZeroMemory(p,n) memset(p,0,n)
#define RtlCopyMemory memcpy
#define RtlEqualMemory(p,q,n) (!memcmp(p,q,n))
#define RTL_NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))
PVOID FwAllocate(SIZE_T Size);
PVOID FwAllocatePages(SIZE_T Size);
VOID FwFree(PVOID Data);
VOID FwFreePages(PVOID Data, SIZE_T Size);
BOOLEAN DkRead(PDK_DEVICE Device, ULONGLONG Offset, ULONG Size, PVOID Data);
BOOLEAN DkWrite(PDK_DEVICE Device, ULONGLONG Offset, ULONG Size, const VOID *Data);
BOOLEAN DkFlush(PDK_DEVICE Device);
BOOLEAN FwGetBootDevice(PDK_DEVICE Device);
PNTFS_VOLUME_INFO NtfsMount(PDK_DEVICE Device);
PVOID NtfsReadFile(PNTFS_VOLUME_INFO Volume, PCSTR Path, PULONG Size);
PVOID NtfsOpenFile(PNTFS_VOLUME_INFO Volume, PCSTR Path, PULONGLONG Size);
BOOLEAN NtfsReadFileAt(PVOID File, ULONGLONG Offset, PVOID Buffer, ULONG Size);
VOID NtfsCloseFile(PVOID File);
BOOLEAN CdfsMount(PDK_DEVICE Device);
PVOID CdfsReadFile(PCSTR Path, PULONG Size);
NTSTATUS NtfsBlMount(PDK_DEVICE Device);
NTSTATUS NtfsBlUnmount(void);
NTSTATUS NtfsBlOpenFile(PCSTR Path, PVOID *File, PULONGLONG Size, PBOOLEAN Directory);
NTSTATUS NtfsBlReadFileAt(PVOID File, ULONGLONG Offset, PVOID Buffer, ULONG Size, PULONG Read);
VOID NtfsBlCloseFile(PVOID File);
NTSTATUS NtfsBlWriteFile(PCSTR Path, const VOID *Data, ULONG Size);
'''
(work / 'bl.h').write_text(header)
(work / 'ntfsbl.h').write_text('#include <bl.h>\n')
source = (root / 'lib/exfat.c').read_text()
source += (root / 'lib/fs.c').read_text().replace('#include "ntfs.h"', '')
source += (root / 'bios/bootfile.c').read_text()
source += (root / 'gui/tests/bootfile.c').read_text()
(work / 'test.c').write_text(source)
fatfs = root.parent / 'sdk/lib/3rdparty/fatfs'
binary = work / 'test'
subprocess.run(['clang', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-Wno-unused-parameter', '-fsanitize=address,undefined', '-D__REACTOS__',
                '-I' + str(work), '-I' + str(fatfs), str(work / 'test.c'),
                str(fatfs / 'ff.c'), str(fatfs / 'ffunicode.c'), '-o', str(binary)], check=True)
subprocess.run([str(binary)], check=True)
