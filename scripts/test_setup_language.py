#!/usr/bin/env python3
# PROJECT:     ReliefWin setup tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Exercise actual setup language precedence with deterministic hive and firmware inputs
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'base/setup/reliefwin/reliefwin.c'


def function(source, name, declaration):
    start = re.search(r'^' + re.escape(name) + r'\(', source, re.MULTILINE).start()
    cursor = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[cursor] == '{') - (source[cursor] == '}')
        cursor += 1
    return declaration + ' ' + source[start:cursor] + '\n'


PREFIX = r'''
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <string>
using WCHAR = wchar_t;
using PCWSTR = const WCHAR *;
using PWSTR = WCHAR *;
using LCID = uint32_t;
using LANGID = uint16_t;
using DWORD = uint32_t;
using ULONG = uint32_t;
using SIZE_T = size_t;
using BOOL = bool;
using VOID = void;
using HRESULT = int32_t;
#define _In_
#define _Inout_
#define MAX_PATH 260
#define UNICODE_NULL 0
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define SUCCEEDED(result) ((result)>=0)
#define KLF_ACTIVATE 1
#define KLF_SETFORPROCESS 256
#define DPRINT1(...) ((void)0)
struct LIST_ENTRY { LIST_ENTRY *Flink, *Blink; };
using PLIST_ENTRY = LIST_ENTRY *;
struct VOLENTRY { LIST_ENTRY ListEntry; struct { PCWSTR DeviceName; } Info; };
using PVOLENTRY = VOLENTRY *;
#define CONTAINING_RECORD(pointer,type,field) ((type *)((char *)(pointer)-offsetof(type,field)))
struct PARTITIONS { LIST_ENTRY VolumesList; };
struct GENERIC_LIST { bool Languages; };
using PGENERIC_LIST = GENERIC_LIST *;
struct SETUPDATA {
    PARTITIONS *PartitionList;
    WCHAR DefaultLanguage[20], DefaultKBLayout[20];
    PCWSTR SelectedLanguageId;
    struct { WCHAR LocaleID[20]; LANGID LanguageId; PGENERIC_LIST LanguageList, LayoutList; } USetupData;
};
using PSETUPDATA = SETUPDATA *;
struct INSTALLED { PCWSTR Device, Locale, Layout; bool Readable; };
static INSTALLED Installed[3];
static unsigned InstalledCount, Reads, FirmwareCalls, KeyboardFallbacks, Loaded, Checks, Failures;
static LCID Firmware;
static const char *Case;
#define EXPECT(condition) do { ++Checks; if(!(condition)) { std::cerr<<Case<<":"<<__LINE__<<": "<<#condition<<"\n"; ++Failures; } } while(0)
static HRESULT StringCchCopyW(PWSTR output,SIZE_T count,PCWSTR input)
{
    SIZE_T length=std::wcslen(input);
    if(length>=count) return -1;
    std::wmemcpy(output,input,length+1);
    return 0;
}
static HRESULT StringCchPrintfW(PWSTR output,SIZE_T count,PCWSTR format,...)
{
    va_list args;
    va_start(args,format);
    int written;
    if(std::wcscmp(format,L"%08lX")==0)
        written=std::swprintf(output,count,L"%08X",va_arg(args,LCID));
    else
    {
        EXPECT(std::wcscmp(format,L"\\\\?\\GLOBALROOT%s\\Windows\\System32\\config\\DEFAULT")==0);
        written=std::swprintf(output,count,L"\\\\?\\GLOBALROOT%ls\\Windows\\System32\\config\\DEFAULT",va_arg(args,PCWSTR));
    }
    va_end(args);
    return written<0?-1:0;
}
static BOOL ReadInstalledLanguage(PCWSTR path,PWSTR locale,DWORD localeCount,PWSTR layout,DWORD layoutCount)
{
    ++Reads;
    for(unsigned index=0;index<InstalledCount;++index)
        if(std::wcsstr(path,Installed[index].Device))
        {
            if(!Installed[index].Readable) return false;
            EXPECT(SUCCEEDED(StringCchCopyW(locale,localeCount,Installed[index].Locale)));
            EXPECT(SUCCEEDED(StringCchCopyW(layout,layoutCount,Installed[index].Layout)));
            return true;
        }
    EXPECT(false);
    return false;
}
static LCID GetFirmwareLanguage() { ++FirmwareCalls; return Firmware; }
static BOOL SelectListEntryById(PGENERIC_LIST list,PCWSTR id)
{
    EXPECT(list);
    return std::wcscmp(id,L"00000409")==0 || std::wcscmp(id,L"00000804")==0 ||
           std::wcscmp(id,L"0000040C")==0 || std::wcscmp(id,L"00000411")==0;
}
static VOID GetLocaleKeyboardLayout(PSETUPDATA setup,LCID locale,PWSTR layout,SIZE_T count)
{
    ++KeyboardFallbacks;
    EXPECT(setup && locale);
    EXPECT(SUCCEEDED(StringCchPrintfW(layout,count,L"%08lX",locale==0x804?LCID(0x409):locale)));
}
static BOOL LoadKeyboardLayoutW(PCWSTR layout,DWORD flags)
{
    ++Loaded;
    EXPECT(layout && *layout && flags==(KLF_ACTIVATE|KLF_SETFORPROCESS));
    return true;
}
static VOID ApplyLanguageChoice(PSETUPDATA,PCWSTR,PCWSTR);
'''


