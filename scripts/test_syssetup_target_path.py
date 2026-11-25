#!/usr/bin/env python3
"""
PROJECT:     ReliefWin System Setup tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Verify target registry paths and temporary hive restore paths
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = "dll/win32/syssetup/target.c"


def extracted(source, name, result):
    match = re.search(r"\n" + name + r"\([^;]+?\n\{", source)
    start = match.start() + 1
    end, depth = source.index("{", start) + 1, 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static " + result + " " + source[start:end] + "\n"


def backup_paths(source, folder):
    harness = r'''
#include <wchar.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define VOID void
#define TRUE 1
#define FALSE 0
#define ERROR_SUCCESS 0
#define MAX_PATH 260
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define INVALID_FILE_ATTRIBUTES UINT32_MAX
#define MOVEFILE_REPLACE_EXISTING 1
#define MOVEFILE_DELAY_UNTIL_REBOOT 4
#define DPRINT1(...) ((void)0)
typedef wchar_t WCHAR;
typedef const WCHAR *PCWSTR;
typedef unsigned ULONG;
typedef int BOOL;
static unsigned Checks, Saves, Moves, Probes, Privileges, FailSave, LogsPresent;
#define CHECK(x) do {++Checks;if(!(x)){fprintf(stderr,"backup line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static int StringCchPrintfW(WCHAR *Out,size_t Count,PCWSTR Format,...)
{
 va_list Args;va_start(Args,Format);int N=vswprintf(Out,Count,Format,Args);va_end(Args);
 CHECK(N>=0&&(size_t)N<Count);return 0;
}
'''
    for name in ("LIVE_CONFIG_PATH", "LIVE_SAVE_SUFFIX"):
        harness += re.search(r"^#define " + name + r" .+$", source, re.M).group() + "\n"
    harness += r'''
static void pSetupEnablePrivilege(PCWSTR Name,BOOL Enable)
{CHECK(!wcscmp(Name,L"SeBackupPrivilege")&&Enable==(Privileges%2==0));Privileges++;}
static unsigned SaveHive(PCWSTR Key,PCWSTR Path)
{
 const WCHAR *Name=Saves%2?L"SECURITY":L"SAM";WCHAR Expected[MAX_PATH],ExpectedKey[MAX_PATH];
 StringCchPrintfW(Expected,ARRAYSIZE(Expected),LIVE_CONFIG_PATH L"%ls.reliefwinsave",Name);
 StringCchPrintfW(ExpectedKey,ARRAYSIZE(ExpectedKey),L"\\Registry\\Machine\\%ls",Name);
 CHECK(!wcscmp(Path,Expected)&&!wcscmp(Key,ExpectedKey));Saves++;return Saves==FailSave?5:0;
}
static BOOL MoveFileExW(PCWSTR Source,PCWSTR Destination,unsigned Flags)
{
 WCHAR Expected[MAX_PATH];
 if(Destination){StringCchPrintfW(Expected,ARRAYSIZE(Expected),L"%ls.reliefwinsave",Destination);CHECK(!wcscmp(Source,Expected)&&Flags==5);}
 else CHECK(wcsstr(Source,L".LOG")&&Flags==4);
 Moves++;return TRUE;
}
static unsigned GetFileAttributesW(PCWSTR Path)
{CHECK(wcsstr(Path,L".LOG")&&!wcsstr(Path,L"save"));Probes++;return LogsPresent?0:INVALID_FILE_ATTRIBUTES;}
'''
    harness += extracted(source, "SaveLiveSecurityHives", "BOOL")
    harness += extracted(source, "ScheduleLiveSecurityRestore", "VOID")
    harness += r'''
int main(void)
{
 CHECK(SaveLiveSecurityHives()&&Saves==2&&Privileges==2);
 LogsPresent=1;ScheduleLiveSecurityRestore();CHECK(Moves==8&&Probes==6);
 Moves=Probes=0;LogsPresent=0;ScheduleLiveSecurityRestore();CHECK(Moves==2&&Probes==6);
 Saves=Privileges=0;FailSave=1;CHECK(!SaveLiveSecurityHives()&&Saves==1&&Privileges==2);
 Saves=Privileges=0;FailSave=2;CHECK(!SaveLiveSecurityHives()&&Saves==2&&Privileges==2);
 printf("Live hive backup paths: %u checks passed against actual save and restore functions\n",Checks);
 return 0;
}
'''
    c_file, binary = folder / "syssetup-hive-backup.c", folder / "syssetup-hive-backup"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline")
    args = parser.parse_args()
    if args.baseline:
        source = subprocess.run(
            ["git", "show", f"{args.baseline}:{SOURCE}"], cwd=ROOT,
            check=True, capture_output=True, text=True).stdout
    else:
        source = (ROOT / SOURCE).read_text()
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    backup_paths(source, folder)
    harness = r'''
#include <wchar.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define _In_
#define _In_opt_
#define _Out_
#define MAX_PATH 260
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FAILED(x) ((x)<0)
#define STATUS_NAME_TOO_LONG ((int32_t)0xC0000106)
#define OBJ_CASE_INSENSITIVE 64
#define MAXIMUM_ALLOWED 0x2000000
typedef int32_t NTSTATUS;
typedef wchar_t WCHAR;
typedef const WCHAR *PCWSTR;
typedef void *HANDLE, **PHANDLE;
typedef struct { PCWSTR Buffer; } UNICODE_STRING;
typedef struct { UNICODE_STRING *Name; } OBJECT_ATTRIBUTES;
static unsigned Checks, Opens;
static NTSTATUS OpenStatus;
static WCHAR Opened[MAX_PATH];
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
static int StringCchCopyW(WCHAR *Dest,size_t Count,PCWSTR Source)
{
 size_t Length=wcslen(Source), Copy=Length<Count?Length:Count-1;
 wmemcpy(Dest,Source,Copy);Dest[Copy]=0;return Length<Count?0:-1;
}
static int StringCchCatW(WCHAR *Dest,size_t Count,PCWSTR Source)
{ size_t Length=wcslen(Dest);return StringCchCopyW(Dest+Length,Count-Length,Source); }
static void RtlInitUnicodeString(UNICODE_STRING *String,PCWSTR Source) { String->Buffer=Source; }
static void InitializeObjectAttributes(OBJECT_ATTRIBUTES *Object,UNICODE_STRING *Name,unsigned Attributes,void *Root,void *Security)
{ CHECK(Attributes==64&&!Root&&!Security);Object->Name=Name; }
static NTSTATUS NtOpenKey(PHANDLE Key,unsigned Access,OBJECT_ATTRIBUTES *Object)
{ CHECK(Access==0x2000000);Opens++;wcscpy(Opened,Object->Name->Buffer);if(!OpenStatus)*Key=(void *)(uintptr_t)123;return OpenStatus; }
'''
    harness += extracted(source, "OpenTargetKey", "NTSTATUS")
    harness += r'''
int main(void)
{
 WCHAR Path[263],Suffix[23],Expected[286];
 for(size_t P=0;P<=261;P++)
 {
  wmemset(Path,L'p',P);Path[P]=0;
  for(int S=-1;S<=21;S++)
  {
   HANDLE Key=(void *)(uintptr_t)456;Opens=0;OpenStatus=0;
   if(S>=0){wmemset(Suffix,L's',S);Suffix[S]=0;}
   size_t Length=P+(S<0?0:(size_t)S+1);
   NTSTATUS Status=OpenTargetKey(Path,S<0?NULL:Suffix,&Key);
   if(Length>=MAX_PATH)
    CHECK(Status==STATUS_NAME_TOO_LONG&&!Opens&&!Key);
   else
   {
    wcscpy(Expected,Path);if(S>=0){wcscat(Expected,L"\\");wcscat(Expected,Suffix);}
    CHECK(!Status&&Opens==1&&Key==(void *)(uintptr_t)123&&!wcscmp(Opened,Expected));
   }
  }
 }
 HANDLE Key=(void *)(uintptr_t)456;Opens=0;OpenStatus=(int32_t)0xC0000022;
 CHECK(OpenTargetKey(L"\\Registry\\Machine\\Target",L"SOFTWARE\\Classes",&Key)==OpenStatus&&Opens==1&&!Key);
 printf("Target registry paths: %u checks passed against actual OpenTargetKey\n",Checks);
 return 0;
}
'''
    c_file, binary = folder / "syssetup-target-path.c", folder / "syssetup-target-path"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    main()
