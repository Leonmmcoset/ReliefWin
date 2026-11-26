#!/usr/bin/env python3
# PROJECT:     ReliefWin setup tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify actual setup keyboard enumeration and registry layout selection
# COPYRIGHT:   Copyright 2026 LeonMMcoset <leonmmcoset@outlook.com>

import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile

from test_setup_language import function


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'base/setup/lib/settings.c'


PREFIX = r'''
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using WCHAR=wchar_t;
using PCWSTR=const WCHAR *;
using PWSTR=WCHAR *;
using KLID=uint32_t;
using LANGID=uint16_t;
using ULONG=uint32_t;
using BOOLEAN=bool;
using BOOL=bool;
#define IN
#define OUT
#define _In_
#define TRUE true
#define FALSE false
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define UNREFERENCED_PARAMETER(p) ((void)(p))
#define LOWORD(value) ((uint16_t)(value))
#define DPRINT1(...) ((void)0)
struct MUI_LAYOUTS { LANGID LangID; KLID LayoutID; };
struct INF_ENTRY { PCWSTR Id; PCWSTR Value; };
struct GENERIC_LIST { std::vector<std::pair<std::wstring,std::wstring>> Entries; };
using PGENERIC_LIST=GENERIC_LIST *;
struct INF_FILE { unsigned Marker; };
using HINF=INF_FILE *;
struct INFCONTEXT { unsigned Marker; };
static INF_FILE InfFile={0x1284};
static unsigned Checks,Failures,DestroyCalls,EnumerationCalls,CreateCalls,RegistryCalls,MUIReads;
static unsigned InfFailure;
static bool RegistryFailure;
static const char *Case;
static const MUI_LAYOUTS *OverrideLayouts;
static std::vector<MUI_LAYOUTS> Captured;
#define EXPECT(condition) do { ++Checks; if(!(condition)) { if(Failures<8) std::cerr<<Case<<":"<<__LINE__<<": "<<#condition<<"\n"; ++Failures; } } while(0)
'''


STUBS = r'''
static bool SpInfFindFirstLine(HINF file,PCWSTR section,PCWSTR key,INFCONTEXT *context)
{
    EXPECT(file==&InfFile && std::wcscmp(section,L"NLS")==0 && std::wcscmp(key,L"DefaultLayout")==0);
    context->Marker=0x6712;
    return InfFailure!=1;
}
static bool INF_GetData(INFCONTEXT *context,PCWSTR *,PCWSTR *value)
{
    EXPECT(context->Marker==0x6712);
    *value=RealDefaultLayout;
    return InfFailure!=2;
}
static PGENERIC_LIST CreateGenericList()
{
    ++CreateCalls;
    return InfFailure==3?nullptr:new GENERIC_LIST;
}
static void DestroyGenericList(PGENERIC_LIST list,BOOL freeData)
{
    ++DestroyCalls;
    EXPECT(list && freeData);
    delete list;
}
static size_t GetNumberOfListEntries(PGENERIC_LIST list) { return list->Entries.size(); }
static void DefaultProcessEntry() {}
static int AddEntriesFromInfSection(PGENERIC_LIST list,HINF file,PCWSTR section,INFCONTEXT *context,
                                   void (*process)(),PWSTR defaultLayout)
{
    ++EnumerationCalls;
    EXPECT(file==&InfFile && std::wcscmp(section,L"KeyboardLayout")==0);
    EXPECT(context->Marker==0x6712 && process==DefaultProcessEntry);
    EXPECT(std::wcscmp(defaultLayout,RealDefaultLayout)==0);
    if(InfFailure==5) return 0;
    for(const auto &entry:RealLayouts)
    {
        list->Entries.emplace_back(entry.Id,entry.Value);
        if(InfFailure==4) return -1;
    }
    return static_cast<int>(ARRAYSIZE(RealLayouts));
}
static const MUI_LAYOUTS *MUIGetLayoutsList(PCWSTR language)
{
    ++MUIReads;
    if(OverrideLayouts) return OverrideLayouts;
    if(std::wcscmp(language,L"00000409")==0) return enUSLayouts;
    if(std::wcscmp(language,L"00000411")==0) return jaJPLayouts;
    EXPECT(std::wcscmp(language,L"00000804")==0);
    return zhCNLayouts;
}
static BOOLEAN AddKbLayoutsToRegistry(const MUI_LAYOUTS *layouts)
{
    ++RegistryCalls;
    Captured.clear();
    unsigned index;
    for(index=0;index<20 && layouts[index].LangID;++index)
        Captured.push_back(layouts[index]);
    EXPECT(index<20);
    if(index<20) EXPECT(layouts[index].LangID==0 && layouts[index].LayoutID==0);
    return !RegistryFailure;
}
'''


