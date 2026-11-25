#!/usr/bin/env python3
"""
PROJECT:     ReliefWin installation tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise Office media placement and actual installer tree mappings
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
OFFICE = ROOT / "base/applications/reliefoffice"
FILES = (
    "reliefword.exe", "reliefexcel.exe", "reliefppt.exe", "relieftodo.exe",
    "reliefdiagram.exe", "reliefmathpp.exe", "reliefmath.exe", "reliefmath-render.exe",
    "ReliefMath/STIXTwoMath-Regular.otf", "reliefoffice.inf", "UPSTREAM_NOTICE",
    "UPSTREAM_LICENSE", "THIRD_PARTY_LICENSES.txt", "reliefoffice-maintenance.exe",
)


def function(source, name, result):
    match = re.search(r"\n" + name + r"\([^;]+?\n\{", source)
    assert match, "actual implementation of " + name
    start = match.start() + 1
    end, depth = source.index("{", start) + 1, 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static " + result + " " + source[start:end] + "\n"


def cmake_function(source, name):
    match = re.search(r"^function\(" + name + r"\).*?^endfunction\(\)", source, re.M | re.S)
    assert match, "actual CMake function " + name
    return match.group() + "\n"


def configure(work, office_source, math_source, generator, data_source):
    fixture = work / "office"
    fixture.mkdir(exist_ok=True)
    (fixture / "CMakeLists.txt").write_text(office_source)
    (fixture / "math.cmake").write_text(math_source)
    for name in ("dependencies.cmake", "cairo.cmake"):
        (fixture / name).write_text("")
    for name in ("ReliefMath", "maintenance", "UPSTREAM_LICENSE", "UPSTREAM_NOTICE", "THIRD_PARTY_LICENSES.txt"):
        link = fixture / name
        if not link.exists():
            link.symlink_to(OFFICE / name, target_is_directory=(OFFICE / name).is_dir())
    image = work / "image/data"
    image.mkdir(parents=True, exist_ok=True)
    original = (ROOT / "image/data/txtsetup.sif").read_bytes()
    (image / "txtsetup.sif").write_bytes(original)
    (work / "create_txtsetup_sif.cmake").write_text(generator)
    selections = re.findall(r"^add_cd_file\([^\n]*txtsetup[^\n]*\)$", data_source, re.M)
    assert len(selections) == 2, "both actual setup SIF media selections"
    noops = (
        "add_library", "add_executable", "target_link_libraries", "include_directories",
        "add_compile_definitions", "add_custom_command", "target_include_directories",
        "add_rc_deps", "set_module_type", "add_importlibs",
    )
    mocks = "".join("function(" + name + ")\nendfunction()\n" for name in noops)
    (work / "CMakeLists.txt").write_text(r'''cmake_minimum_required(VERSION 3.17)
project(ReliefOfficeDeployment NONE)
set(REACTOS_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(REACTOS_BINARY_DIR "${CMAKE_BINARY_DIR}")
set(ARCH amd64)
set(_generated "${CMAKE_BINARY_DIR}/generated")
''' + mocks + r'''
function(set_target_properties target)
    cmake_parse_arguments(T "" "OUTPUT_NAME" "" ${ARGN})
    if(T_OUTPUT_NAME)
        set_property(GLOBAL PROPERTY "office_name_${target}" "${T_OUTPUT_NAME}")
    endif()
endfunction()
function(add_cd_file)
    cmake_parse_arguments(C "NO_CAB" "TARGET;DESTINATION;NAME_ON_CD" "FILE;FOR" ${ARGN})
    if(C_TARGET)
        get_property(name GLOBAL PROPERTY "office_name_${C_TARGET}")
        if(NOT name)
            set(name "${C_TARGET}")
        endif()
        set(C_FILE "${CMAKE_BINARY_DIR}/${name}.exe")
    endif()
    foreach(source IN LISTS C_FILE)
        if(C_NAME_ON_CD)
            set(name "${C_NAME_ON_CD}")
        else()
            get_filename_component(name "${source}" NAME)
        endif()
        foreach(mode IN LISTS C_FOR)
            set_property(GLOBAL APPEND PROPERTY "deployment_${mode}"
                "${C_DESTINATION}/${name}|${source}")
        endforeach()
    endforeach()
endfunction()
add_subdirectory(office)
include("${CMAKE_SOURCE_DIR}/create_txtsetup_sif.cmake")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/boot/bootdata")
create_txtsetup_sif()
''' + "\n".join(selections) + r'''
foreach(mode bootcd preinstall livecd)
    get_property(entries GLOBAL PROPERTY "deployment_${mode}")
    string(REPLACE ";" "\n" entries "${entries}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/${mode}.$<CONFIG>.lst" CONTENT "${entries}\n")
endforeach()
''')
    result = subprocess.run(["cmake", "-S", str(work), "-B", str(work / "generated"),
                             "-DCMAKE_BUILD_TYPE=Debug"], capture_output=True, text=True)
    assert result.returncode == 0, "actual Office CMake execution:\n" + result.stdout + result.stderr
    build = work / "generated"
    entries = {}
    for mode in ("bootcd", "preinstall", "livecd"):
        lines = (build / (mode + ".Debug.lst")).read_text().splitlines()
        pairs = [line.split("|", 1) for line in lines if line]
        assert len(pairs) == len(dict(pairs)), mode + " destinations have no duplicates"
        entries[mode] = dict(pairs)
    for mode, prefix in (("bootcd", "deployment/ReliefOffice/"),
                         ("preinstall", "Program Files/ReliefOffice/")):
        expected = {prefix + name for name in FILES}
        actual = {name for name in entries[mode] if not name.endswith("txtsetup.sif")}
        assert actual == expected, (mode + " exact fourteen Office deployment files", sorted(actual ^ expected))
        for name in FILES:
            source = Path(entries[mode][prefix + name])
            if not name.endswith(".exe"):
                expected_source = OFFICE / ("maintenance/reliefoffice.inf" if name == "reliefoffice.inf" else name)
                assert source.resolve() == expected_source.resolve(), "actual source of " + name
    assert entries["livecd"] == {}, "Office has no livecd install entries"
    assert not any(name.startswith("Program Files/") for name in entries["bootcd"]), "BootCD has no installed Office tree"
    original_sif = build / "boot/bootdata/txtsetup.Debug.sif"
    bootcd_sif = build / "boot/bootdata/txtsetup.bootcd.Debug.sif"
    assert original_sif.read_bytes() == original.replace(b"\r\n", b"\n"), "preinstall SIF retains the original CMake-generated content"
    assert Path(entries["preinstall"]["amd64/txtsetup.sif"]) == original_sif
    assert Path(entries["bootcd"]["reactos/txtsetup.sif"]) == bootcd_sif, "BootCD selects its specialized SIF"
    assert bootcd_sif.is_file(), "specialized BootCD setup SIF generated"
    return bootcd_sif, original_sif


def mapping_harness(work, bootcd_sif, preinstall_sif):
    loader = (ROOT / "bootloader/winload/install.c").read_text()
    install = (ROOT / "base/setup/lib/install.c").read_text()
    paths = (ROOT / "base/setup/lib/utils/filesup.c").read_text()
    harness = r'''
#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wchar.h>
#define _In_
#define _In_z_
#define _Inout_
#define _Out_
#define _Out_writes_z_(n)
#define IN
#define OUT
#define VOID void
#define TRUE 1
#define FALSE 0
#define ANSI_NULL '\0'
#define UNICODE_NULL L'\0'
#define MAX_PATH 260
#define ANYSIZE_ARRAY 1
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FIELD_OFFSET(t,m) offsetof(t,m)
#define NT_SUCCESS(s) ((s)>=0)
#define STATUS_SUCCESS 0
#define STATUS_BUFFER_OVERFLOW ((int32_t)0x80000005)
#define OBJ_NAME_PATH_SEPARATOR L'\\'
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _wcsnicmp wcsncasecmp
#define min(a,b) ((a)<(b)?(a):(b))
#define RtlZeroMemory(p,n) memset(p,0,n)
#define RtlCopyMemory(p,q,n) memcpy(p,q,n)
#define RtlMoveMemory(p,q,n) memmove(p,q,n)
#define DPRINT(...) ((void)0)
#define FwAllocate malloc
#define FwFree free
typedef char CHAR,*PSTR;
typedef const char *PCSTR;
typedef uint32_t ULONG;
typedef unsigned char UCHAR,BOOLEAN,*PBOOLEAN;
typedef size_t SIZE_T;
typedef int32_t NTSTATUS;
typedef wchar_t WCHAR,*PWSTR,*PWCHAR;
typedef const WCHAR *PCWSTR;
static unsigned Checks;
#define CHECK(x) do {++Checks;if(!(x)){fprintf(stderr,"deployment line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static NTSTATUS RtlStringCbCopyA(PSTR Out,SIZE_T Size,PCSTR Source)
{
    size_t Length=strlen(Source);
    if(!Size)return STATUS_BUFFER_OVERFLOW;
    size_t Count=Length<Size?Length:Size-1;memcpy(Out,Source,Count);Out[Count]=0;
    return Length<Size?0:STATUS_BUFFER_OVERFLOW;
}
static NTSTATUS RtlStringCbPrintfA(PSTR Out,SIZE_T Size,PCSTR Format,...)
{
    va_list Args;va_start(Args,Format);int Count=vsnprintf(Out,Size,Format,Args);va_end(Args);
    return Count>=0&&(size_t)Count<Size?0:STATUS_BUFFER_OVERFLOW;
}
#define RtlStringCchPrintfA RtlStringCbPrintfA
#define RtlStringCchCopyA RtlStringCbCopyA
static NTSTATUS RtlStringCchCopyW(PWSTR Out,SIZE_T Size,PCWSTR Source)
{
    size_t Length=wcslen(Source);
    if(!Size)return STATUS_BUFFER_OVERFLOW;
    size_t Count=Length<Size?Length:Size-1;wmemcpy(Out,Source,Count);Out[Count]=0;
    return Length<Size?0:STATUS_BUFFER_OVERFLOW;
}
static NTSTATUS RtlStringCchCatW(PWSTR Out,SIZE_T Size,PCWSTR Source)
{
    size_t Length=wcslen(Out);
    return Length<Size?RtlStringCchCopyW(Out+Length,Size-Length,Source):STATUS_BUFFER_OVERFLOW;
}
'''
    for name in ("OSL_INSTALL_PATH_CHARS", "OSL_INSTALL_NAME_CHARS", "OSL_INSTALL_BUCKETS",
                 "OSL_INSTALL_TREES", "OSL_INSTALL_COMPUTER", "OSL_INSTALL_FIELDS"):
        harness += re.search(r"^#define " + name + r" .+$", loader, re.M).group() + "\n"
    for name in ("OSL_NAME", "OSL_NAME_SET", "OSL_TREE", "OSL_COMPUTER_FILE"):
        harness += re.search(r"typedef struct _" + name + r"\b.*?\}\s*[^;]+;", loader, re.S).group() + "\n"
    harness += r'''
typedef struct {
    PSTR SetupText;
    CHAR SystemRoot[OSL_INSTALL_PATH_CHARS];
    OSL_TREE Trees[OSL_INSTALL_TREES];
    ULONG TreeCount,SystemTree,ComputerCount;
    OSL_COMPUTER_FILE ComputerFiles[OSL_INSTALL_COMPUTER];
    OSL_NAME_SET Exclude,Preserve,Computers;
} OSL_INSTALL,*POSL_INSTALL;
'''
    for name, result in (("OslNameHash", "ULONG"), ("OslSamePath", "BOOLEAN"),
                         ("OslFindName", "POSL_NAME"), ("OslAddName", "POSL_NAME"),
                         ("OslFreeNames", "VOID"), ("OslReadLine", "BOOLEAN"),
                         ("OslInfToken", "VOID"), ("OslParseSetupLine", "VOID"),
                         ("OslParseSetupInf", "VOID"), ("OslMapEntry", "BOOLEAN")):
        harness += function(loader, name, result)
    harness += r'''
typedef struct {PWSTR Buffer;} UNICODE_STRING;
typedef struct {UNICODE_STRING SourceRootPath;POSL_INSTALL SetupInf;} USETUP_DATA,*PUSETUP_DATA;
typedef unsigned INFCONTEXT;
'''
    harness += re.search(r"#define UPGRADE_MAX_TREES .*?\} UPGRADE_PLAN, \*PUPGRADE_PLAN;", install, re.S).group() + "\n"
    harness += r'''
static void Widen(PWSTR Out,PCSTR In)
{while(*In)*Out++=(unsigned char)*In++;*Out=0;}
static BOOLEAN SpInfFindFirstLine(POSL_INSTALL Inf,PCWSTR Section,PCWSTR Key,INFCONTEXT *Context)
{
    CHAR Narrow[MAX_PATH];size_t Length=wcslen(Key);(void)Context;
    CHECK(!wcscmp(Section,L"SystemTrees.Exclude")&&Length<sizeof(Narrow));
    for(size_t i=0;i<=Length;i++)Narrow[i]=(CHAR)Key[i];
    return OslFindName(&Inf->Exclude,Narrow)!=NULL;
}
'''
    for name in ("ConcatPathsV", "CombinePathsV", "CombinePaths"):
        harness += function(paths, name, "NTSTATUS")
    harness += function(install, "BuildFullDirectoryPath", "NTSTATUS")
    harness += function(install, "MapUpgradeEntry", "BOOLEAN")
    harness += r'''
static void Load(POSL_INSTALL Inf,const char *Path)
{
    FILE *File=fopen(Path,"rb");CHECK(File!=NULL);CHECK(!fseek(File,0,SEEK_END));
    long Size=ftell(File);CHECK(Size>0);rewind(File);Inf->SetupText=malloc((size_t)Size+1);CHECK(Inf->SetupText!=NULL);
    CHECK(fread(Inf->SetupText,1,(size_t)Size,File)==(size_t)Size);Inf->SetupText[Size]=0;CHECK(!fclose(File));
    strcpy(Inf->SystemRoot,"Windows\\");OslParseSetupInf(Inf,FALSE);
}
static void MakePlan(POSL_INSTALL Inf,PUPGRADE_PLAN Plan,PUSETUP_DATA Data)
{
    Data->SourceRootPath.Buffer=L"\\Device\\Ramdisk0";Data->SetupInf=Inf;Plan->SetupData=Data;Plan->TreeCount=Inf->TreeCount;
    for(ULONG i=0;i<Inf->TreeCount;i++){
        WCHAR Target[MAX_PATH];Widen(Plan->Trees[i].Media,Inf->Trees[i].Media);Widen(Target,Inf->Trees[i].Target);
        CHECK(NT_SUCCESS(BuildFullDirectoryPath(L"\\Device\\HarddiskVolume9\\",L"Windows",Target,Plan->Trees[i].Target,MAX_PATH)));
    }
}
static void Mapping(POSL_INSTALL Inf,PUPGRADE_PLAN Plan,PCSTR Entry,PCSTR Expected,BOOLEAN Directory)
{
    CHAR Name[OSL_INSTALL_PATH_CHARS],Target[OSL_INSTALL_PATH_CHARS];BOOLEAN IsDirectory;
    WCHAR Wide[MAX_PATH],Want[MAX_PATH],Full[MAX_PATH],Source[MAX_PATH];
    Widen(Wide,Entry);CHECK(OslMapEntry(Inf,Entry,Name,Target,&IsDirectory)==(Expected!=NULL));
    if(Expected)CHECK(IsDirectory==Directory);
    CHECK(MapUpgradeEntry(Plan,Wide,&IsDirectory)==(Expected!=NULL));
    if(!Expected)return;
    CHECK(IsDirectory==Directory);CHECK(!strcmp(Target,Expected));Widen(Want,Expected);
    CHECK(NT_SUCCESS(CombinePaths(Full,MAX_PATH,2,L"\\Device\\HarddiskVolume9",Want)));
    size_t TargetLength=wcslen(Full);
    CHECK(!wcscmp(Plan->Target,Full)||(Directory&&wcslen(Plan->Target)==TargetLength+1&&
          Plan->Target[TargetLength]==L'\\'&&!wcsncmp(Plan->Target,Full,TargetLength)));
    wcscpy(Source,Wide);size_t Length=wcslen(Source);if(Directory&&Length&&Source[Length-1]==L'\\')Source[Length-1]=0;
    CHECK(NT_SUCCESS(CombinePaths(Full,MAX_PATH,2,L"\\Device\\Ramdisk0",Source)));
    CHECK(!wcscmp(Plan->Source,Full));
}
static void Cleanup(POSL_INSTALL Inf)
{OslFreeNames(&Inf->Exclude);OslFreeNames(&Inf->Preserve);OslFreeNames(&Inf->Computers);free(Inf->SetupText);}
int main(int argc,char **argv)
{
    CHECK(argc==3);OSL_INSTALL Boot={0},Pre={0};UPGRADE_PLAN BootPlan={0},PrePlan={0};USETUP_DATA BootData,PreData;
    Load(&Boot,argv[1]);Load(&Pre,argv[2]);CHECK(Pre.TreeCount==2);
    MakePlan(&Boot,&BootPlan,&BootData);MakePlan(&Pre,&PrePlan,&PreData);
'''
    for name in FILES:
        native = name.replace("/", "\\")
        entry = "deployment\\ReliefOffice\\" + native
        target = "Program Files\\ReliefOffice\\" + native
        harness += "    Mapping(&Boot,&BootPlan," + json.dumps(entry) + "," + json.dumps(target) + ",FALSE);\n"
        harness += "    Mapping(&Boot,&BootPlan," + json.dumps(target) + "," + json.dumps(target) + ",FALSE);\n"
        harness += "    Mapping(&Pre,&PrePlan," + json.dumps(target) + "," + json.dumps(target) + ",FALSE);\n"
        harness += "    Mapping(&Pre,&PrePlan," + json.dumps(entry) + ",NULL,FALSE);\n"
    harness += r'''
    Mapping(&Boot,&BootPlan,"deployment\\ReliefOffice\\","Program Files\\ReliefOffice",TRUE);
    Mapping(&Boot,&BootPlan,"deployment\\ReliefOffice\\ReliefMath\\","Program Files\\ReliefOffice\\ReliefMath",TRUE);
    Mapping(&Boot,&BootPlan,"Program Files\\ReliefOffice\\reliefword.exe","Program Files\\ReliefOffice\\reliefword.exe",FALSE);
    Mapping(&Boot,&BootPlan,"Program Files\\ReliefOffice\\user-document.rword","Program Files\\ReliefOffice\\user-document.rword",FALSE);
    const char *Invalid[]={"deployment\\ReliefOfficeExtra\\reliefword.exe","deployment\\ReliefOffice2\\reliefword.exe",
        "deploymentX\\ReliefOffice\\reliefword.exe","deployment\\reliefword.exe","deployment\\",
        "Windows\\system32\\config\\SOFTWARE","Windows\\SysWOW64\\config\\SOFTWARE"};
    for(size_t i=0;i<ARRAYSIZE(Invalid);i++)Mapping(&Boot,&BootPlan,Invalid[i],NULL,FALSE);
    WCHAR Full[MAX_PATH];
    CHECK(!BuildFullDirectoryPath(L"C:\\",L"Windows",L"\\",Full,MAX_PATH)&&!wcscmp(Full,L"C:\\Windows"));
    CHECK(!BuildFullDirectoryPath(L"C:\\",L"Windows",L"system32",Full,MAX_PATH)&&!wcscmp(Full,L"C:\\Windows\\system32"));
    CHECK(!BuildFullDirectoryPath(L"C:\\",L"系统",L"\\Program Files\\ReliefOffice",Full,MAX_PATH)&&!wcscmp(Full,L"C:\\Program Files\\ReliefOffice"));
    CHECK(BuildFullDirectoryPath(L"C:\\",L"Windows",L"\\Program Files\\ReliefOffice",Full,8)==STATUS_BUFFER_OVERFLOW);
    CHECK(Boot.TreeCount==3);Cleanup(&Boot);Cleanup(&Pre);printf("Office actual loader parser and loader/setuplib mappings: %u checks passed\n",Checks);return 0;
}
'''
    c_file, binary = work / "mapping.c", work / "mapping"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary), str(bootcd_sif), str(preinstall_sif)], check=True)
    return binary


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--verify-regression", action="store_true")
    args = parser.parse_args()
    work = Path(tempfile.gettempdir()) / "reliefwin-brand-host/office-deployment"
    work.mkdir(parents=True, exist_ok=True)
    sources = ((OFFICE / "CMakeLists.txt").read_text(), (OFFICE / "math.cmake").read_text(),
               cmake_function((ROOT / "sdk/cmake/CMakeMacros.cmake").read_text(), "create_txtsetup_sif"),
               (ROOT / "image/data/CMakeLists.txt").read_text())
    manifest = (OFFICE / "maintenance/manifest.h").read_text()
    actual_files = tuple(name.replace("\\\\", "/") for name in re.findall(r'L"([^"\n]+)"', manifest.split("OfficeFiles[]", 1)[1].split("};", 1)[0]))
    assert actual_files == FILES, "deployment matches the fixed maintenance manifest"
    bootcd_sif, preinstall_sif = configure(work, *sources)
    binary = mapping_harness(work, bootcd_sif, preinstall_sif)
    if args.verify_regression:
        legacy_office = re.sub(r'set_property\(GLOBAL APPEND PROPERTY BOOTCD_SYSTEM_TREES.*?\)\n', "", sources[0], count=1, flags=re.S)
        assert legacy_office != sources[0], "actual deployment tree property exists"
        legacy_office = legacy_office.replace('DESTINATION "deployment/ReliefOffice"', 'DESTINATION "Program Files/ReliefOffice"')
        legacy_math = sources[1].replace('DESTINATION "deployment/ReliefOffice', 'DESTINATION "Program Files/ReliefOffice')
        legacy_generator = re.sub(r'\n    file\(READ.*?^endfunction\(\)', "\nendfunction()", sources[2], count=1, flags=re.M | re.S)
        assert legacy_generator != sources[2], "actual specialized SIF generation exists"
        legacy_data = sources[3].replace("txtsetup.bootcd.$<CONFIG>.sif", "txtsetup.$<CONFIG>.sif")
        mutations = (
            ((legacy_office, legacy_math, legacy_generator, legacy_data), "bootcd exact fourteen Office deployment files"),
            ((*sources[:3], sources[3].replace("txtsetup.bootcd.$<CONFIG>.sif", "txtsetup.$<CONFIG>.sif")),
             "BootCD selects its specialized SIF"),
        )
        for mutation, expected_failure in mutations:
            try:
                configure(work, *mutation)
            except AssertionError as error:
                assert expected_failure in str(error), "regression failed for an unrelated reason: " + str(error)
            else:
                raise AssertionError("old installed media placement or shared SIF mutation escaped the regression")
        unmapped = re.sub(r'set_property\(GLOBAL APPEND PROPERTY BOOTCD_SYSTEM_TREES.*?\)\n', "", sources[0], count=1, flags=re.S)
        assert unmapped != sources[0], "actual deployment tree property exists"
        missing_sif, original_sif = configure(work, unmapped, *sources[1:])
        result = subprocess.run([str(binary), str(missing_sif), str(original_sif)], capture_output=True, text=True)
        assert result.returncode and "OslMapEntry" in result.stderr, "missing deployment tree must fail the actual mapping check"
        configure(work, *sources)
        print("Office deployment regression: original fourteen-file installed layout/single SIF, shared SIF and missing tree mapping rejected")
    print("Office actual CMake: fourteen offline BootCD files, unchanged preinstall payload/SIF and media SIF selection passed")
    print("Live has no Office install entries; its independent offline deployment payload is intentionally permitted")


if __name__ == "__main__":
    main()