TESTS = r'''
static void run_case(const char *name,PCWSTR initialLocale,PCWSTR initialLayout,LCID firmware,
                     PCWSTR expectedLocale,PCWSTR expectedLayout,unsigned expectedReads,
                     unsigned expectedFirmware,unsigned expectedFallbacks,bool applied=true)
{
    GENERIC_LIST languages={true}, layouts={false};
    PARTITIONS partitions={};
    VOLENTRY volumes[3]={};
    SETUPDATA setup={};
    Case=name;
    Reads=FirmwareCalls=KeyboardFallbacks=Loaded=0;
    Firmware=firmware;
    LIST_ENTRY *tail=&partitions.VolumesList;
    for(unsigned index=0;index<InstalledCount;++index)
    {
        volumes[index].Info.DeviceName=Installed[index].Device;
        tail->Flink=&volumes[index].ListEntry;
        volumes[index].ListEntry.Blink=tail;
        tail=&volumes[index].ListEntry;
    }
    tail->Flink=&partitions.VolumesList;
    partitions.VolumesList.Blink=tail;
    setup.PartitionList=&partitions;
    setup.USetupData.LanguageList=&languages;
    setup.USetupData.LayoutList=&layouts;
    EXPECT(SUCCEEDED(StringCchCopyW(setup.DefaultLanguage,ARRAYSIZE(setup.DefaultLanguage),initialLocale)));
    EXPECT(SUCCEEDED(StringCchCopyW(setup.DefaultKBLayout,ARRAYSIZE(setup.DefaultKBLayout),initialLayout)));
    DetectLanguageDefaults(&setup);
    EXPECT(std::wcscmp(setup.DefaultLanguage,expectedLocale)==0);
    EXPECT(std::wcscmp(setup.DefaultKBLayout,expectedLayout)==0);
    EXPECT(Reads==expectedReads && FirmwareCalls==expectedFirmware && KeyboardFallbacks==expectedFallbacks);
    EXPECT(Loaded==(applied?1u:0u));
    if(applied)
    {
        EXPECT(std::wcscmp(setup.USetupData.LocaleID,expectedLocale)==0);
        EXPECT(setup.SelectedLanguageId==setup.DefaultLanguage);
        EXPECT(setup.USetupData.LanguageId==(std::wcstoul(expectedLocale,nullptr,16)&0xffff));
    }
}
int main()
{
    InstalledCount=0;
    run_case("Chinese user locale survives English firmware",L"00000804",L"00000409",0x409,L"00000804",L"00000409",0,0,0);
    run_case("other user locale also survives firmware",L"0000040C",L"0000040C",0x409,L"0000040C",L"0000040C",0,0,0);
    run_case("missing keyboard uses locale keyboard",L"00000804",L"",0x409,L"00000804",L"00000409",0,0,1);
    run_case("unsupported keyboard uses locale keyboard",L"00000804",L"unsupported",0x409,L"00000804",L"00000409",0,0,1);
    run_case("zero locale falls back to firmware",L"00000000",L"00000411",0x409,L"00000409",L"00000409",0,1,1);
    run_case("empty locale falls back to firmware",L"",L"00000411",0x804,L"00000804",L"00000409",0,1,1);
    run_case("no locale available retains current state",L"00000000",L"00000411",0,L"00000000",L"00000411",0,1,0,false);
    Installed[0]={L"\\Device\\HarddiskVolume1",L"0000040C",L"0000040C",false};
    Installed[1]={L"\\Device\\HarddiskVolume2",L"00000411",L"00000411",false};
    InstalledCount=2;
    run_case("unreadable installed hives retain Chinese default",L"00000804",L"00000409",0x409,L"00000804",L"00000409",2,0,0);
    Installed[0].Readable=true;
    run_case("installed user choice takes priority",L"00000804",L"00000409",0x409,L"0000040C",L"0000040C",1,0,0);
    Installed[0].Locale=L"00000000";
    Installed[1].Readable=true;
    run_case("later installed valid locale takes priority",L"00000804",L"00000409",0x409,L"00000411",L"00000411",2,0,0);
    Installed[0].Locale=L"00000804"; Installed[0].Layout=L"";
    run_case("installed locale with no keyboard uses locale keyboard",L"0000040C",L"0000040C",0x409,L"00000804",L"00000409",1,0,1);
    std::cout<<"Setup language precedence: 11 cases, "<<Checks<<" checks, "<<Failures<<" failures\n";
    return Failures?1:0;
}
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default='c++')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--verify-regression', action='store_true')
    args = parser.parse_args()
    source = SOURCE.read_text()
    detect = function(source, 'DetectLanguageDefaults', 'static VOID')
    apply = function(source, 'ApplyLanguageChoice', 'static VOID')
    prior = '''    if (Lcid == 0)
    {
        Lcid = wcstoul(pSetupData->DefaultLanguage, NULL, 16);
        StringCchCopyW(Layout, ARRAYSIZE(Layout), pSetupData->DefaultKBLayout);
    }
'''
    variants = [('current', detect)]
    if args.verify_regression:
        if detect.count(prior) != 1:
            raise RuntimeError('Cannot isolate original firmware precedence defect')
        variants.append(('original precedence', detect.replace(prior, '', 1)))
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        for name, body in variants:
            (path / 'test.cpp').write_text(PREFIX + apply + body + TESTS)
            command = [args.cxx, '-std=c++17', '-Wall', '-Wextra', '-Werror', str(path / 'test.cpp'), '-o', str(path / 'test')]
            if args.sanitize:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
            subprocess.run(command, check=True)
            result = subprocess.run([str(path / 'test')])
            if name == 'current' and result.returncode:
                return result.returncode
            if name != 'current':
                if result.returncode != 1:
                    raise RuntimeError('Original precedence did not fail the behavioral regression')
                print('Original firmware precedence failed as expected')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