TESTS = r'''
static std::vector<MUI_LAYOUTS> snapshot(const MUI_LAYOUTS *layouts)
{
    std::vector<MUI_LAYOUTS> result;
    for(unsigned index=0;layouts[index].LangID;++index) result.push_back(layouts[index]);
    return result;
}
static void expect_equal(const std::vector<MUI_LAYOUTS> &left,const std::vector<MUI_LAYOUTS> &right)
{
    EXPECT(left.size()==right.size());
    for(size_t index=0;index<left.size() && index<right.size();++index)
        EXPECT(left[index].LangID==right[index].LangID && left[index].LayoutID==right[index].LayoutID);
}
static void enumerate(PCWSTR language,unsigned failure=0)
{
    Case="complete distinct keyboard choices independently of UI language";
    InfFailure=failure;
    DestroyCalls=EnumerationCalls=CreateCalls=0;
    WCHAR defaultLayout[32]={};
    PGENERIC_LIST list=CreateKeyboardLayoutList(&InfFile,language,defaultLayout);
    if(failure)
    {
        EXPECT(list==nullptr);
        EXPECT(DestroyCalls==(failure==4 || failure==5?1u:0u));
        EXPECT(CreateCalls==(failure>=3?1u:0u));
        EXPECT(EnumerationCalls==(failure>=4?1u:0u));
        if(list) DestroyGenericList(list,true);
        return;
    }
    EXPECT(list);
    if(!list) return;
    EXPECT(EnumerationCalls==1 && CreateCalls==1 && DestroyCalls==0);
    EXPECT(std::wcscmp(defaultLayout,RealDefaultLayout)==0);
    EXPECT(list->Entries.size()==ARRAYSIZE(RealLayouts));
    std::set<std::wstring> unique;
    for(size_t index=0;index<list->Entries.size();++index)
    {
        EXPECT(unique.insert(list->Entries[index].first).second);
        if(index<ARRAYSIZE(RealLayouts))
            EXPECT(list->Entries[index].first==RealLayouts[index].Id && list->Entries[index].second==RealLayouts[index].Value);
    }
    EXPECT(unique.count(L"0000040c")==1 || unique.count(L"0000040C")==1);
    EXPECT(unique.count(L"00000401")==1);
    EXPECT(unique.count(L"00000411")==1);
    DestroyGenericList(list,true);
    EXPECT(DestroyCalls==1);
}
static void select(PCWSTR language,PCWSTR choice)
{
    Case="selected keyboard layout persisted first with all unaffected defaults";
    const auto before=snapshot(MUIGetLayoutsList(language));
    const KLID selected=std::wcstoul(choice,nullptr,16);
    RegistryCalls=0;
    Captured.clear();
    EXPECT(ProcessKeyboardLayoutRegistry(choice,language));
    EXPECT(RegistryCalls==1);
    MUI_LAYOUTS selectedProfile={LOWORD(selected),selected};
    for(const auto &entry:before)
        if(entry.LayoutID==selected) { selectedProfile=entry; break; }
    std::vector<MUI_LAYOUTS> expected={selectedProfile};
    std::set<std::pair<LANGID,KLID>> unique={{selectedProfile.LangID,selectedProfile.LayoutID}};
    for(const auto &entry:before)
        if(unique.insert({entry.LangID,entry.LayoutID}).second) expected.push_back(entry);
    expect_equal(Captured,expected);
    expect_equal(snapshot(MUIGetLayoutsList(language)),before);
    RegistryFailure=true;
    RegistryCalls=0;
    EXPECT(!ProcessKeyboardLayoutRegistry(choice,language));
    EXPECT(RegistryCalls==1);
    expect_equal(Captured,expected);
    expect_equal(snapshot(MUIGetLayoutsList(language)),before);
    RegistryFailure=false;
}
static void check_profiles(PCWSTR choice,const std::vector<MUI_LAYOUTS> &expected)
{
    Case="preserve selected and auxiliary language/layout profile pairs";
    const auto before=snapshot(OverrideLayouts);
    for(unsigned repeat=0;repeat<2;++repeat)
    {
        RegistryCalls=0;
        EXPECT(ProcessKeyboardLayoutRegistry(choice,L"00000804"));
        EXPECT(RegistryCalls==1);
        expect_equal(Captured,expected);
        expect_equal(snapshot(OverrideLayouts),before);
    }
}
int main(int argc,char **argv)
{
    bool regressionOnly=argc>1 && std::string(argv[1])=="original";
    for(PCWSTR language:{L"00000409",L"00000411",L"00000804"})
    {
        enumerate(language);
        select(language,L"0000040c");
        select(language,L"00000401");
        select(language,L"00000411");
        select(language,L"00000409");
        select(language,L"00000804");
        select(language,L"00010409");
        if(!regressionOnly)
            for(unsigned failure=1;failure<=5;++failure) enumerate(language,failure);
    }
    if(!regressionOnly)
    {
        Case="invalid external keyboard identifiers do not write the registry";
        for(PCWSTR choice:{PCWSTR(nullptr),L"",L"0",L"00000000",L"00010000",L"0000040",L"000004090",L"0000040g",L" 0000409",L"+0000409",L"0x000409"})
        {
            RegistryCalls=MUIReads=0;
            EXPECT(!ProcessKeyboardLayoutRegistry(choice,L"00000409"));
            EXPECT(RegistryCalls==0 && MUIReads==0);
        }
        Case="duplicate and full baseline keyboard lists";
        std::array<MUI_LAYOUTS,32> baseline={};
        baseline[0]={0x409,0x409};
        baseline[1]={0x411,0x411};
        baseline[2]=baseline[0];
        baseline[3]={0x804,0x804};
        OverrideLayouts=baseline.data();
        select(L"00000409",L"0000040c");
        select(L"00000409",L"00000411");
        baseline={};
        for(unsigned index=0;index<18;++index) baseline[index]={LANGID(0x409+index),KLID(0x10000409+index)};
        select(L"00000409",L"0000040c");
        baseline[18]={0x425,0x20000425};
        RegistryCalls=0;
        EXPECT(!ProcessKeyboardLayoutRegistry(L"0000040c",L"00000409"));
        EXPECT(RegistryCalls==0);
        baseline[19]=baseline[18];
        baseline[20]=baseline[0];
        RegistryCalls=0;
        EXPECT(ProcessKeyboardLayoutRegistry(L"10000409",L"00000409"));
        EXPECT(RegistryCalls==1 && Captured.size()==19);
        baseline={};
        baseline[0]={0x804,0x409};
        baseline[1]={0x409,0x409};
        baseline[2]=baseline[0];
        baseline[3]={0x411,0xe0010411};
        check_profiles(L"00000409",{{0x804,0x409},{0x409,0x409},{0x411,0xe0010411}});
        check_profiles(L"E0010411",{{0x411,0xe0010411},{0x804,0x409},{0x409,0x409}});
        check_profiles(L"0000040c",{{0x40c,0x40c},{0x804,0x409},{0x409,0x409},{0x411,0xe0010411}});
        baseline={};
        for(unsigned index=0;index<18;++index) baseline[index]={LANGID(0x401+index),0x409};
        std::vector<MUI_LAYOUTS> fullProfiles={{0x40c,0x40c}};
        for(unsigned index=0;index<18;++index) fullProfiles.push_back(baseline[index]);
        check_profiles(L"0000040c",fullProfiles);
        baseline[18]={0x413,0x409};
        RegistryCalls=0;
        EXPECT(!ProcessKeyboardLayoutRegistry(L"0000040c",L"00000804"));
        EXPECT(RegistryCalls==0);
        fullProfiles.clear();
        for(unsigned index=0;index<19;++index) fullProfiles.push_back(baseline[index]);
        check_profiles(L"00000409",fullProfiles);
        OverrideLayouts=nullptr;
    }
    std::cout<<"Setup keyboard choices and registry: "<<ARRAYSIZE(RealLayouts)<<" actual SIF layouts, 3 UI languages, "<<Checks<<" checks, "<<Failures<<" failures\n";
    return Failures?1:0;
}
'''


