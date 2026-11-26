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
OOBE = ROOT / 'base/setup/reliefwin/oobe.c'


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
using LPWSTR = PWSTR;
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
    struct {
        WCHAR LocaleID[20]; LANGID LanguageId;
        PGENERIC_LIST LanguageList, LayoutList, ComputerList, DisplayList, KeyboardList;
        void *SetupInf;
    } USetupData;
    BOOL bUnattend,AutoDaylight;
    WCHAR ComputerName[32];
    ULONG TimeZoneIndex;
};
using PSETUPDATA = SETUPDATA *;
struct INSTALLED { PCWSTR Device, Locale, Layout; bool Readable; };
static INSTALLED Installed[3];
static unsigned InstalledCount, Reads, FirmwareCalls, KeyboardFallbacks, Loaded, Checks, Failures;
static LCID Firmware;
static const char *Case;
#define EXPECT(condition) do { ++Checks; if(!(condition)) { if(Failures<8) std::cerr<<Case<<":"<<__LINE__<<": "<<#condition<<"\n"; ++Failures; } } while(0)
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
    if(std::wcscmp(format,L"%08lX")==0 || std::wcscmp(format,L"%08lx")==0)
        written=std::swprintf(output,count,std::wcscmp(format,L"%08lx")==0?L"%08x":L"%08X",va_arg(args,LCID));
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
    if(!list) return false;
    if(!list->Languages)
    {
        if(std::wcslen(id)!=8) return false;
        wchar_t *end;
        unsigned long value=std::wcstoul(id,&end,16);
        if(*end) return false;
        for(auto keyboard:AvailableKeyboardLayouts)
            if(value==keyboard) return true;
        return false;
    }
    return std::wcscmp(id,L"00000409")==0 || std::wcscmp(id,L"00000804")==0 ||
           std::wcscmp(id,L"00000411")==0;
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
#define TRUE true
#define FALSE false
#define LANG_ENGLISH 0x09
#define LANG_JAPANESE 0x11
#define LANG_CHINESE 0x04
#define SUBLANG_ENGLISH_US 0x01
#define SUBLANG_DEFAULT 0x01
#define SUBLANG_CHINESE_SIMPLIFIED 0x02
#define SORT_DEFAULT 0
#define LOCALE_SLOCALIZEDDISPLAYNAME 0x00000002
#define MAKELANGID(primary,sub) ((LANGID)(((sub)<<10)|(primary)))
#define MAKELCID(language,sort) ((LCID)(((sort)<<16)|(language)))
struct CHOICE_LIST { unsigned Count; };
static CHOICE_LIST LocaleList;
static unsigned LocaleInfoCalls, ChoiceCalls, RegionCalls;
static unsigned LocaleFailure;
static LANGID UserUILanguage;
static LCID RegionLocale;
static WCHAR ChoiceId[20], ChoiceName[128];
static LANGID GetUserDefaultUILanguage() { return UserUILanguage; }
[[maybe_unused]] static LCID GetUserDefaultLCID() { ++RegionCalls; return RegionLocale; }
static int GetLocaleInfoW(LCID locale,DWORD type,PWSTR name,int count)
{
    ++LocaleInfoCalls;
    EXPECT(locale && type==LOCALE_SLOCALIZEDDISPLAYNAME);
    EXPECT(count>=13);
    if(LocaleFailure==1) return 0;
    std::wmemcpy(name,L"Display name",13);
    return 13;
}
static BOOL AddChoice(CHOICE_LIST *list,PCWSTR id,PCWSTR name)
{
    ++ChoiceCalls;
    EXPECT(list==&LocaleList);
    EXPECT(SUCCEEDED(StringCchCopyW(ChoiceId,ARRAYSIZE(ChoiceId),id)));
    EXPECT(SUCCEEDED(StringCchCopyW(ChoiceName,ARRAYSIZE(ChoiceName),name)));
    if(LocaleFailure==2) return false;
    ++list->Count;
    return true;
}
static unsigned InitFailure,UnattendedCalls,InitCases;
static bool KeyboardNameFailure;
static PCWSTR CurrentKeyboard=L"00000409";
static GENERIC_LIST SupportedLanguages={true},SupportedKeyboards={false},OtherList={false};
static PARTITIONS InitPartitions;
static PARTITIONS *CreatePartitionList()
{
    if(InitFailure==1) return nullptr;
    InitPartitions.VolumesList.Flink=InitPartitions.VolumesList.Blink=&InitPartitions.VolumesList;
    return &InitPartitions;
}
static PGENERIC_LIST CreateComputerTypeList(void *) { return &OtherList; }
static PGENERIC_LIST CreateDisplayDriverList(void *) { return &OtherList; }
static PGENERIC_LIST CreateKeyboardDriverList(void *) { return &OtherList; }
static PGENERIC_LIST CreateLanguageList(void *,PWSTR language)
{
    if(InitFailure==2) return nullptr;
    EXPECT(SUCCEEDED(StringCchCopyW(language,20,InitFailure==3?L"0000040C":L"00000804")));
    return &SupportedLanguages;
}
static PGENERIC_LIST CreateKeyboardLayoutList(void *,PCWSTR language,PWSTR keyboard)
{
    EXPECT(SelectListEntryById(&SupportedLanguages,language));
    EXPECT(SUCCEEDED(StringCchCopyW(keyboard,20,L"00000409")));
    return InitFailure==4?nullptr:&SupportedKeyboards;
}
static BOOL GetKeyboardLayoutNameW(PWSTR layout)
{
    EXPECT(SUCCEEDED(StringCchCopyW(layout,20,CurrentKeyboard)));
    return !KeyboardNameFailure;
}
static void GenerateComputerName(PWSTR name)
{
    EXPECT(SUCCEEDED(StringCchCopyW(name,32,L"ReliefWin")));
}
static void GetTimeZoneListIndex(ULONG *index) { *index=42; }
static void LoadUnattendedAccountSettings(PSETUPDATA setup)
{
    EXPECT(setup->bUnattend);
    ++UnattendedCalls;
}
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
    EXPECT(SUCCEEDED(StringCchCopyW(setup.USetupData.LocaleID,ARRAYSIZE(setup.USetupData.LocaleID),initialLocale)));
    setup.SelectedLanguageId=setup.DefaultLanguage;
    setup.USetupData.LanguageId=std::wcstoul(initialLocale,nullptr,16)&0xffff;
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
static void run_oobe_tests()
{
    Case="OOBE supported UI languages and regional independence";
    for(unsigned language=0;language<0x10000;++language)
    {
        bool supported=language==0x409 || language==0x411 || language==0x804;
        UserUILanguage=language;
        for(LCID region:{LCID(0x409),LCID(0x40c),LCID(0x411),LCID(0x804),LCID(0x404),LCID(0x10407)})
        {
            RegionLocale=region;
            EXPECT(GetInitialSetupLanguage()==(supported?language:0x804));
            EXPECT(RegionCalls==0);
        }
        WCHAR locale[20];
        EXPECT(std::swprintf(locale,ARRAYSIZE(locale),L"%08X",language)>0);
        LocaleList.Count=LocaleInfoCalls=ChoiceCalls=0;
        EXPECT(AddLocaleChoice(locale));
        EXPECT(LocaleInfoCalls==(supported?1u:0u));
        EXPECT(ChoiceCalls==(supported?1u:0u));
        EXPECT(LocaleList.Count==(supported?1u:0u));
        if(supported)
        {
            EXPECT(std::wcscmp(ChoiceId,locale)==0);
            EXPECT(std::wcscmp(ChoiceName,L"Display name")==0);
        }
    }
    for(LCID locale:{LCID(0x10409),LCID(0x20411),LCID(0x10804),LCID(0xffffffff),LCID(0x10000409)})
    {
        WCHAR text[20];
        EXPECT(std::swprintf(text,ARRAYSIZE(text),L"%08X",locale)>0);
        LocaleList.Count=LocaleInfoCalls=ChoiceCalls=0;
        EXPECT(AddLocaleChoice(text));
        EXPECT(LocaleList.Count==0 && LocaleInfoCalls==0 && ChoiceCalls==0);
    }
    for(unsigned failure:{1u,2u})
        for(LCID locale:{LCID(0x409),LCID(0x411),LCID(0x804)})
        {
            WCHAR text[20];
            EXPECT(std::swprintf(text,ARRAYSIZE(text),L"%08X",locale)>0);
            LocaleFailure=failure;
            LocaleList.Count=LocaleInfoCalls=ChoiceCalls=0;
            EXPECT(AddLocaleChoice(text));
            EXPECT(LocaleInfoCalls==1 && LocaleList.Count==0);
            EXPECT(ChoiceCalls==(failure==2?1u:0u));
        }
    LocaleFailure=0;
}
static void initialize(bool unattended,LANGID ui,LCID region,PCWSTR candidate,PCWSTR keyboard,
                       PCWSTR expectedUI,PCWSTR expectedKeyboard,unsigned failure=0,bool keyboardFailure=false)
{
    Case="setup initialization keeps UI selection and keyboard choice independent";
    ++InitCases;
    SETUPDATA setup={};
    setup.bUnattend=unattended;
    setup.USetupData.SetupInf=&setup;
    EXPECT(SUCCEEDED(StringCchCopyW(setup.USetupData.LocaleID,20,candidate)));
    UserUILanguage=ui;
    RegionLocale=region;
    CurrentKeyboard=keyboard;
    InitFailure=failure;
    KeyboardNameFailure=keyboardFailure;
    InstalledCount=0;
    Reads=FirmwareCalls=KeyboardFallbacks=Loaded=RegionCalls=UnattendedCalls=0;
    bool loaded=LoadSetupData(&setup);
    EXPECT(loaded==(failure==0));
    EXPECT(RegionCalls==0);
    if(!loaded) return;
    EXPECT(std::wcscmp(setup.DefaultLanguage,expectedUI)==0);
    EXPECT(std::wcscmp(setup.USetupData.LocaleID,expectedUI)==0);
    EXPECT(setup.SelectedLanguageId==setup.DefaultLanguage);
    EXPECT(setup.USetupData.LanguageId==(std::wcstoul(expectedUI,nullptr,16)&0xffff));
    EXPECT(std::wcscmp(setup.DefaultKBLayout,expectedKeyboard)==0);
    EXPECT(SelectListEntryById(setup.USetupData.LanguageList,setup.DefaultLanguage));
    EXPECT(SelectListEntryById(setup.USetupData.LayoutList,setup.DefaultKBLayout));
    EXPECT(std::wcscmp(setup.ComputerName,L"ReliefWin")==0);
    EXPECT(setup.TimeZoneIndex==42 && setup.AutoDaylight);
    EXPECT(UnattendedCalls==(unattended?1u:0u));
    EXPECT(Loaded==(unattended?0u:1u));
}
static void run_initialization_tests()
{
    for(LANGID ui:{LANGID(0x409),LANGID(0x411),LANGID(0x804),LANGID(0x40c),LANGID(0x809),LANGID(0x404),LANGID(0x9999)})
        for(LCID region:{LCID(0x409),LCID(0x40c),LCID(0x804),LCID(0x10407)})
            for(PCWSTR keyboard:{L"00000409",L"0000040C",L"00000401",L"00000411",L"00000804",L"unsupported"})
            {
                WCHAR expectedUI[20];
                EXPECT(std::swprintf(expectedUI,ARRAYSIZE(expectedUI),L"%08X",ui==0x409 || ui==0x411 || ui==0x804?ui:0x804)>0);
                initialize(false,ui,region,L"0000040C",keyboard,expectedUI,
                           SelectListEntryById(&SupportedKeyboards,keyboard)?keyboard:L"00000409");
            }
    for(PCWSTR candidate:{L"00000409",L"00000411",L"00000804",L"0000040C",L"00000401",L"00000404",L"00009999"})
        for(LANGID ui:{LANGID(0x409),LANGID(0x411),LANGID(0x804)})
        {
            bool supported=std::wcscmp(candidate,L"00000409")==0 || std::wcscmp(candidate,L"00000411")==0 || std::wcscmp(candidate,L"00000804")==0;
            PCWSTR layout=std::wcscmp(candidate,L"00000804")==0 || std::wcscmp(candidate,L"00009999")==0?L"00000409":candidate;
            if(!SelectListEntryById(&SupportedKeyboards,layout)) layout=L"00000409";
            initialize(true,ui,0x40c,candidate,L"unsupported",supported?candidate:L"00000804",layout);
        }
    initialize(false,0x804,0x40c,L"0000040C",L"0000040C",L"00000804",L"00000409",0,true);
    for(unsigned failure=1;failure<=4;++failure)
        initialize(false,0x411,0x40c,L"0000040C",L"0000040C",L"00000411",L"0000040C",failure);
    InitFailure=0;
    KeyboardNameFailure=false;
}
int main()
{
    InstalledCount=0;
    run_case("Chinese user locale survives English firmware",L"00000804",L"00000409",0x409,L"00000804",L"00000409",0,0,0);
    run_case("Japanese user locale survives firmware",L"00000411",L"00000411",0x409,L"00000411",L"00000411",0,0,0);
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
    Installed[0].Locale=L"00000411";
    run_case("installed user choice takes priority",L"00000804",L"00000409",0x409,L"00000411",L"0000040C",1,0,0);
    Installed[0].Locale=L"00000000";
    Installed[1].Readable=true;
    run_case("later installed valid locale takes priority",L"00000804",L"00000409",0x409,L"00000411",L"00000411",2,0,0);
    Installed[0].Locale=L"00000804"; Installed[0].Layout=L"";
    run_case("installed locale with no keyboard uses locale keyboard",L"00000411",L"00000411",0x409,L"00000804",L"00000409",1,0,1);
    InstalledCount=1;
    Installed[0]={L"\\Device\\HarddiskVolume1",L"0000040C",L"0000040C",true};
    run_case("unsupported installed UI retains Chinese and French keyboard",L"00000804",L"00000409",0x409,L"00000804",L"0000040C",1,0,0);
    run_oobe_tests();
    run_initialization_tests();
    std::cout<<"Setup and OOBE language selection: 12 precedence cases, "<<InitCases<<" initialization cases, all 65536 LANGIDs, 6 region settings, "<<Checks<<" checks, "<<Failures<<" failures\n";
    return Failures?1:0;
}
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default='c++')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--verify-regression', action='store_true')
    parser.add_argument('--verify-ui-regression', metavar='REVISION')
    parser.add_argument('--verify-initialization-regression', metavar='REVISION')
    args = parser.parse_args()
    source = SOURCE.read_text()
    detect = function(source, 'DetectLanguageDefaults', 'static VOID')
    apply = function(source, 'ApplyLanguageChoice', 'static VOID')
    load = function(source, 'LoadSetupData', 'static BOOL')
    oobe = OOBE.read_text()
    initial = function(oobe, 'GetInitialSetupLanguage', 'static LANGID')
    choices = function(oobe, 'AddLocaleChoice', 'static BOOL')
    prior = '''    if (Lcid == 0)
    {
        Lcid = wcstoul(pSetupData->DefaultLanguage, NULL, 16);
        StringCchCopyW(Layout, ARRAYSIZE(Layout), pSetupData->DefaultKBLayout);
    }
'''
    variants = [('current', detect, choices, load)]
    if args.verify_regression:
        if detect.count(prior) != 1:
            raise RuntimeError('Cannot isolate original firmware precedence defect')
        variants.append(('original precedence', detect.replace(prior, '', 1), choices, load))
    if args.verify_ui_regression:
        original = subprocess.check_output(['git', 'show', args.verify_ui_regression + ':base/setup/reliefwin/oobe.c'], cwd=ROOT, text=True)
        variants.append(('original UI choices', detect, function(original, 'AddLocaleChoice', 'static BOOL'), load))
    if args.verify_initialization_regression:
        original = subprocess.check_output(['git', 'show', args.verify_initialization_regression + ':base/setup/reliefwin/reliefwin.c'], cwd=ROOT, text=True)
        variants.append(('original setup initialization', detect, choices, function(original, 'LoadSetupData', 'static BOOL')))
    sif = (ROOT / 'image/data/txtsetup.sif').read_text(encoding='utf-8-sig')
    keyboards = re.search(r'^\[KeyboardLayout\]\s*\n(.*?)(?=^\[|\Z)', sif, re.M | re.S)[1]
    identifiers = re.findall(r'^([0-9a-fA-F]{8})\s*=', keyboards, re.M)
    keyboard_data = 'static const uint32_t AvailableKeyboardLayouts[] = {' + ','.join('0x' + value for value in identifiers) + '};\n'
    prefix = PREFIX.replace('static BOOL SelectListEntryById(', keyboard_data + 'static BOOL SelectListEntryById(', 1)
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        for name, body, locale_choices, initialization in variants:
            (path / 'test.cpp').write_text(prefix + apply + body + initial + locale_choices + initialization + TESTS)
            command = [args.cxx, '-std=c++17', '-Wall', '-Wextra', '-Werror', str(path / 'test.cpp'), '-o', str(path / 'test')]
            if args.sanitize:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
            subprocess.run(command, check=True)
            result = subprocess.run([str(path / 'test')])
            if name == 'current' and result.returncode:
                return result.returncode
            if name != 'current':
                if result.returncode != 1:
                    raise RuntimeError(name + ' did not fail the behavioral regression')
                print(name + ' failed as expected')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
