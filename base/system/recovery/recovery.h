/*
 * PROJECT:     ReliefWin Recovery Center
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Offline recovery operations
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <windows.h>

#define RC_DRIVER_NAME 256
#define RC_BACKUP_VALUE L"ReliefWinRecoveryOriginalStart"
#define RC_BCD_MANAGER L"{9dea862c-5cdd-4e70-acc1-f32b344d4795}"

typedef struct _RC_DRIVER
{
    WCHAR Name[RC_DRIVER_NAME];
    WCHAR Display[RC_DRIVER_NAME];
    DWORD Start;
    DWORD ErrorControl;
    BOOL Protected;
    BOOL CanUndo;
} RC_DRIVER;

DWORD RcOpenSystem(PCWSTR Windows, BOOL Write, PHKEY Hive, PHKEY Services);
DWORD RcReadDriver(HKEY Services, PCWSTR Name, RC_DRIVER *Driver);
DWORD RcChangeDriver(HKEY Services, PCWSTR Name, BOOL Undo);
DWORD RcValidateBcd(PCWSTR Path);
DWORD RcBackupBcd(PCWSTR Path, PCWSTR Backup);
DWORD RcRestoreBcd(PCWSTR Path, PCWSTR Backup);
DWORD RcReadLog(PCWSTR Path, PWSTR *Text, BOOL *Truncated);

