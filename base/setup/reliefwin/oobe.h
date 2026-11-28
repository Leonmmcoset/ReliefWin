/*
 * PROJECT:     ReliefWin Setup
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     First-boot setup declarations
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 */

#pragma once

typedef struct _OOBE_APP
{
    WCHAR Name[128];
    WCHAR Url[512];
    WCHAR File[MAX_PATH];
    WCHAR Arguments[256];
    WCHAR Signer[128];
    WCHAR Sha256[65];
    ULONG SpaceMb;
    BOOL Msi;
    BOOL Present;
    BOOL Unattended;
    BOOL Selected;
    DWORD Result;
} OOBE_APP, *POOBE_APP;

typedef struct _OOBE_PROGRESS
{
    HWND hwndActivity;
    HWND hwndItem;
    HWND hwndProgress;
} OOBE_PROGRESS, *POOBE_PROGRESS;

ULONG
LoadOobeApps(
    _Out_ POOBE_APP* Apps,
    _In_ BOOL IncludePresent);

BOOL
GetSystemDriveSpace(
    _Out_opt_ PULONGLONG TotalMb,
    _Out_ PULONGLONG FreeMb);

BOOL
WaitForInternet(VOID);

DWORD
InstallOobeApp(
    _Inout_ POOBE_APP App,
    _In_ POOBE_PROGRESS Progress);

VOID
OobeResultText(
    _In_ DWORD Result,
    _Out_writes_(cchMessage) PWSTR Message,
    _In_ SIZE_T cchMessage,
    _Out_writes_(cchCode) PWSTR Code,
    _In_ SIZE_T cchCode);

extern POOBE_APP OobeApps;
extern ULONG OobeAppCount;
extern BOOL OobeInstallDrivers;

DWORD OobeBootLanguage(PCWSTR Locale, BOOL Write);
DWORD OobeEncryptionState(PDWORD Flags);
BOOL OobeOwnsAccount(PCWSTR Name);
BOOL OobePending(VOID);
DWORD OobeApply(PSETUPDATA Data, POOBE_PROGRESS Progress, BOOL ComponentsOnly);
DWORD OobeComplete(BOOL ComponentsOnly);
DWORD OobeFinishSession(BOOL Reboot);
int RunOobeExperience(PSETUPDATA Data, BOOL Preview, BOOL ComponentsOnly, int Show);
