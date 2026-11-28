#!/usr/bin/env python3
"""
PROJECT:     ReliefWin Setup tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise actual OOBE account rollback and completion with injected API failures
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""
import pathlib
import re
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[4]
SOURCE = ROOT / 'base/setup/reliefwin/oobe.c'


def extract(source, name, prefix):
    match = re.search(r'^' + name + r'\(', source, re.M)
    start = source.index('{', match.start())
    cursor, depth = start + 1, 1
    while depth:
        depth += (source[cursor] == '{') - (source[cursor] == '}')
        cursor += 1
    return prefix + ' ' + source[match.start():cursor] + '\n'


PREFIX = r'''
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using DWORD=uint32_t; using BYTE=uint8_t; using BOOL=int; using WCHAR=wchar_t;
using LPWSTR=WCHAR*; using LPCWSTR=const WCHAR*; using PSID=void*; using PVOID=void*; using SIZE_T=size_t; using USHORT=uint16_t;
using HKEY=int; using PCWSTR=const WCHAR*; using PBYTE=BYTE*; using PDWORD=DWORD*; using LPBYTE=BYTE*;
using NET_API_STATUS=DWORD; using NTSTATUS=long;
#define TRUE 1
#define FALSE 0
#define VOID void
#define _In_
#define WINAPI
#define DPRINT(...)
#define DPRINT1(...)
#define STATUS_SUCCESS 0
#define STATUS_INVALID_PARAMETER -2
#define UNICODE_STRING_MAX_CHARS 32767
#define POLICY_VIEW_LOCAL_INFORMATION 1
#define POLICY_TRUST_ADMIN 2
#define PolicyAccountDomainInformation 5
#define SAM_SERVER_CONNECT 1
#define SAM_SERVER_LOOKUP_DOMAIN 2
#define DOMAIN_WRITE_OTHER_PARAMETERS 4
#define DomainNameInformation 5
#define MAX_PATH 260
#define MAX_COMPUTERNAME_LENGTH 15
#define ComputerNamePhysicalNetBIOS 1
#define ComputerNamePhysicalDnsHostname 2
#define SE_SHUTDOWN_NAME L"SeShutdownPrivilege"
#define ERROR_PRIVILEGE_NOT_HELD 1314
#define EWX_REBOOT 2
#define EWX_LOGOFF 0
#define SHTDN_REASON_MAJOR_OPERATINGSYSTEM 0x00020000
#define SHTDN_REASON_MINOR_RECONFIG 4
#define SHTDN_REASON_FLAG_PLANNED 0x80000000
#define NT_SUCCESS(s) ((s)>=0)
#define ERROR_SUCCESS 0
#define NERR_Success 0
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_WRITE_FAULT 29
#define ERROR_INVALID_STATE 5023
#define UNLEN 256
#define SECURITY_MAX_SID_SIZE 68
#define SID_MAX_SUB_AUTHORITIES 15
#define HKEY_LOCAL_MACHINE 1
#define KEY_SET_VALUE 2
#define REG_SZ 1
#define REG_BINARY 3
#define REG_DWORD 4
#define RRF_RT_REG_SZ 2
#define RRF_RT_REG_BINARY 8
#define UF_SCRIPT 1
#define UF_ACCOUNTDISABLE 2
#define UF_NORMAL_ACCOUNT 512
#define USER_PRIV_USER 1
#define OOBE_STATE L"SOFTWARE\\ReliefWin\\OOBE"
#define OOBE_RUN L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#define ZeroMemory(p,n) memset(p,0,n)
#define _wcsicmp wcscasecmp
#define WinBuiltinAdministratorsSid 1
#define WinBuiltinUsersSid 2
struct USER_INFO_1 { WCHAR* usri1_name; WCHAR* usri1_password; DWORD usri1_priv,usri1_flags; };
using PUSER_INFO_1=USER_INFO_1*;
struct USER_INFO_1003 { WCHAR* usri1003_password; };
struct USER_INFO_1008 { DWORD usri1008_flags; };
struct SETUPDATA { WCHAR UserName[257],Password[257],ComputerName[16]; BOOL bMustReboot; } SetupData;
using PSETUPDATA=SETUPDATA*;
struct User { DWORD flags; std::wstring password; bool admin=false,users=false; DWORD sid; };
struct Value { DWORD type; std::vector<BYTE> bytes; };
std::map<std::wstring,User> Users;
std::map<std::wstring,Value> Registry;
std::vector<std::wstring> Writes;
int Call=0, Fail=0, Checks=0;
DWORD LastError=0;
struct UNICODE_STRING { USHORT Length,MaximumLength; LPWSTR Buffer; };
struct POLICY_ACCOUNT_DOMAIN_INFO { UNICODE_STRING DomainName; PSID DomainSid; };
using PPOLICY_ACCOUNT_DOMAIN_INFO=POLICY_ACCOUNT_DOMAIN_INFO*;
struct LSA_OBJECT_ATTRIBUTES { size_t Length; };
using LSA_HANDLE=void*; using SAM_HANDLE=void*;
struct DOMAIN_NAME_INFORMATION { UNICODE_STRING DomainName; };
bool DomainInfoAlive=false,SamCalled=false;
NTSTATUS PolicySetStatus=0;
POLICY_ACCOUNT_DOMAIN_INFO DomainInfo;
NTSTATUS LsaOpenPolicy(void*,LSA_OBJECT_ATTRIBUTES*,int,LSA_HANDLE* handle) {*handle=(void*)1;return 0;}
NTSTATUS LsaQueryInformationPolicy(LSA_HANDLE,int,void** result) {DomainInfoAlive=true;DomainInfo.DomainSid=&DomainInfo;*result=&DomainInfo;return 0;}
NTSTATUS LsaSetInformationPolicy(LSA_HANDLE,int,void*) {return PolicySetStatus;}
void LsaFreeMemory(void*) {DomainInfoAlive=false;}
void LsaClose(LSA_HANDLE) {}
NTSTATUS SamConnect(void*,SAM_HANDLE* handle,int,void*) {*handle=(void*)1;SamCalled=true;return 0;}
NTSTATUS SamOpenDomain(SAM_HANDLE,int,PSID sid,SAM_HANDLE* handle) {*handle=(void*)2;return sid==&DomainInfo&&!DomainInfoAlive ? -3 : 0;}
NTSTATUS SamSetInformationDomain(SAM_HANDLE,int,void*) {return 0;}
void SamCloseHandle(SAM_HANDLE) {}
DWORD ExitFlags;
DWORD step();
BOOL EnableTokenPrivilege(PCWSTR) {return !step();}
BOOL ExitWindowsEx(DWORD flags,DWORD) {ExitFlags=flags;auto e=step();LastError=e;return !e;}
BOOL GetComputerNameExW(int,WCHAR* name,PDWORD) {wcscpy(name,L"RELIEFWIN");return TRUE;}
BOOL SetComputerNameExW(int,PCWSTR) {auto e=step();LastError=e;return !e;}
NTSTATUS SetAccountsDomainSid(PSID,LPCWSTR);
DWORD RtlNtStatusToDosError(NTSTATUS) {return ERROR_WRITE_FAULT;}

DWORD step() { return ++Call==Fail ? ERROR_WRITE_FAULT : 0; }
void check(bool value,const char* msg) { ++Checks; if(!value) { std::cerr<<msg<<" call="<<Call<<" fail="<<Fail<<"\n"; std::exit(1); } }
std::wstring keyname(HKEY key,PCWSTR name) { return std::to_wstring(key)+L"/"+name; }
DWORD RegOpenKeyExW(HKEY,PCWSTR path,DWORD,DWORD,HKEY* key) {
 auto e=step(); if(e) return e; *key=wcscmp(path,OOBE_STATE)==0?2:wcscmp(path,OOBE_RUN)==0?3:4; return 0;
}
DWORD RegCreateKeyExW(HKEY root,PCWSTR p,DWORD,WCHAR*,DWORD,DWORD access,void*,HKEY* k,void*) {return RegOpenKeyExW(root,p,0,access,k);}
void RegCloseKey(HKEY) {}
DWORD RegSetValueExW(HKEY key,PCWSTR name,DWORD,DWORD type,const BYTE* data,DWORD size) {
 auto e=step(); if(e)return e; Registry[keyname(key,name)]={type,std::vector<BYTE>(data,data+size)}; Writes.push_back(name);return 0;
}
DWORD RegDeleteValueW(HKEY key,PCWSTR name) {
 auto e=step(); if(e)return e; return Registry.erase(keyname(key,name))?0:ERROR_FILE_NOT_FOUND;
}
DWORD RegDeleteKeyValueW(HKEY,PCWSTR,PCWSTR name) { Registry.erase(keyname(2,name));return 0; }
DWORD RegGetValueW(HKEY,PCWSTR path,PCWSTR name,DWORD,void*,void* data,PDWORD size) {
 HKEY key=wcscmp(path,OOBE_STATE)==0?2:3; auto found=Registry.find(keyname(key,name));
 if(found==Registry.end())return ERROR_FILE_NOT_FOUND;
 if(*size<found->second.bytes.size()) return 234;
 *size=found->second.bytes.size(); memcpy(data,found->second.bytes.data(),*size);return 0;
}
DWORD RegFlushKey(HKEY) {return step();}
DWORD GetLastError() {return LastError;}
BOOL GetUserNameW(WCHAR* data,PDWORD) {wcscpy(data,L"Administrator");return 1;}
BOOL GetBuiltinAdministratorName(WCHAR* data,DWORD) {wcscpy(data,L"Administrator");return 1;}
DWORD GetModuleFileNameW(void*,WCHAR* data,DWORD) {wcscpy(data,L"C:\\Windows\\system32\\oobe\\oobe.exe");return wcslen(data);}
BOOL IsValidSid(void* sid) {return ((BYTE*)sid)[0]==1 && ((BYTE*)sid)[1]==1;}
DWORD GetLengthSid(void*) {return 12;}
DWORD GetSidLengthRequired(BYTE count) {return 8+4*count;}
BOOL EqualSid(void*a,void*b) {return !memcmp(a,b,12);}
DWORD AccountSid(PCWSTR name,PBYTE sid,PDWORD size) {
 auto e=step(); if(e)return e; auto found=Users.find(name);if(found==Users.end())return 2221;
 memset(sid,0,12);sid[0]=1;sid[1]=1;memcpy(sid+8,&found->second.sid,4);*size=12;return 0;
}
DWORD NetUserAdd(void*,DWORD,LPBYTE bytes,void*) {
 auto e=step();if(e)return e;auto data=(USER_INFO_1*)bytes;if(Users.count(data->usri1_name))return 2224;
 Users[data->usri1_name]={data->usri1_flags,data->usri1_password,false,false,1001};return 0;
}
DWORD NetUserDel(void*,PCWSTR name) {auto e=step();if(e)return e;Users.erase(name);return 0;}
DWORD NetUserGetInfo(void*,PCWSTR name,DWORD,LPBYTE* result) {
 auto e=step();if(e)return e;if(!Users.count(name))return 2221;
 static USER_INFO_1 data;data.usri1_flags=Users[name].flags;*result=(LPBYTE)&data;return 0;
}
void NetApiBufferFree(void*) {}
DWORD NetUserSetInfo(void*,PCWSTR name,DWORD level,LPBYTE bytes,void*) {
 auto e=step();if(e)return e;if(!Users.count(name))return 2221;
 if(level==1008)Users[name].flags=((USER_INFO_1008*)bytes)->usri1008_flags;
 else Users[name].password=((USER_INFO_1003*)bytes)->usri1003_password;return 0;
}
DWORD AddToLocalGroup(int group,void* sid) {
 auto e=step();if(e)return e;DWORD id;memcpy(&id,(BYTE*)sid+8,4);
 for(auto& [name,user]:Users) if(user.sid==id){if(group==1)user.admin=true;else user.users=true;return 0;}return 2221;
}
void reset() {
 Users.clear();Registry.clear();Writes.clear();Call=Fail=0;SetupData.bMustReboot=FALSE;
 Users[L"Administrator"]={UF_NORMAL_ACCOUNT,L"",true,true,500};
 wcscpy(SetupData.UserName,L"ReliefTester");wcscpy(SetupData.Password,L"Oobe-Test-2026");wcscpy(SetupData.ComputerName,L"RELIEFWIN");
 const WCHAR empty[]=L"",pending[]=L"C:\\Windows\\system32\\oobe\\oobe.exe";
 Registry[keyname(4,L"DefaultPassword")]={REG_SZ,std::vector<BYTE>((BYTE*)empty,(BYTE*)empty+sizeof(empty))};
 Registry[keyname(3,L"OOBE")]={REG_SZ,std::vector<BYTE>((BYTE*)pending,(BYTE*)pending+sizeof(pending))};
}
'''
MAIN = r'''
int main() {
 PolicySetStatus=0;SamCalled=false;
 check(SetAccountsDomainSid(nullptr,L"RELIEF-DESKTOP")==0,"account domain SID remains alive during rename");
 check(!DomainInfoAlive&&SamCalled,"domain rename cleans up after SAM call");
 PolicySetStatus=-4;SamCalled=false;
 check(SetAccountsDomainSid(nullptr,L"RELIEF-DESKTOP")==-4,"policy rename failure is preserved");
 check(!DomainInfoAlive&&!SamCalled,"failed policy rename cleans up without SAM rename");
 PolicySetStatus=0;

 reset();check(ApplyComputerName(&SetupData)==0&&!SetupData.bMustReboot,"unchanged name needs no reboot");
 wcscpy(SetupData.ComputerName,L"RELIEF-DESKTOP");
 check(ApplyComputerName(&SetupData)==0&&SetupData.bMustReboot,"renamed device requires reboot");
 reset();wcscpy(SetupData.ComputerName,L"RELIEF-DESKTOP");Fail=1;
 check(ApplyComputerName(&SetupData)!=0&&!SetupData.bMustReboot,"failed rename is propagated");

 reset();check(OobeFinishSession(FALSE)==0&&ExitFlags==EWX_LOGOFF,"unchanged device signs out");
 reset();check(OobeFinishSession(TRUE)==0&&ExitFlags==EWX_REBOOT,"renamed device restarts");
 reset();Fail=1;check(OobeFinishSession(TRUE)==ERROR_PRIVILEGE_NOT_HELD,"restart privilege failure propagated");
 reset();Fail=2;check(OobeFinishSession(TRUE)==ERROR_WRITE_FAULT,"restart failure propagated");
 reset();check(OobePending(),"pending setup is detected");
 check(CreateAccount(&SetupData)==0,"create account"); int operations=Call;
 check(Users[L"ReliefTester"].admin&&Users[L"ReliefTester"].users,"both groups assigned");
 check(!(Users[L"ReliefTester"].flags&UF_ACCOUNTDISABLE),"new account enabled");
 check(!Registry.count(keyname(4,L"DefaultPassword")),"no plaintext auto-logon password");
 check(std::find(Writes.begin(),Writes.end(),L"DefaultPassword")==Writes.end(),"password is never persisted for auto-logon");
 check(OobeOwnsAccount(L"ReliefTester"),"created SID recorded");
 auto validSid=Registry[keyname(2,L"AccountSid")];
 Registry[keyname(2,L"AccountSid")].bytes.resize(8);
 check(!OobeOwnsAccount(L"ReliefTester"),"truncated SID rejected");
 Registry[keyname(2,L"AccountSid")]=validSid;
 Registry[keyname(2,L"AccountSid")].bytes[1]=255;
 check(!OobeOwnsAccount(L"ReliefTester"),"invalid subauthority count rejected");
 Registry[keyname(2,L"AccountSid")]=validSid;
 wcscpy(SetupData.Password,L"Retry-2026");check(CreateAccount(&SetupData)==0,"owned account can resume");
 check(Users[L"ReliefTester"].password==L"Retry-2026","retry sets supplied password");
 Users[L"ReliefTester"].sid=2001;check(!OobeOwnsAccount(L"ReliefTester"),"replaced account is not owned");
 check(CreateAccount(&SetupData)==2224,"foreign account cannot be overwritten");
 for(int fail=1;fail<=operations;++fail) {
  reset();Fail=fail;auto error=CreateAccount(&SetupData);
  check(error!=0,"injected account failure propagated");
  check(OobePending(),"failed account creation retains run marker");
  if(Users.count(L"ReliefTester")) check(OobeOwnsAccount(L"ReliefTester"),"retained account has resumable SID");
  check(Users[L"Administrator"].flags==UF_NORMAL_ACCOUNT,"setup administrator untouched by failed apply");
 }
 reset();check(CreateAccount(&SetupData)==0,"setup completion fixture");Call=0;
 check(OobeComplete(FALSE)==0,"complete setup");operations=Call;
 check(!OobePending(),"only successful completion removes run marker");
 check(Users[L"Administrator"].flags&UF_ACCOUNTDISABLE,"temporary builtin administrator disabled");
 check(Registry.count(keyname(2,L"Completed")),"completion record written");
 for(int fail=1;fail<=operations;++fail) {
  reset();check(CreateAccount(&SetupData)==0,"completion retry fixture");Call=0;Fail=fail;
  check(OobeComplete(FALSE)!=0,"completion failure propagated");
  check(OobePending(),"completion failure retains retry entry");
  check(Users[L"Administrator"].flags==UF_NORMAL_ACCOUNT,"completion failure restores setup account");
  Fail=0;check(OobeComplete(FALSE)==0,"completion retry succeeds");
 }
 reset();check(OobeComplete(FALSE)==ERROR_INVALID_STATE,"completion without owned account rejected");
 check(OobePending(),"invalid completion retains run marker");
 reset();check(OobeComplete(TRUE)==0,"component-only completion does not create account");
 check(Users[L"Administrator"].flags==UF_NORMAL_ACCOUNT,"component-only mode retains account");
 std::cout<<Checks<<" OOBE account and completion checks passed\n";
}
'''


def main():
    source=SOURCE.read_text()
    domain=extract((ROOT/'dll/win32/syssetup/security.c').read_text(),'SetAccountsDomainSid','NTSTATUS WINAPI')
    functions=domain+''.join(extract(source,name,result) for name,result in [
        ('ApplyComputerName','static DWORD'),('SetStringValue','static DWORD'),('WriteLogonSettings','static DWORD'),
        ('OobeOwnsAccount','BOOL'),('CreateAccount','static DWORD'),
        ('OobePending','BOOL'),('OobeComplete','DWORD'),('OobeFinishSession','DWORD')])
    with tempfile.TemporaryDirectory(prefix='reliefwin-oobe-') as temp:
        cpp=pathlib.Path(temp)/'transactions.cpp';binary=pathlib.Path(temp)/'transactions'
        cpp.write_text(PREFIX+functions+MAIN)
        subprocess.run(['c++','-std=c++17','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',str(cpp),'-o',str(binary)],check=True)
        subprocess.run([str(binary)],check=True)

if __name__=='__main__':main()