def data():
    text = (ROOT / 'image/data/txtsetup.sif').read_text(encoding='utf-8-sig')
    sections = {}
    section = None
    for line in text.splitlines():
        if line.strip().startswith(';'):
            continue
        match = re.fullmatch(r'\s*\[([^]]+)\]\s*', line)
        if match:
            section = sections.setdefault(match[1].lower(), [])
        elif section is not None and '=' in line:
            key, value = line.split('=', 1)
            section.append((key.strip(), value.strip().strip('"')))
    default = dict(sections['nls'])['DefaultLayout']
    text = 'static const WCHAR RealDefaultLayout[] = L' + json.dumps(default) + ';\n'
    text += 'static const INF_ENTRY RealLayouts[] = {\n'
    for key, value in sections['keyboardlayout']:
        if not re.fullmatch(r'[0-9a-fA-F]{8}', key):
            raise ValueError('Invalid keyboard layout fixture: ' + key)
        text += '{L' + json.dumps(key) + ', L' + json.dumps(value) + '},\n'
    text += '};\n'
    for locale in ('en-US', 'ja-JP', 'zh-CN'):
        text += '#include "' + str(ROOT / 'base/setup/lib/lang' / (locale + '.h')) + '"\n'
    return text


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default='c++')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--verify-regression', metavar='REVISION')
    parser.add_argument('--verify-profile-regression', action='store_true')
    args = parser.parse_args()
    source = SOURCE.read_text()
    variants = [('current', source)]
    if args.verify_regression:
        original = subprocess.check_output(['git', 'show', args.verify_regression + ':base/setup/lib/settings.c'], cwd=ROOT, text=True)
        variants.append(('original keyboard selection', original))
    if args.verify_profile_regression:
        preserve = '''    for (uIndex = 0; LayoutsList[uIndex].LangID != 0; ++uIndex)
    {
        if (LayoutsList[uIndex].LayoutID == LayoutId)
        {
            NewLayoutsList[0] = LayoutsList[uIndex];
            break;
        }
    }

'''
        full_pair = '''NewLayoutsList[uNewPos].LangID == LayoutsList[uIndex].LangID &&
                NewLayoutsList[uNewPos].LayoutID == LayoutsList[uIndex].LayoutID'''
        if source.count(preserve)!=1 or source.count(full_pair)!=1:
            raise RuntimeError('Cannot isolate original language/layout profile loss')
        lossy = source.replace(preserve,'',1).replace(full_pair,'NewLayoutsList[uNewPos].LayoutID == LayoutsList[uIndex].LayoutID',1)
        variants.append(('original profile loss',lossy))
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        for name, body in variants:
            create = function(body, 'CreateKeyboardLayoutList', 'PGENERIC_LIST')
            registry = function(body, 'ProcessKeyboardLayoutRegistry', 'BOOLEAN')
            (path / 'test.cpp').write_text(PREFIX + data() + STUBS + create + registry + TESTS)
            command = [args.cxx, '-std=c++17', '-Wall', '-Wextra', '-Werror', str(path / 'test.cpp'), '-o', str(path / 'test')]
            if args.sanitize:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
            subprocess.run(command, check=True)
            run = [str(path / 'test')] + (['original'] if name == 'original keyboard selection' else [])
            result = subprocess.run(run)
            if name == 'current' and result.returncode:
                return result.returncode
            if name != 'current':
                if result.returncode != 1:
                    raise RuntimeError(name + ' did not fail its focused regression')
                print(name + ' failed as expected')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
