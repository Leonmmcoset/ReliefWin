#!/usr/bin/env python3
# PROJECT:     ReliefWin host tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify Linux runtime removal in sources and generated artifacts
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
from pathlib import Path
import re
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
from test_boot_brand import function


ROOT = Path(__file__).resolve().parents[1]
RETIRED = (
    'subsystems/linux',
    'submodules/lxgpu',
    'submodules/lxgpu.cmake',
    'submodules/host-tests/lxcore',
    'ntos/ps/pico.c',
    'sdk/include/reliefwin/pico.h',
    'modules/rostests/kmtests/ntos_ps/PsPico.c',
)
RUNTIME = re.compile(
    r'\b(?:lxcore|lxterm|lxgpu|lxwayland|lxwinhost|lxlaunch|lxrun|lxsession|lxmm)\b'
    r'|\b(?:PsPico|PspPico|KiPico|TestLxMm)\w*'
    r'|ENABLE_LXGPU|subsystems[/\\]linux|reliefwin[/\\]pico\.h'
    r'|Linux 终端\.lnk|Linux Terminal\.lnk', re.IGNORECASE)
TEXT_SUFFIXES = {
    '.c', '.cpp', '.cxx', '.h', '.hpp', '.s', '.asm', '.inc', '.in',
    '.rc', '.inf', '.sif', '.cmake', '.py', '.json', '.md', '.txt', '.spec',
}


def check_source(baseline):
    if baseline:
        names = subprocess.check_output(
            ['git', 'ls-tree', '-rz', '--name-only', baseline], cwd=ROOT
        ).decode().split('\0')
        present = [prefix for prefix in RETIRED
                   if any(name == prefix or name.startswith(prefix + '/') for name in names)]
        assert len(present) >= 6, 'Baseline does not contain the retired Linux runtime'
        print(f'{baseline}: confirmed the Linux runtime existed before removal')
        return
    names = subprocess.check_output(
        ['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard'], cwd=ROOT
    ).decode().split('\0')
    names = [name for name in names if (ROOT / name).is_file()]
    remaining = [prefix for prefix in RETIRED
                 if any(name == prefix or name.startswith(prefix + '/') for name in names)]
    assert not remaining, 'Removed components still present: ' + ', '.join(remaining)
    failures = []
    checked = 0
    for name in names:
        if name == str(Path(__file__).relative_to(ROOT)):
            continue
        path = ROOT / name
        if path.suffix.lower() not in TEXT_SUFFIXES and path.name != 'CMakeLists.txt':
            continue
        data = path.read_bytes()
        if data.startswith((b'\xff\xfe', b'\xfe\xff')):
            source = data.decode('utf-16')
        elif b'\0' in data:
            continue
        else:
            source = data.decode('utf-8-sig', errors='replace')
        checked += 1
        for number, line in enumerate(source.splitlines(), 1):
            if RUNTIME.search(line):
                failures.append(f'{name}:{number}: {line.strip()}')
            if name.startswith(('ntos/', 'drivers/filesystems/')) and re.search(
                    r'FileStatLxInformation|FILE_STAT_LX_INFORMATION|LX_FILE_METADATA_'
                    r'|NtfsQueryLxMetadata|Ext4FillStatLx', line):
                failures.append(f'{name}:{number}: {line.strip()}')
    assert not failures, '\n'.join(failures)
    print(f'Source audit: {checked} text files, no removed runtime references')


