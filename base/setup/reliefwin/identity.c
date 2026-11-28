/*
 * PROJECT:     ReliefWin Setup
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Account identity validation shared by setup and first boot
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefwin.h"
#include <ntsecapi.h>

VOID
GenerateComputerName(
    _Out_writes_(MAX_COMPUTERNAME_LENGTH + 1) PWSTR Buffer)
{
    static const WCHAR Chars[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    UINT i;

    srand(GetTickCount());
    StringCchCopyW(Buffer, MAX_COMPUTERNAME_LENGTH + 1, L"RELIEFWIN-");
    for (i = (UINT)wcslen(Buffer); i < MAX_COMPUTERNAME_LENGTH; i++)
        Buffer[i] = Chars[rand() % (ARRAYSIZE(Chars) - 1)];
    Buffer[MAX_COMPUTERNAME_LENGTH] = UNICODE_NULL;
}

BOOL
IsValidSetupComputerName(
    _In_ PCWSTR Name)
{
    SIZE_T Length = wcslen(Name);
    BOOL OnlyDigits = TRUE;
    PCWSTR p;

    if (Length == 0 || Length > MAX_COMPUTERNAME_LENGTH)
        return FALSE;

    for (p = Name; *p; p++)
    {
        if (!((*p >= L'A' && *p <= L'Z') || (*p >= L'a' && *p <= L'z') ||
              (*p >= L'0' && *p <= L'9') || *p == L'-'))
        {
            return FALSE;
        }
        if (*p < L'0' || *p > L'9')
            OnlyDigits = FALSE;
    }

    return !OnlyDigits;
}

BOOL
IsValidSetupUserName(
    _In_ PCWSTR Name)
{
    SIZE_T Length = wcslen(Name);
    BOOL OnlyDotsAndSpaces = TRUE;
    PCWSTR p;

    if (Length == 0 || Length > 20)
        return FALSE;

    for (p = Name; *p; p++)
    {
        if (*p < L' ' || wcschr(L"\"/\\[]:;|=,+*?<>@", *p))
            return FALSE;
        if (*p != L'.' && *p != L' ')
            OnlyDotsAndSpaces = FALSE;
    }

    return !OnlyDotsAndSpaces;
}

BOOL
GetBuiltinAdministratorName(
    _Out_writes_(cchName) PWSTR Name,
    _In_ DWORD cchName)
{
    PPOLICY_ACCOUNT_DOMAIN_INFO AccountDomain = NULL;
    LSA_OBJECT_ATTRIBUTES ObjectAttributes;
    LSA_HANDLE PolicyHandle;
    BYTE Sid[SECURITY_MAX_SID_SIZE];
    DWORD cbSid = sizeof(Sid);
    WCHAR Domain[MAX_PATH];
    DWORD cchDomain = ARRAYSIZE(Domain);
    SID_NAME_USE Use;
    NTSTATUS Status;
    BOOL Result;

    ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));
    ObjectAttributes.Length = sizeof(ObjectAttributes);
    Status = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_VIEW_LOCAL_INFORMATION, &PolicyHandle);
    if (NT_SUCCESS(Status))
    {
        Status = LsaQueryInformationPolicy(PolicyHandle, PolicyAccountDomainInformation, (PVOID*)&AccountDomain);
        LsaClose(PolicyHandle);
    }
    if (!NT_SUCCESS(Status))
    {
        SetLastError(LsaNtStatusToWinError(Status));
        return FALSE;
    }

    Result = CreateWellKnownSid(WinAccountAdministratorSid, AccountDomain->DomainSid, Sid, &cbSid) &&
             LookupAccountSidW(NULL, Sid, Name, &cchName, Domain, &cchDomain, &Use);
    LsaFreeMemory(AccountDomain);
    if (Result && Use != SidTypeUser)
    {
        SetLastError(ERROR_NO_SUCH_USER);
        return FALSE;
    }
    return Result;
}
