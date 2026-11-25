#!/usr/bin/env python3
"""
PROJECT:     ReliefOffice maintenance tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise installed-file ownership, retry and registration contracts
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[4]
HERE = Path(__file__).resolve().parent


def function(source, name, result):
    match = re.search(r"\n" + name + r"\([^;]+?\n\{", source)
    start = match.start() + 1
    body = source.index("{", start)
    depth, end = 1, body + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static " + result + " " + source[start:end] + "\n"


def test_shortcuts(source, folder):
    harness = r'''
#include <wchar.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define _In_
#define MAX_PATH 260
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FAILED(x) ((x)<0)
#define SUCCEEDED(x) ((x)>=0)
#define S_OK 0
#define S_FALSE 1
#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_GEN_FAILURE 31
#define ERROR_FILENAME_EXCED_RANGE 206
#define INVALID_FILE_ATTRIBUTES 0xffffffffu
#define CLSCTX_INPROC_SERVER 1
#define STGM_READ 0
#define SLGP_RAWPATH 4
typedef wchar_t WCHAR,*PWSTR;
typedef const WCHAR *PCWSTR;
typedef uint32_t DWORD;
typedef void *PVOID;
typedef int HRESULT,IShellLinkW,IPersistFile,WIN32_FIND_DATAW;
static const int CLSID_ShellLink=1,IID_IShellLinkW=2,IID_IPersistFile=3;
static int Link,File,CreateResult,QueryResult,LoadResult,PathResult,OwnTarget;
static unsigned Checks,Deletes,LinkReleases,FileReleases,Creates;
static DWORD AttributeError,DeleteError;
static PCWSTR Directory=L"C:\\Program Files\\ReliefOffice";
#define CHECK(x) do {++Checks;if(!(x)){fprintf(stderr,"shortcut line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static int StringCchPrintfW(PWSTR Out,size_t Count,PCWSTR Format,...)
{va_list Args;va_start(Args,Format);int N=vswprintf(Out,Count,Format,Args);va_end(Args);return N<0||(size_t)N>=Count?-1:0;}
static int _wcsicmp(PCWSTR A,PCWSTR B)
{return wcscmp(A,B);}
static DWORD GetFileAttributesW(PCWSTR Path)
{CHECK(!wcscmp(Path,L"C:\\Programs\\ReliefOffice\\ReliefWord.lnk"));return AttributeError?INVALID_FILE_ATTRIBUTES:0;}
static DWORD GetLastError(void)
{return AttributeError?AttributeError:DeleteError;}
static int DeleteFileW(PCWSTR Path)
{CHECK(!wcscmp(Path,L"C:\\Programs\\ReliefOffice\\ReliefWord.lnk"));Deletes++;return !DeleteError;}
static HRESULT CoCreateInstance(const int *Class,void *Outer,int Context,const int *Iid,void **Out)
{CHECK(Class==&CLSID_ShellLink&&!Outer&&Context==1&&Iid==&IID_IShellLinkW);Creates++;*Out=&Link;return CreateResult;}
static HRESULT IShellLinkW_QueryInterface(IShellLinkW *Object,const int *Iid,void **Out)
{CHECK(Object==&Link&&Iid==&IID_IPersistFile);*Out=&File;return QueryResult;}
static HRESULT IPersistFile_Load(IPersistFile *Object,PCWSTR Path,DWORD Flags)
{CHECK(Object==&File&&!wcscmp(Path,L"C:\\Programs\\ReliefOffice\\ReliefWord.lnk")&&!Flags);return LoadResult;}
static HRESULT IShellLinkW_GetPath(IShellLinkW *Object,PWSTR Path,DWORD Count,WIN32_FIND_DATAW *Data,DWORD Flags)
{CHECK(Object==&Link&&Data&&Flags==4);if(PathResult==S_FALSE)Path[0]=0;else StringCchPrintfW(Path,Count,L"%ls\\reliefword.exe",OwnTarget?Directory:L"D:\\Other");return PathResult;}
static void IPersistFile_Release(IPersistFile *Object)
{CHECK(Object==&File);FileReleases++;}
static void IShellLinkW_Release(IShellLinkW *Object)
{CHECK(Object==&Link);LinkReleases++;}
'''
    harness += (HERE / "manifest.h").read_text().replace("#pragma once", "")
    harness += function(source, "OfficeRemoveShortcut", "DWORD")
    harness += r'''
static void Reset(void)
{CreateResult=QueryResult=LoadResult=PathResult=0;OwnTarget=1;Deletes=LinkReleases=FileReleases=Creates=0;AttributeError=DeleteError=0;}
static DWORD Remove(void)
{return OfficeRemoveShortcut(Directory,L"C:\\Programs",&OfficeApplications[0]);}
int main(void)
{
 (void)OfficeFiles;
 Reset();CHECK(Remove()==0&&Deletes==1&&LinkReleases==1&&FileReleases==1);
 Reset();OwnTarget=0;CHECK(Remove()==0&&!Deletes&&LinkReleases==1&&FileReleases==1);
 Reset();PathResult=S_FALSE;CHECK(Remove()==0&&!Deletes&&LinkReleases==1&&FileReleases==1);
 Reset();PathResult=-1;CHECK(Remove()==31&&!Deletes&&LinkReleases==1&&FileReleases==1);
 Reset();CreateResult=-1;CHECK(Remove()==31&&!Deletes&&!LinkReleases&&!FileReleases);
 Reset();QueryResult=-1;CHECK(Remove()==31&&!Deletes&&LinkReleases==1&&!FileReleases);
 Reset();LoadResult=-1;CHECK(Remove()==31&&!Deletes&&LinkReleases==1&&FileReleases==1);
 Reset();DeleteError=32;CHECK(Remove()==32&&Deletes==1&&LinkReleases==1&&FileReleases==1);
 Reset();AttributeError=2;CHECK(Remove()==0&&!Creates&&!Deletes);
 Reset();AttributeError=3;CHECK(Remove()==0&&!Creates&&!Deletes);
 Reset();AttributeError=5;CHECK(Remove()==5&&!Creates&&!Deletes);
 Reset();Directory=L"D:\\程序文件\\ReliefOffice";CHECK(Remove()==0&&Deletes==1&&LinkReleases==1&&FileReleases==1);
 printf("Office shortcut ownership: %u checks passed against actual COM and cleanup function\n",Checks);
 return 0;
}
'''
    c_file, binary = folder / "office-shortcuts.c", folder / "office-shortcuts"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


def main():
    source = (HERE / "uninstall.c").read_text()
    implementation = "".join(function(source, name, result) for name, result in (
        ("OfficeReadString", "DWORD"), ("OfficeDeleteFile", "DWORD"),
        ("OfficeRemoveAssociation", "DWORD"), ("OfficeRestoreUninstall", "DWORD"), ("OfficeUninstall", "DWORD")))
    harness = r'''
#include <wchar.h>
#include <wctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define _In_
#define _In_opt_
#define _Out_writes_(n)
#define VOID void
#define MAX_PATH 260
#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_ACCESS_DENIED 5
#define ERROR_INVALID_DATA 13
#define ERROR_SHARING_VIOLATION 32
#define ERROR_FILENAME_EXCED_RANGE 206
#define ERROR_GEN_FAILURE 31
#define KEY_QUERY_VALUE 1
#define KEY_SET_VALUE 2
#define REG_SZ 1
#define REG_DWORD 4
#define UNICODE_NULL 0
#define HKEY_LOCAL_MACHINE 1
#define MOVEFILE_DELAY_UNTIL_REBOOT 4
#define COINIT_APARTMENTTHREADED 2
#define CSIDL_COMMON_PROGRAMS 23
#define SHGFP_TYPE_CURRENT 0
#define SHCNE_ASSOCCHANGED 0x8000000
#define SHCNF_IDLIST 0
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FAILED(x) ((x)<0)
typedef wchar_t WCHAR, *PWSTR;
typedef const wchar_t *PCWSTR;
typedef uint32_t DWORD, ULONG;
typedef unsigned char BYTE, *PBYTE;
typedef int HKEY, HRESULT;
static unsigned Checks, DeleteCalls, DeleteTrees, DeleteValues, Moves, ShortcutCalls;
static int Busy, OwnCommand, OwnExtension, Registered, ReadType, MissingFiles, ShortcutFailure, QueueFailure, RegistryDeleteFailure;
static WCHAR LastKey[520];
static const WCHAR OfficeUninstallKey[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ReliefOffice";
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
static DWORD LastError;
static DWORD GetLastError(void) { return LastError; }
static int _wcsicmp(PCWSTR A,PCWSTR B)
{ for (;*A&&*B;A++,B++) { int D=towlower(*A)-towlower(*B); if(D) return D; } return *A-*B; }
static int StringCchPrintfW(PWSTR Out,size_t Count,PCWSTR Format,...)
{
    va_list Args; va_start(Args,Format); int N=vswprintf(Out,Count,Format,Args); va_end(Args);
    return N<0 || (size_t)N>=Count ? -1 : 0;
}
static DWORD RegOpenKeyExW(HKEY Root,PCWSTR Key,DWORD Options,DWORD Access,HKEY *Handle)
{ (void)Options;(void)Access; CHECK(Root==1); wcscpy(LastKey,Key); *Handle=2; return 0; }
static DWORD RegQueryValueExW(HKEY Key,PCWSTR Name,void *Reserved,DWORD *Type,PBYTE Value,DWORD *Size)
{
    PCWSTR Data;
    (void)Reserved;CHECK(Key==2);
    if (Name && !wcscmp(Name,L"InstallLocation"))
    { if(!Registered) return ERROR_FILE_NOT_FOUND; Data=Registered==1?L"C:\\Program Files\\ReliefOffice":L"D:\\Other"; }
    else if (wcsstr(LastKey,L"shell\\open\\command"))
        Data=OwnCommand?L"\"C:\\Program Files\\ReliefOffice\\reliefword.exe\" \"%1\"":L"\"D:\\Other\\word.exe\" \"%1\"";
    else Data=OwnExtension?L"ReliefOffice.Word":L"Other.Word";
    DWORD Needed=(wcslen(Data)+1)*sizeof(WCHAR);CHECK(*Size>=Needed);memcpy(Value,Data,Needed);*Size=Needed;*Type=ReadType;return 0;
}
static DWORD RegCloseKey(HKEY Key) { CHECK(Key==2);return 0; }
static DWORD RegDeleteValueW(HKEY Key,PCWSTR Name)
{ CHECK(Key==2 && Name==NULL);DeleteValues++;return 0; }
static DWORD RegDeleteTreeW(HKEY Root,PCWSTR Key)
{ CHECK(Root==1);DeleteTrees++;if(!wcscmp(Key,OfficeUninstallKey)){if(RegistryDeleteFailure)return 5;Registered=0;}return 0; }
static int StringCchCopyW(PWSTR Target,size_t Count,PCWSTR Source)
{ CHECK(wcslen(Source)<Count);wcscpy(Target,Source);return 0; }
static DWORD RegCreateKeyExW(HKEY Root,PCWSTR SubKey,DWORD Reserved,PWSTR Class,DWORD Options,DWORD Access,void *Security,HKEY *Key,DWORD *Disposition)
{ CHECK(Root==1&&!wcscmp(SubKey,OfficeUninstallKey)&&!Reserved&&!Class&&!Options&&Access==2&&!Security&&!Disposition);*Key=2;Registered=1;return 0; }
static DWORD RegSetValueExW(HKEY Key,PCWSTR Name,DWORD Reserved,DWORD Type,const BYTE *Data,DWORD Size)
{ CHECK(Key==2&&Name&&!Reserved&&Data&&Size);CHECK(Type==1||Type==4);return 0; }
static int DeleteFileW(PCWSTR Path)
{
    CHECK(wcsstr(Path,L"C:\\Program Files\\ReliefOffice\\")==Path);
    CHECK(!wcsstr(Path,L"document") && !wcsstr(Path,L".rword") && !wcsstr(Path,L"maintenance.exe"));
    DeleteCalls++; if(Busy){LastError=ERROR_SHARING_VIOLATION;return 0;}
    if(MissingFiles){LastError=ERROR_FILE_NOT_FOUND;return 0;}return 1;
}
static HRESULT CoInitializeEx(void *Reserved,DWORD Mode) { CHECK(!Reserved&&Mode==2);return 0; }
static void CoUninitialize(void) {}
static HRESULT SHGetFolderPathW(void *Window,int Folder,void *Token,DWORD Flags,PWSTR Path)
{ CHECK(!Window&&Folder==23&&!Token&&!Flags);wcscpy(Path,L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");return 0; }
static int RemoveDirectoryW(PCWSTR Path)
{ CHECK(!wcsstr(Path,L"Documents"));return 0; }
static int MoveFileExW(PCWSTR Path,PCWSTR Destination,DWORD Flags)
{ CHECK(!Destination&&Flags==4&&wcsstr(Path,L"ReliefOffice"));Moves++;if(QueueFailure){LastError=5;return 0;}return 1; }
static void SHChangeNotify(long Event,unsigned Flags,const void *A,const void *B)
{ CHECK(Event==SHCNE_ASSOCCHANGED&&!Flags&&!A&&!B); }
'''
    harness += (HERE / "manifest.h").read_text().replace("#pragma once", "")
    harness += r'''
static DWORD OfficeRemoveShortcut(PCWSTR Directory,PCWSTR Programs,const OFFICE_APPLICATION *Application)
{ CHECK(Directory&&Programs&&Application);ShortcutCalls++;return ShortcutFailure?5:0; }
'''
    harness += implementation
    harness += r'''
static void Reset(void)
{
    Busy=MissingFiles=ShortcutFailure=QueueFailure=RegistryDeleteFailure=0;OwnCommand=OwnExtension=Registered=ReadType=1;
    DeleteCalls=DeleteTrees=DeleteValues=Moves=ShortcutCalls=0;
}
int main(void)
{
    PCWSTR Dir=L"C:\\Program Files\\ReliefOffice";
    PCWSTR Exe=L"C:\\Program Files\\ReliefOffice\\reliefoffice-maintenance.exe";
    Reset(); CHECK(ARRAYSIZE(OfficeApplications)==7&&ARRAYSIZE(OfficeFiles)==14);
    CHECK(OfficeRemoveAssociation(Dir,&OfficeApplications[0])==0&&DeleteTrees==1&&DeleteValues==1);
    Reset();OwnCommand=0;CHECK(OfficeRemoveAssociation(Dir,&OfficeApplications[0])==0&&!DeleteTrees&&!DeleteValues);
    Reset();OwnExtension=0;CHECK(OfficeRemoveAssociation(Dir,&OfficeApplications[0])==0&&DeleteTrees==1&&!DeleteValues);
    Reset();ReadType=4;CHECK(OfficeRemoveAssociation(Dir,&OfficeApplications[0])==13&&!DeleteTrees&&!DeleteValues);
    Reset();Busy=1;CHECK(OfficeUninstall(Exe,Dir)==32&&Registered&&!Moves&&!ShortcutCalls&&!DeleteTrees);
    Busy=0;MissingFiles=1;CHECK(OfficeUninstall(Exe,Dir)==0&&!Registered&&Moves==2&&ShortcutCalls==7);
    Reset();ShortcutFailure=1;CHECK(OfficeUninstall(Exe,Dir)==5&&Registered&&!Moves);
    ShortcutFailure=0;CHECK(OfficeUninstall(Exe,Dir)==0&&!Registered);
    Reset();QueueFailure=1;CHECK(OfficeUninstall(Exe,Dir)==5&&Registered&&Moves==1);
    QueueFailure=0;CHECK(OfficeUninstall(Exe,Dir)==0&&!Registered);
    Reset();RegistryDeleteFailure=1;CHECK(OfficeUninstall(Exe,Dir)==5&&Registered&&!Moves);
    RegistryDeleteFailure=0;CHECK(OfficeUninstall(Exe,Dir)==0&&!Registered);
    Reset();Registered=2;CHECK(OfficeUninstall(Exe,Dir)==13&&!DeleteCalls&&!Moves);
    Reset();Registered=0;CHECK(OfficeUninstall(Exe,Dir)==2&&!DeleteCalls&&!Moves);
    printf("Office maintenance: %u checks passed against actual ownership, manifest and retry functions\n",Checks);
    return 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    c_file, binary = folder / "office-maintenance.c", folder / "office-maintenance"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    test_shortcuts(source, folder)
    inf = (HERE / "reliefoffice.inf").read_text()
    assert len(re.findall(r"^Name=Relief[^,]+,0$", inf, re.M)) == 7
    assert "Desktop" not in inf and "CopyFiles" not in inf
    assert inf.count(',,0x00000002,"ReliefOffice.') == 7
    assert "SPINST_REGISTRY | SPINST_PROFILEITEMS" in (ROOT / "dll/win32/syssetup/install.c").read_text()
    assert 'SetupSetDirectoryIdW(Inf, DIRID_USER, Directory)' in (ROOT / "dll/win32/syssetup/install.c").read_text()
    print("Office INF: seven common Start menu links and non-clobbering extension defaults verified")

    setup_source = (ROOT / "dll/win32/syssetup/install.c").read_text()
    setup_harness = r'''
#include <wchar.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define VOID void
#define MAX_PATH 260
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define ERROR_SUCCESS 0
#define ERROR_INVALID_DATA 13
#define ERROR_FILENAME_EXCED_RANGE 206
#define HKEY_LOCAL_MACHINE 1
#define KEY_QUERY_VALUE 1
#define REG_SZ 1
#define UNICODE_NULL 0
#define INF_STYLE_WIN4 2
#define DIRID_USER 32768
#define SPINST_REGISTRY 4
#define SPINST_PROFILEITEMS 256
#define INVALID_HANDLE_VALUE (-1)
#define FAILED(x) ((x)<0)
typedef wchar_t WCHAR;
typedef uint32_t DWORD;
typedef unsigned char *PBYTE;
typedef int HKEY,HINF;
static unsigned Checks,OpenInf,CloseInf,SetDir,InstallInf;
static DWORD OpenError,QueryError,Type=1,SizeOverride,DirectoryError,InstallError;
#define CHECK(x) do {++Checks;if(!(x)){fprintf(stderr,"setup line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static DWORD LastError;
static DWORD GetLastError(void){return LastError;}
static DWORD RegOpenKeyExW(HKEY Root,const WCHAR *Name,DWORD Options,DWORD Access,HKEY *Key)
{CHECK(Root==1&&wcsstr(Name,L"CurrentVersion")&&!Options&&Access==1);*Key=2;return OpenError;}
static DWORD RegQueryValueExW(HKEY Key,const WCHAR *Name,void *Reserved,DWORD *ResultType,PBYTE Buffer,DWORD *Size)
{CHECK(Key==2&&!wcscmp(Name,L"ProgramFilesDir")&&!Reserved);const WCHAR *Path=L"D:\\程序文件";*ResultType=Type;DWORD Actual=(wcslen(Path)+1)*sizeof(WCHAR);CHECK(*Size>=Actual);memcpy(Buffer,Path,Actual);*Size=SizeOverride?SizeOverride:Actual;return QueryError;}
static DWORD RegCloseKey(HKEY Key){CHECK(Key==2);return 0;}
static int StringCchCatW(WCHAR *Target,size_t Count,const WCHAR *Suffix)
{if(wcslen(Target)+wcslen(Suffix)>=Count)return -1;wcscat(Target,Suffix);return 0;}
static int StringCchPrintfW(WCHAR *Out,size_t Count,const WCHAR *Format,...)
{
 va_list Args;va_start(Args,Format);int N=vswprintf(Out,Count,Format,Args);va_end(Args);return N<0?-1:0;
}
static HINF SetupOpenInfFileW(const WCHAR *Path,const WCHAR *Class,DWORD Style,void *Line)
{CHECK(!wcscmp(Path,L"D:\\程序文件\\ReliefOffice\\reliefoffice.inf")&&!Class&&Style==2&&!Line);OpenInf++;return 3;}
static int SetupSetDirectoryIdW(HINF Inf,DWORD DirId,const WCHAR *Path)
{CHECK(Inf==3&&DirId==DIRID_USER&&!wcscmp(Path,L"D:\\程序文件\\ReliefOffice"));SetDir++;if(DirectoryError){LastError=DirectoryError;return 0;}return 1;}
static int SetupInstallFromInfSectionW(void *Owner,HINF Inf,const WCHAR *Section,unsigned Flags,void *Root,void *Source,unsigned Copy,void *Callback,void *Context,void *Devices,void *Device)
{CHECK(!Owner&&Inf==3&&!wcscmp(Section,L"DefaultInstall")&&Flags==(4|256)&&!Root&&!Source&&!Copy&&!Callback&&!Context&&!Devices&&!Device);InstallInf++;if(InstallError){LastError=InstallError;return 0;}return 1;}
static void SetupCloseInfFile(HINF Inf){CHECK(Inf==3);CloseInf++;}
'''
    setup_harness += function(setup_source, "InstallReliefOffice", "DWORD")
    setup_harness += r'''
int main(void)
{
 CHECK(InstallReliefOffice()==0&&OpenInf==1&&CloseInf==1&&SetDir==1&&InstallInf==1);
 OpenError=5;CHECK(InstallReliefOffice()==5&&OpenInf==1);OpenError=0;
 QueryError=2;CHECK(InstallReliefOffice()==2&&OpenInf==1);QueryError=0;
 Type=4;CHECK(InstallReliefOffice()==13&&OpenInf==1);Type=1;
 SizeOverride=1;CHECK(InstallReliefOffice()==13&&OpenInf==1);SizeOverride=0;
 DirectoryError=5;CHECK(InstallReliefOffice()==5&&OpenInf==2&&CloseInf==2&&InstallInf==1);DirectoryError=0;
 InstallError=5;CHECK(InstallReliefOffice()==5&&CloseInf==3);InstallError=0;
 CHECK(InstallReliefOffice()==0&&OpenInf==4&&CloseInf==4&&InstallInf==3);
 printf("Office setup: %u checks passed against actual target-directory and SetupAPI registration function\n",Checks);
 return 0;
}
'''
    c_file, binary = folder / "office-setup.c", folder / "office-setup"
    c_file.write_text(setup_harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    main()
