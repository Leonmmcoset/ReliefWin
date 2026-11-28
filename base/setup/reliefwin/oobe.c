/*
 * PROJECT:     ReliefWin Setup
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     First-boot configuration, account transaction and completion state
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefwin.h"
#include <winnls.h>
#include <tzlib.h>
#include <objbase.h>
#include <lm.h>
#include <ntsam.h>
#include <userenv.h>
#include <shellapi.h>
#include <winioctl.h>
#include <reliefwin/drivers/volumecrypt.h>
#include "oobe.h"
#include "resource.h"

#define NDEBUG
#include <debug.h>

#define OOBE_DEFAULT_HIVE L"ReliefWin.FirstBoot.Default"
#define OOBE_STATE L"SOFTWARE\\ReliefWin\\OOBE"
#define OOBE_RUN L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"

NTSTATUS WINAPI SetAccountsDomainSid(PSID DomainSid, LPCWSTR DomainName);
HANDLE ProcessHeap;
SETUPDATA SetupData;

VOID
SetSetupLanguage(PCWSTR LocaleId)
{
    SetThreadUILanguage(LANGIDFROMLCID(wcstoul(LocaleId, NULL, 16)));
}

VOID
LoadPrintfW(PWSTR Text, SIZE_T Count, UINT Id, ...)
{
    WCHAR Format[512];
    va_list Args;
    Text[0] = 0;
    if (!LoadStringW(GetModuleHandleW(NULL), Id, Format, ARRAYSIZE(Format)))
        return;
    va_start(Args, Id);
    StringCchVPrintfW(Text, Count, Format, Args);
    va_end(Args);
}

typedef struct _CHOICE_LIST
{
    PSETUP_CHOICE Items;
    ULONG Count;
} CHOICE_LIST, *PCHOICE_LIST;

static CHOICE_LIST LocaleList;

static LANGID
GetInitialSetupLanguage(VOID)
{
    LANGID LanguageId = GetUserDefaultUILanguage();

    if (LanguageId == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) ||
        LanguageId == MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT) ||
        LanguageId == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED))
    {
        return LanguageId;
    }

    return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
}

static BOOL
AddChoice(
    _Inout_ PCHOICE_LIST List,
    _In_ PCWSTR Id,
    _In_ PCWSTR Name)
{
    PSETUP_CHOICE Items;

    Items = List->Items ? HeapReAlloc(ProcessHeap, 0, List->Items, (List->Count + 1) * sizeof(*Items))
                        : HeapAlloc(ProcessHeap, 0, sizeof(*Items));
    if (!Items)
        return FALSE;

    StringCchCopyW(Items[List->Count].Id, ARRAYSIZE(Items[List->Count].Id), Id);
    StringCchCopyW(Items[List->Count].Name, ARRAYSIZE(Items[List->Count].Name), Name);
    List->Items = Items;
    List->Count++;
    return TRUE;
}

static BOOL
CALLBACK
AddLocaleChoice(
    LPWSTR LocaleString)
{
    WCHAR Name[128];
    LCID Lcid = wcstoul(LocaleString, NULL, 16);

    if (Lcid != MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT) &&
        Lcid != MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), SORT_DEFAULT) &&
        Lcid != MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT))
    {
        return TRUE;
    }

    if (GetLocaleInfoW(Lcid, LOCALE_SLOCALIZEDDISPLAYNAME, Name, ARRAYSIZE(Name)))
        AddChoice(&LocaleList, LocaleString, Name);
    return TRUE;
}

VOID
LoadLanguageChoices(
    _Inout_ PSETUPDATA pSetupData)
{
    CHOICE_LIST Layouts = { NULL, 0 };
    WCHAR Klid[KL_NAMELENGTH];
    WCHAR Name[128];
    DWORD cchKlid, cbName, cbLoaded, Index;
    HKEY hLayouts, hLayout;

    if (pSetupData->Locales)
        return;

    EnumSystemLocalesW(AddLocaleChoice, LCID_INSTALLED);
    pSetupData->Locales = LocaleList.Items;
    pSetupData->LocaleCount = LocaleList.Count;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
                      0, KEY_ENUMERATE_SUB_KEYS, &hLayouts) != ERROR_SUCCESS)
    {
        return;
    }

    for (Index = 0; ; Index++)
    {
        cchKlid = ARRAYSIZE(Klid);
        if (RegEnumKeyExW(hLayouts, Index, Klid, &cchKlid, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            break;
        if (RegOpenKeyExW(hLayouts, Klid, 0, KEY_QUERY_VALUE, &hLayout) != ERROR_SUCCESS)
            continue;
        cbName = sizeof(Name);
        if (RegLoadMUIStringW(hLayout, L"Layout Display Name", Name, sizeof(Name), &cbLoaded, 0, NULL) == ERROR_SUCCESS ||
            RegGetValueW(hLayout, NULL, L"Layout Text", RRF_RT_REG_SZ, NULL, Name, &cbName) == ERROR_SUCCESS)
        {
            AddChoice(&Layouts, Klid, Name);
        }
        RegCloseKey(hLayout);
    }
    RegCloseKey(hLayouts);

    pSetupData->Layouts = Layouts.Items;
    pSetupData->LayoutCount = Layouts.Count;
}

VOID
ApplyLanguageChoice(
    _Inout_ PSETUPDATA pSetupData,
    _In_ PCWSTR LocaleId,
    _In_ PCWSTR LayoutId)
{
    WCHAR Locale[ARRAYSIZE(pSetupData->DefaultLanguage)];
    WCHAR Layout[ARRAYSIZE(pSetupData->DefaultKBLayout)];

    StringCchCopyW(Locale, ARRAYSIZE(Locale), LocaleId);
    StringCchCopyW(Layout, ARRAYSIZE(Layout), LayoutId);
    StringCchCopyW(pSetupData->DefaultLanguage, ARRAYSIZE(pSetupData->DefaultLanguage), Locale);
    StringCchCopyW(pSetupData->DefaultKBLayout, ARRAYSIZE(pSetupData->DefaultKBLayout), Layout);
}

static BOOL
EnableTokenPrivilege(
    _In_ PCWSTR Privilege)
{
    TOKEN_PRIVILEGES Privileges;
    HANDLE hToken;
    BOOL Success;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        return FALSE;

    Privileges.PrivilegeCount = 1;
    Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    Success = LookupPrivilegeValueW(NULL, Privilege, &Privileges.Privileges[0].Luid) &&
              AdjustTokenPrivileges(hToken, FALSE, &Privileges, 0, NULL, NULL) &&
              GetLastError() == ERROR_SUCCESS;
    CloseHandle(hToken);
    return Success;
}

static DWORD
SetStringValue(HKEY Key, PCWSTR Name, PCWSTR Value)
{
    return RegSetValueExW(Key, Name, 0, REG_SZ, (const BYTE*)Value,
                          (DWORD)((wcslen(Value) + 1) * sizeof(WCHAR)));
}

static DWORD
WriteUserRegion(HKEY User, LCID Lcid, PCWSTR Layout)
{
    static const struct
    {
        LCTYPE Type;
        PCWSTR Name;
    } Values[] =
    {
        { LOCALE_SNAME, L"LocaleName" },
        { LOCALE_SDECIMAL, L"sDecimal" },
        { LOCALE_STHOUSAND, L"sThousand" },
        { LOCALE_SNEGATIVESIGN, L"sNegativeSign" },
        { LOCALE_SPOSITIVESIGN, L"sPositiveSign" },
        { LOCALE_SGROUPING, L"sGrouping" },
        { LOCALE_SLIST, L"sList" },
        { LOCALE_SNATIVEDIGITS, L"sNativeDigits" },
        { LOCALE_INEGNUMBER, L"iNegNumber" },
        { LOCALE_IDIGITS, L"iDigits" },
        { LOCALE_ILZERO, L"iLZero" },
        { LOCALE_IMEASURE, L"iMeasure" },
        { LOCALE_IDIGITSUBSTITUTION, L"NumShape" },
        { LOCALE_SCURRENCY, L"sCurrency" },
        { LOCALE_SMONDECIMALSEP, L"sMonDecimalSep" },
        { LOCALE_SMONTHOUSANDSEP, L"sMonThousandSep" },
        { LOCALE_SMONGROUPING, L"sMonGrouping" },
        { LOCALE_ICURRENCY, L"iCurrency" },
        { LOCALE_INEGCURR, L"iNegCurr" },
        { LOCALE_ICURRDIGITS, L"iCurrDigits" },
        { LOCALE_STIMEFORMAT, L"sTimeFormat" },
        { LOCALE_SSHORTTIME, L"sShortTime" },
        { LOCALE_STIME, L"sTime" },
        { LOCALE_S1159, L"s1159" },
        { LOCALE_S2359, L"s2359" },
        { LOCALE_ITIME, L"iTime" },
        { LOCALE_ITIMEMARKPOSN, L"iTimePrefix" },
        { LOCALE_ITLZERO, L"iTLZero" },
        { LOCALE_SLONGDATE, L"sLongDate" },
        { LOCALE_SSHORTDATE, L"sShortDate" },
        { LOCALE_SYEARMONTH, L"sYearMonth" },
        { LOCALE_SDATE, L"sDate" },
        { LOCALE_IFIRSTDAYOFWEEK, L"iFirstDayOfWeek" },
        { LOCALE_IFIRSTWEEKOFYEAR, L"iFirstWeekOfYear" },
        { LOCALE_IDATE, L"iDate" },
        { LOCALE_ICALENDARTYPE, L"iCalendarType" },
        { LOCALE_SCOUNTRY, L"sCountry" },
        { LOCALE_SABBREVLANGNAME, L"sLanguage" },
        { LOCALE_ICOUNTRY, L"iCountry" },
    };
    WCHAR Text[96];
    WCHAR Languages[8] = {0};
    HKEY Key;
    DWORD Error, Index;

    Error = RegCreateKeyExW(User, L"Control Panel\\International", 0, NULL, 0,
                            KEY_SET_VALUE, NULL, &Key, NULL);
    if (Error != ERROR_SUCCESS)
        return Error;
    StringCchPrintfW(Text, ARRAYSIZE(Text), L"%08lX", Lcid);
    Error = SetStringValue(Key, L"Locale", Text);
    for (Index = 0; Error == ERROR_SUCCESS && Index < ARRAYSIZE(Values); Index++)
    {
        if (!GetLocaleInfoW(Lcid, Values[Index].Type | LOCALE_NOUSEROVERRIDE, Text, ARRAYSIZE(Text)))
            Error = GetLastError();
        else
            Error = SetStringValue(Key, Values[Index].Name, Text);
    }
    RegCloseKey(Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegDeleteTreeW(User, L"Keyboard Layout\\Preload");
    if (Error != ERROR_SUCCESS && Error != ERROR_FILE_NOT_FOUND && Error != ERROR_PATH_NOT_FOUND)
        return Error;
    Error = RegCreateKeyExW(User, L"Keyboard Layout\\Preload", 0, NULL, 0, KEY_SET_VALUE, NULL, &Key, NULL);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = SetStringValue(Key, L"1", Layout);
    RegCloseKey(Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegCreateKeyExW(User, L"Control Panel\\Desktop", 0, NULL, 0, KEY_SET_VALUE, NULL, &Key, NULL);
    if (Error != ERROR_SUCCESS)
        return Error;
    if (!LCIDToLocaleName(Lcid, Languages, ARRAYSIZE(Languages) - 1, 0))
        Error = GetLastError();
    else
        Error = RegSetValueExW(Key, L"PreferredUILanguages", 0, REG_MULTI_SZ, (BYTE*)Languages,
                               (DWORD)((wcslen(Languages) + 2) * sizeof(WCHAR)));
    RegCloseKey(Key);
    return Error;
}

static DWORD
ApplyRegion(PSETUPDATA Data)
{
    LCID Lcid = wcstoul(Data->DefaultLanguage, NULL, 16);
    WCHAR Path[MAX_PATH], Text[16];
    DWORD Length = ARRAYSIZE(Path), Error, UnloadError;
    HKEY Key;

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Nls\\Language",
                          0, KEY_SET_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    StringCchPrintfW(Text, ARRAYSIZE(Text), L"%04X", LANGIDFROMLCID(Lcid));
    Error = SetStringValue(Key, L"InstallLanguage", Text);
    RegCloseKey(Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = WriteUserRegion(HKEY_CURRENT_USER, Lcid, Data->DefaultKBLayout);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegOpenKeyExW(HKEY_USERS, L".DEFAULT", 0, KEY_ALL_ACCESS, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = WriteUserRegion(Key, Lcid, Data->DefaultKBLayout);
    RegCloseKey(Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    if (!GetDefaultUserProfileDirectoryW(Path, &Length))
        return GetLastError();
    if (FAILED(StringCchCatW(Path, ARRAYSIZE(Path), L"\\NTUSER.DAT")))
        return ERROR_FILENAME_EXCED_RANGE;
    if (!EnableTokenPrivilege(SE_BACKUP_NAME) || !EnableTokenPrivilege(SE_RESTORE_NAME))
        return ERROR_PRIVILEGE_NOT_HELD;
    Error = RegLoadKeyW(HKEY_USERS, OOBE_DEFAULT_HIVE, Path);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = RegOpenKeyExW(HKEY_USERS, OOBE_DEFAULT_HIVE, 0, KEY_ALL_ACCESS, &Key);
    if (Error == ERROR_SUCCESS)
    {
        Error = WriteUserRegion(Key, Lcid, Data->DefaultKBLayout);
        RegCloseKey(Key);
    }
    UnloadError = RegUnLoadKeyW(HKEY_USERS, OOBE_DEFAULT_HIVE);
    return Error != ERROR_SUCCESS ? Error : UnloadError;
}

DWORD
OobeBootLanguage(PCWSTR Locale, BOOL Write)
{
    WCHAR Partition[MAX_PATH], Path[MAX_PATH + 64], Temp[MAX_PATH + 64];
    WCHAR Leaf[8];
    CHAR Tag[6] = {0};
    DWORD Size = sizeof(Partition), Error, Done = 0;
    HANDLE File;
    LANGID Id;

    Error = RegGetValueW(HKEY_LOCAL_MACHINE, L"SYSTEM\\Setup", L"SystemPartition",
                         RRF_RT_REG_SZ, NULL, Partition, &Size);
    if (Error != ERROR_SUCCESS)
        return Error;
    if (FAILED(StringCchPrintfW(Path, ARRAYSIZE(Path), L"\\\\?\\GLOBALROOT%s\\ReliefWin.bootlang", Partition)))
        return ERROR_FILENAME_EXCED_RANGE;
    if (!Write)
    {
        File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (File == INVALID_HANDLE_VALUE)
            return GetLastError();
        Error = ReadFile(File, Tag, sizeof(Tag), &Done, NULL) ? ERROR_SUCCESS : GetLastError();
        CloseHandle(File);
        if (Error != ERROR_SUCCESS || Done != 5)
            return Error != ERROR_SUCCESS ? Error : ERROR_INVALID_DATA;
        Id = !strcmp(Tag, "en-US") ? 0x0409 : !strcmp(Tag, "ja-JP") ? 0x0411 :
             !strcmp(Tag, "zh-CN") ? 0x0804 : 0;
        if (!Id)
            return ERROR_INVALID_DATA;
        StringCchPrintfW(SetupData.DefaultLanguage, ARRAYSIZE(SetupData.DefaultLanguage), L"%08x", Id);
        return ERROR_SUCCESS;
    }
    Id = LANGIDFROMLCID(wcstoul(Locale, NULL, 16));
    StringCchCopyW(Leaf, ARRAYSIZE(Leaf), Id == 0x0411 ? L"ja-JP" : Id == 0x0409 ? L"en-US" : L"zh-CN");
    if (!WideCharToMultiByte(CP_UTF8, 0, Leaf, -1, Tag, sizeof(Tag), NULL, NULL))
        return GetLastError();
    if (FAILED(StringCchPrintfW(Temp, ARRAYSIZE(Temp), L"%s.new", Path)))
        return ERROR_FILENAME_EXCED_RANGE;
    File = CreateFileW(Temp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if (File == INVALID_HANDLE_VALUE)
        return GetLastError();
    Error = ERROR_SUCCESS;
    if (!WriteFile(File, Tag, 5, &Done, NULL))
        Error = GetLastError();
    else if (Done != 5)
        Error = ERROR_WRITE_FAULT;
    else if (!FlushFileBuffers(File))
        Error = GetLastError();
    CloseHandle(File);
    if (Error == ERROR_SUCCESS && !MoveFileExW(Temp, Path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        Error = GetLastError();
    if (Error != ERROR_SUCCESS)
        DeleteFileW(Temp);
    return Error;
}

DWORD
OobeEncryptionState(PDWORD Flags)
{
    WCHAR Windows[MAX_PATH], Device[] = L"\\\\.\\C:";
    HANDLE Volume;
    DWORD Done, Error;
    *Flags = 0;
    if (GetWindowsDirectoryW(Windows, ARRAYSIZE(Windows)) < 3)
        return ERROR_PATH_NOT_FOUND;
    Device[4] = Windows[0];
    Volume = CreateFileW(Device, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (Volume == INVALID_HANDLE_VALUE)
        return GetLastError();
    Error = DeviceIoControl(Volume, RW_CRYPT_IOCTL_VOLUME_STATE, NULL, 0, Flags, sizeof(*Flags), &Done, NULL)
            ? (Done == sizeof(*Flags) ? ERROR_SUCCESS : ERROR_INVALID_DATA) : GetLastError();
    CloseHandle(Volume);
    return Error;
}

static DWORD
ApplyTimeZone(
    _In_ PSETUPDATA pSetupData)
{
    DYNAMIC_TIME_ZONE_INFORMATION Zone;
    REG_TZI_FORMAT ZoneInfo;
    WCHAR KeyName[128];
    ULONG StandardSize, DaylightSize, ZoneIndex;
    DWORD cchKeyName, Index;
    LONG Error;
    HKEY hZones, hZone;
    BOOL Found = FALSE;

    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones",
                          0, KEY_ENUMERATE_SUB_KEYS, &hZones);
    if (Error != ERROR_SUCCESS)
    {
        return Error;
    }

    for (Index = 0; !Found; Index++)
    {
        cchKeyName = ARRAYSIZE(KeyName);
        if (RegEnumKeyExW(hZones, Index, KeyName, &cchKeyName, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            break;
        if (RegOpenKeyExW(hZones, KeyName, 0, KEY_QUERY_VALUE, &hZone) != ERROR_SUCCESS)
            continue;

        ZeroMemory(&Zone, sizeof(Zone));
        StandardSize = sizeof(Zone.StandardName);
        DaylightSize = sizeof(Zone.DaylightName);
        if (QueryTimeZoneData(hZone, &ZoneIndex, &ZoneInfo, NULL, NULL,
                              Zone.StandardName, &StandardSize,
                              Zone.DaylightName, &DaylightSize) == ERROR_SUCCESS &&
            ZoneIndex == pSetupData->TimeZoneIndex)
        {
            Found = TRUE;
        }
        RegCloseKey(hZone);
    }
    RegCloseKey(hZones);

    if (!Found)
    {
        DPRINT1("Time zone index %lu not found\n", pSetupData->TimeZoneIndex);
        return ERROR_NOT_FOUND;
    }

    Zone.Bias = ZoneInfo.Bias;
    Zone.StandardBias = ZoneInfo.StandardBias;
    Zone.DaylightBias = ZoneInfo.DaylightBias;
    Zone.StandardDate = ZoneInfo.StandardDate;
    Zone.DaylightDate = ZoneInfo.DaylightDate;
    StringCchCopyW(Zone.TimeZoneKeyName, ARRAYSIZE(Zone.TimeZoneKeyName), KeyName);
    Zone.DynamicDaylightTimeDisabled = !pSetupData->AutoDaylight;

    EnableTokenPrivilege(SE_TIME_ZONE_NAME);
    if (!SetDynamicTimeZoneInformation(&Zone))
        return GetLastError();
    SetAutoDaylight(pSetupData->AutoDaylight);
    return ERROR_SUCCESS;
}

static DWORD
ApplyComputerName(
    _In_ PSETUPDATA pSetupData)
{
    WCHAR Current[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD cchCurrent = ARRAYSIZE(Current);
    NTSTATUS Status;

    if (GetComputerNameExW(ComputerNamePhysicalNetBIOS, Current, &cchCurrent) &&
        _wcsicmp(Current, pSetupData->ComputerName) == 0)
    {
        return ERROR_SUCCESS;
    }

    if (!SetComputerNameExW(ComputerNamePhysicalDnsHostname, pSetupData->ComputerName))
        return GetLastError();

    pSetupData->bMustReboot = TRUE;
    Status = SetAccountsDomainSid(NULL, pSetupData->ComputerName);
    return NT_SUCCESS(Status) ? ERROR_SUCCESS : RtlNtStatusToDosError(Status);
}

static DWORD
AddToLocalGroup(
    _In_ WELL_KNOWN_SID_TYPE GroupType,
    _In_ PSID UserSid)
{
    BYTE GroupSid[SECURITY_MAX_SID_SIZE];
    DWORD cbGroupSid = sizeof(GroupSid);
    WCHAR Group[GNLEN + 1], Domain[DNLEN + 1];
    DWORD cchGroup = ARRAYSIZE(Group), cchDomain = ARRAYSIZE(Domain);
    LOCALGROUP_MEMBERS_INFO_0 Member;
    SID_NAME_USE Use;
    NET_API_STATUS Status;

    if (!CreateWellKnownSid(GroupType, NULL, GroupSid, &cbGroupSid) ||
        !LookupAccountSidW(NULL, GroupSid, Group, &cchGroup, Domain, &cchDomain, &Use))
    {
        return GetLastError();
    }

    Member.lgrmi0_sid = UserSid;
    Status = NetLocalGroupAddMembers(NULL, Group, 0, (LPBYTE)&Member, 1);
    return (Status == ERROR_MEMBER_IN_ALIAS) ? NERR_Success : Status;
}

static DWORD
WriteLogonSettings(PSETUPDATA Data)
{
    HKEY Key;
    DWORD Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0, KEY_SET_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        return Error;
    Error = SetStringValue(Key, L"AutoAdminLogon", L"0");
    if (Error == ERROR_SUCCESS)
    {
        Error = RegDeleteValueW(Key, L"DefaultPassword");
        if (Error == ERROR_FILE_NOT_FOUND)
            Error = ERROR_SUCCESS;
    }
    if (Error == ERROR_SUCCESS)
        Error = SetStringValue(Key, L"DefaultUserName", Data->UserName);
    if (Error == ERROR_SUCCESS)
        Error = SetStringValue(Key, L"DefaultDomainName", Data->ComputerName);
    RegCloseKey(Key);
    return Error;
}

static DWORD
AccountSid(PCWSTR Name, PBYTE Sid, PDWORD Size)
{
    WCHAR Domain[DNLEN + 1];
    DWORD Count = ARRAYSIZE(Domain);
    SID_NAME_USE Use;
    if (!LookupAccountNameW(NULL, Name, Sid, Size, Domain, &Count, &Use))
        return GetLastError();
    return Use == SidTypeUser ? ERROR_SUCCESS : ERROR_NONE_MAPPED;
}

BOOL
OobeOwnsAccount(PCWSTR Name)
{
    BYTE Sid[SECURITY_MAX_SID_SIZE], Saved[SECURITY_MAX_SID_SIZE] = {0};
    DWORD Size = sizeof(Sid), SavedSize = sizeof(Saved);
    WCHAR SavedName[UNLEN + 1];
    DWORD NameSize = sizeof(SavedName);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, OOBE_STATE, L"AccountName", RRF_RT_REG_SZ,
                     NULL, SavedName, &NameSize) != ERROR_SUCCESS || _wcsicmp(Name, SavedName) != 0 ||
        RegGetValueW(HKEY_LOCAL_MACHINE, OOBE_STATE, L"AccountSid", RRF_RT_REG_BINARY,
                     NULL, Saved, &SavedSize) != ERROR_SUCCESS ||
        SavedSize < 8 || Saved[1] > SID_MAX_SUB_AUTHORITIES ||
        GetSidLengthRequired(Saved[1]) != SavedSize || !IsValidSid(Saved) || GetLengthSid(Saved) != SavedSize ||
        AccountSid(Name, Sid, &Size) != ERROR_SUCCESS)
        return FALSE;
    return EqualSid(Sid, Saved);
}

static DWORD
CreateAccount(PSETUPDATA Data)
{
    USER_INFO_1 Info;
    USER_INFO_1003 Password;
    USER_INFO_1008 Flags;
    BYTE Sid[SECURITY_MAX_SID_SIZE];
    DWORD Size = sizeof(Sid), Error;
    HKEY Key;
    BOOL Created = FALSE;

    if (!OobeOwnsAccount(Data->UserName))
    {
        ZeroMemory(&Info, sizeof(Info));
        Info.usri1_name = Data->UserName;
        Info.usri1_password = Data->Password;
        Info.usri1_priv = USER_PRIV_USER;
        Info.usri1_flags = UF_SCRIPT | UF_NORMAL_ACCOUNT | UF_ACCOUNTDISABLE;
        Error = NetUserAdd(NULL, 1, (LPBYTE)&Info, NULL);
        if (Error != NERR_Success)
            return Error;
        Created = TRUE;
        Error = AccountSid(Data->UserName, Sid, &Size);
        if (Error != ERROR_SUCCESS)
            goto Failure;
        Error = RegCreateKeyExW(HKEY_LOCAL_MACHINE, OOBE_STATE, 0, NULL, 0,
                                KEY_SET_VALUE, NULL, &Key, NULL);
        if (Error != ERROR_SUCCESS)
            goto Failure;
        Error = SetStringValue(Key, L"AccountName", Data->UserName);
        if (Error == ERROR_SUCCESS)
            Error = RegSetValueExW(Key, L"AccountSid", 0, REG_BINARY, Sid, Size);
        if (Error == ERROR_SUCCESS)
            Error = RegFlushKey(Key);
        RegCloseKey(Key);
        if (Error != ERROR_SUCCESS)
            goto Failure;
    }
    else
    {
        Error = AccountSid(Data->UserName, Sid, &Size);
        if (Error != ERROR_SUCCESS)
            return Error;
        Password.usri1003_password = Data->Password;
        Error = NetUserSetInfo(NULL, Data->UserName, 1003, (LPBYTE)&Password, NULL);
        if (Error != NERR_Success)
            return Error;
    }
    Error = AddToLocalGroup(WinBuiltinAdministratorsSid, Sid);
    if (Error == ERROR_SUCCESS)
        Error = AddToLocalGroup(WinBuiltinUsersSid, Sid);
    if (Error != ERROR_SUCCESS)
        goto Failure;
    Flags.usri1008_flags = UF_SCRIPT | UF_NORMAL_ACCOUNT;
    Error = NetUserSetInfo(NULL, Data->UserName, 1008, (LPBYTE)&Flags, NULL);
    if (Error == NERR_Success)
        Error = WriteLogonSettings(Data);
    if (Error == ERROR_SUCCESS)
        return Error;
Failure:
    if (Created && NetUserDel(NULL, Data->UserName) == NERR_Success)
    {
        RegDeleteKeyValueW(HKEY_LOCAL_MACHINE, OOBE_STATE, L"AccountName");
        RegDeleteKeyValueW(HKEY_LOCAL_MACHINE, OOBE_STATE, L"AccountSid");
    }
    return Error;
}

DWORD
OobeApply(PSETUPDATA Data, POOBE_PROGRESS Progress, BOOL ComponentsOnly)
{
    DWORD Error;
    ULONG Index;
    HRESULT Com;
    if (!ComponentsOnly)
    {
        SetSetupLanguage(Data->DefaultLanguage);
        SetWindowTextW(Progress->hwndActivity, L"");
        Error = ApplyRegion(Data);
        if (Error == ERROR_SUCCESS)
            Error = ApplyTimeZone(Data);
        if (Error == ERROR_SUCCESS)
            Error = ApplyComputerName(Data);
        if (Error == ERROR_SUCCESS)
            Error = OobeBootLanguage(Data->DefaultLanguage, TRUE);
        if (Error == ERROR_SUCCESS)
            Error = CreateAccount(Data);
        SecureZeroMemory(Data->Password, sizeof(Data->Password));
        if (Error != ERROR_SUCCESS)
            return Error;
    }
    Com = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    for (Index = 0; Index < OobeAppCount; Index++)
    {
        if (OobeApps[Index].Selected)
        {
            Error = InstallOobeApp(&OobeApps[Index], Progress);
            if (Error != ERROR_SUCCESS)
            {
                if (SUCCEEDED(Com)) CoUninitialize();
                return Error;
            }
        }
    }
    if (SUCCEEDED(Com)) CoUninitialize();
    Data->bInstalled = TRUE;
    return ERROR_SUCCESS;
}

BOOL
OobePending(VOID)
{
    WCHAR Path[MAX_PATH + 32];
    DWORD Size = sizeof(Path);
    return RegGetValueW(HKEY_LOCAL_MACHINE, OOBE_RUN, L"OOBE", RRF_RT_REG_SZ,
                        NULL, Path, &Size) == ERROR_SUCCESS;
}

DWORD
OobeComplete(BOOL ComponentsOnly)
{
    HKEY Key;
    DWORD Error, Complete = 1;
    WCHAR Current[UNLEN + 1], Administrator[UNLEN + 1];
    DWORD Length = ARRAYSIZE(Current), OldFlags = 0;
    PUSER_INFO_1 Info;
    USER_INFO_1008 Flags;
    BOOL Disabled = FALSE;
    if (!ComponentsOnly)
    {
        if (!OobeOwnsAccount(SetupData.UserName))
            return ERROR_INVALID_STATE;
        if (!GetUserNameW(Current, &Length) ||
            !GetBuiltinAdministratorName(Administrator, ARRAYSIZE(Administrator)))
            return GetLastError();
        if (!_wcsicmp(Current, Administrator))
        {
            Error = NetUserGetInfo(NULL, Current, 1, (LPBYTE*)&Info);
            if (Error != NERR_Success)
                return Error;
            OldFlags = Info->usri1_flags;
            NetApiBufferFree(Info);
            Flags.usri1008_flags = OldFlags | UF_ACCOUNTDISABLE;
            Error = NetUserSetInfo(NULL, Current, 1008, (LPBYTE)&Flags, NULL);
            if (Error != NERR_Success)
                return Error;
            Disabled = TRUE;
        }
        Error = RegCreateKeyExW(HKEY_LOCAL_MACHINE, OOBE_STATE, 0, NULL, 0,
                                KEY_SET_VALUE, NULL, &Key, NULL);
        if (Error != ERROR_SUCCESS)
            goto Failure;
        Error = RegSetValueExW(Key, L"Completed", 0, REG_DWORD, (BYTE*)&Complete, sizeof(Complete));
        if (Error == ERROR_SUCCESS)
            Error = RegFlushKey(Key);
        RegCloseKey(Key);
        if (Error != ERROR_SUCCESS)
            goto Failure;
    }
    Error = RegOpenKeyExW(HKEY_LOCAL_MACHINE, OOBE_RUN, 0, KEY_SET_VALUE, &Key);
    if (Error != ERROR_SUCCESS)
        goto Failure;
    Error = RegDeleteValueW(Key, L"OOBE");
    if (Error == ERROR_FILE_NOT_FOUND)
        Error = ERROR_SUCCESS;
    if (Error == ERROR_SUCCESS)
        Error = RegFlushKey(Key);
    if (Error != ERROR_SUCCESS)
    {
        WCHAR Path[MAX_PATH];
        DWORD Count = GetModuleFileNameW(NULL, Path, ARRAYSIZE(Path));
        if (Count && Count < ARRAYSIZE(Path))
            SetStringValue(Key, L"OOBE", Path);
    }
    RegCloseKey(Key);
Failure:
    if (Error != ERROR_SUCCESS && Disabled)
    {
        Flags.usri1008_flags = OldFlags;
        NetUserSetInfo(NULL, Current, 1008, (LPBYTE)&Flags, NULL);
    }
    return Error;
}

DWORD
OobeFinishSession(BOOL Reboot)
{
    if (Reboot && !EnableTokenPrivilege(SE_SHUTDOWN_NAME))
        return ERROR_PRIVILEGE_NOT_HELD;
    if (!ExitWindowsEx(Reboot ? EWX_REBOOT : EWX_LOGOFF,
                       SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_RECONFIG | SHTDN_REASON_FLAG_PLANNED))
        return GetLastError();
    return ERROR_SUCCESS;
}

int WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE Previous, LPWSTR CommandLine, int Show)
{
    INITCOMMONCONTROLSEX Controls = {sizeof(Controls), ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS};
    BOOL Preview = FALSE, ComponentsOnly = FALSE, Pending;
    LPWSTR *Arguments;
    int Count, Index, Result;
    DWORD Size;
    HRESULT Com;
    HANDLE Mutex;
    UNREFERENCED_PARAMETER(Previous);
    UNREFERENCED_PARAMETER(CommandLine);
    Arguments = CommandLineToArgvW(GetCommandLineW(), &Count);
    if (!Arguments)
        return ERROR_INVALID_PARAMETER;
    for (Index = 1; Index < Count; Index++)
    {
        if (!_wcsicmp(Arguments[Index], L"/preview")) Preview = TRUE;
        else if (!_wcsicmp(Arguments[Index], L"/components")) ComponentsOnly = TRUE;
        else { LocalFree(Arguments); return ERROR_INVALID_PARAMETER; }
    }
    LocalFree(Arguments);
    Mutex = CreateMutexW(NULL, TRUE, L"Local\\ReliefWin.OOBE");
    if (!Mutex)
        return GetLastError();
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(Mutex);
        return ERROR_ALREADY_EXISTS;
    }
    Pending = OobePending();
    Preview = Preview || !Pending;
    ProcessHeap = GetProcessHeap();
    SetupData.hInstance = Instance;
    StringCchPrintfW(SetupData.DefaultLanguage, ARRAYSIZE(SetupData.DefaultLanguage), L"%08x", GetInitialSetupLanguage());
    OobeBootLanguage(NULL, FALSE);
    SetSetupLanguage(SetupData.DefaultLanguage);
    if (!GetKeyboardLayoutNameW(SetupData.DefaultKBLayout))
        StringCchCopyW(SetupData.DefaultKBLayout, ARRAYSIZE(SetupData.DefaultKBLayout), L"00000409");
    Size = ARRAYSIZE(SetupData.ComputerName);
    if (!GetComputerNameW(SetupData.ComputerName, &Size))
        GenerateComputerName(SetupData.ComputerName);
    Size = sizeof(SetupData.UserName);
    if (Pending && !Preview)
        RegGetValueW(HKEY_LOCAL_MACHINE, OOBE_STATE, L"AccountName", RRF_RT_REG_SZ, NULL, SetupData.UserName, &Size);
    SetupData.TimeZoneIndex = (ULONG)-1;
    GetTimeZoneListIndex(&SetupData.TimeZoneIndex);
    SetupData.AutoDaylight = TRUE;
    LoadLanguageChoices(&SetupData);
    OobeAppCount = LoadOobeApps(&OobeApps, ComponentsOnly);
    for (Index = 0; (ULONG)Index < OobeAppCount; Index++)
        OobeApps[Index].Selected = FALSE;
    OobeInstallDrivers = FALSE;
    InitCommonControlsEx(&Controls);
    Com = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    Result = RunOobeExperience(&SetupData, Preview, ComponentsOnly, Show);
    SecureZeroMemory(SetupData.Password, sizeof(SetupData.Password));
    if (SetupData.Locales) HeapFree(ProcessHeap, 0, SetupData.Locales);
    if (SetupData.Layouts) HeapFree(ProcessHeap, 0, SetupData.Layouts);
    if (OobeApps) HeapFree(ProcessHeap, 0, OobeApps);
    if (SUCCEEDED(Com)) CoUninitialize();
    ReleaseMutex(Mutex);
    CloseHandle(Mutex);
    return Result;
}