def check_build(build):
    build = build.resolve()
    for name in ('build.ninja', 'boot/preinstall.Debug.lst',
                 'boot/livecd.Debug.lst', 'boot/bootcd.Debug.lst'):
        path = build / name
        assert path.is_file(), f'Missing build artifact: {path}'
        assert not RUNTIME.search(path.read_text()), f'Removed runtime in {path}'
    for name in ('subsystems/linux', 'submodules/lxgpu'):
        assert not (build / name).exists(), f'Stale output: {build / name}'
    stale = list(build.glob('ntos/CMakeFiles/*/ps/pico.c.obj'))
    for name in ('lxcore.sys', 'lxrun.exe', 'lxterm.exe', 'lxwinhost.exe', 'lxgpu.exe'):
        if (build / 'symbols' / name).exists():
            stale.append(build / 'symbols' / name)
    assert not stale, 'Stale output: ' + ', '.join(map(str, stale))
    kernel = build / 'ntos/ntoskrnl.exe'
    exports = subprocess.check_output(['llvm-readobj', '--coff-exports', str(kernel)], text=True)
    assert not re.search(r'\b(?:PsPico\w*|PsRegisterPicoProvider)\b', exports), str(kernel)
    for mode in ('preinstall', ''):
        hives = build / 'boot/bootdata' / mode
        for name in ('system', 'software', 'default'):
            path = hives / name
            assert path.is_file(), f'Missing generated hive: {path}'
            data = path.read_bytes()
            for value in ('lxcore', 'lxterm.exe', 'Linux Terminal', 'Linux 终端.lnk'):
                for encoding in ('utf-8', 'utf-16le'):
                    assert value.encode(encoding) not in data, f'{value} remains in {path}'
    print(f'{build.name}: image manifests, kernel exports, generated hives and stale outputs passed')


def check_ext4_eas():
    source = (ROOT / 'drivers/filesystems/ext4/ea.c').read_text()
    harness = r'''
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
typedef int32_t NTSTATUS;
#include "ntstatus.h"
#define _In_
#define _Out_
#define _Inout_
#define _In_reads_bytes_(x)
#define _In_reads_bytes_opt_(x)
#define VOID void
#define EOK 0
#define PagedPool 0
#define TAG_EXT4 0
#define MAXUSHORT UINT16_MAX
#define ASSERT(x) assert(x)
#define NT_SUCCESS(x) ((x) >= 0)
#define ALIGN_UP_BY(x,a) (((x)+(a)-1)&~((a)-1))
#define FIELD_OFFSET(t,f) offsetof(t,f)
#define RtlCopyMemory(d,s,n) memcpy(d,s,n)
#define RtlCompareMemory(a,b,n) (!memcmp(a,b,n) ? (n) : 0)
#define EXT4_XATTR_USER "user."
#define EXT4_XATTR_USER_LENGTH 5
#define EXT4_EA_ENTRY_SIZE(n,v) ALIGN_UP_BY(offsetof(FILE_FULL_EA_INFORMATION,EaName)+(n)+1+(v),sizeof(ULONG))
typedef uint32_t ULONG, *PULONG;
typedef uint8_t UCHAR, *PUCHAR;
typedef uint16_t USHORT;
typedef char CHAR, *PCHAR;
typedef struct {
    ULONG NextEntryOffset;
    UCHAR Flags, EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;
typedef struct { int Core; } EXT4_VCB, *PEXT4_VCB;
typedef struct { PEXT4_VCB Vcb; ULONG Inode; } EXT4_FCB, *PEXT4_FCB;
static unsigned Allocations, AllocationCalls, FailAllocation, Checks;
static int Locked, Kind, FailList;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static void *Allocate(size_t Size) {
    if (++AllocationCalls == FailAllocation) return NULL;
    void *Block=calloc(1,Size); CHECK(Block != NULL); ++Allocations; return Block;
}
#define ExAllocatePoolUninitialized(p,n,t) Allocate(n)
#define ExAllocatePoolZero(p,n,t) Allocate(n)
static void ExFreePoolWithTag(void *Block,int Tag) {
    (void)Tag; CHECK(Block && Allocations); --Allocations; free(Block);
}
static void Ext4LockCore(PEXT4_VCB Vcb) { (void)Vcb; CHECK(!Locked); Locked=1; }
static void Ext4UnlockCore(PEXT4_VCB Vcb) { (void)Vcb; CHECK(Locked); Locked=0; }
static NTSTATUS Ext4Status(PEXT4_VCB Vcb,int Error) {
    (void)Vcb; return Error == EOK ? STATUS_SUCCESS : Error == ENOMEM ? STATUS_INSUFFICIENT_RESOURCES : STATUS_IO_DEVICE_ERROR;
}
static int E4XattrList(int *Core,ULONG Inode,char *Buffer,uint32_t Size,uint32_t *Length) {
    (void)Core; (void)Inode; CHECK(Locked);
    static const char Names[]="user.note\0security.label\0user.empty\0user.large";
    static const char Other[]="security.label";
    const char *Data=Kind==2 ? Other : Names;
    *Length=Kind==0 ? 0 : Kind==2 ? sizeof(Other) : sizeof(Names);
    if (FailList) return EIO;
    if (Buffer) { CHECK(Size>=*Length); memcpy(Buffer,Data,*Length); }
    return EOK;
}
static int E4XattrGet(int *Core,ULONG Inode,const char *Name,ULONG NameLength,void *Value,ULONG Size,uint32_t *Length) {
    (void)Core; (void)Inode; (void)NameLength; CHECK(Locked);
    *Length=!strcmp(Name,"user.note") ? 3 : !strcmp(Name,"user.large") ? 65536 : 0;
    if (Value) { CHECK(Size>=*Length); if (*Length) memcpy(Value,"abc",*Length); }
    return EOK;
}
'''
    for name, result in (('Ext4AppendEa', 'PFILE_FULL_EA_INFORMATION'),
                         ('Ext4CollectEas', 'NTSTATUS'), ('Ext4EaSize', 'ULONG')):
        harness += function(source, name, result)
    harness += r'''
int main(void) {
    EXT4_VCB Volume={0}; EXT4_FCB File={&Volume,1}; PUCHAR List; ULONG Length;
    CHECK(Ext4CollectEas(&File,&List,&Length)==STATUS_SUCCESS);
    CHECK(Length==0); ExFreePoolWithTag(List,0); CHECK(Ext4EaSize(&File)==0);
    Kind=2; CHECK(Ext4EaSize(&File)==0); CHECK(Allocations==0 && !Locked);
    Kind=1; CHECK(Ext4CollectEas(&File,&List,&Length)==STATUS_SUCCESS);
    PFILE_FULL_EA_INFORMATION First=(PFILE_FULL_EA_INFORMATION)List;
    CHECK(!strcmp(First->EaName,"note") && First->EaValueLength==3);
    CHECK(!memcmp(First->EaName+5,"abc",3)); CHECK(First->NextEntryOffset>0);
    PFILE_FULL_EA_INFORMATION Last=(PFILE_FULL_EA_INFORMATION)(List+First->NextEntryOffset);
    CHECK(!strcmp(Last->EaName,"empty") && Last->EaValueLength==0 && Last->NextEntryOffset==0);
    CHECK(Length==EXT4_EA_ENTRY_SIZE(4,3)+EXT4_EA_ENTRY_SIZE(5,0)); ExFreePoolWithTag(List,0);
    CHECK(Ext4EaSize(&File)==sizeof(ULONG)+5+4+3+5+5);
    for (unsigned Fail=1;Fail<=2;Fail++) {
        AllocationCalls=0; FailAllocation=Fail;
        CHECK(Ext4CollectEas(&File,&List,&Length)==STATUS_INSUFFICIENT_RESOURCES);
        CHECK(List==NULL && Length==0 && !Locked && Allocations==0);
    }
    FailAllocation=0; FailList=1;
    CHECK(Ext4CollectEas(&File,&List,&Length)==STATUS_IO_DEVICE_ERROR);
    CHECK(List==NULL && Length==0 && !Locked && Allocations==0);
    printf("ext4 actual EA collection: %u checks passed, empty lists and failure cleanup covered\n",Checks);
    return 0;
}
'''
    directory = Path(tempfile.gettempdir()) / 'reliefwin-brand-host'
    directory.mkdir(exist_ok=True)
    c_file, binary = directory / 'ext4-ea.c', directory / 'ext4-ea'
    c_file.write_text(harness)
    subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-g',
                    '-fsanitize=address,undefined', '-I', str(ROOT / 'sdk/include/psdk'),
                    str(c_file), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--baseline', help='confirm the retired runtime existed at this revision')
    parser.add_argument('--ext4-eas', action='store_true', help='exercise the actual EA collection functions')
    parser.add_argument('--build-root', type=Path, action='append', default=[])
    args = parser.parse_args()
    if args.ext4_eas:
        check_ext4_eas()
    else:
        check_source(args.baseline)
    for build in args.build_root:
        check_build(build)


if __name__ == '__main__':
    main()
