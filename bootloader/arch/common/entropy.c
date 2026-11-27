/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Fail closed where no CPU entropy interface is implemented
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bl.h>

BOOLEAN FwArchGetEntropy(PUCHAR Buffer, ULONG Size)
{
    RtlSecureZeroMemory(Buffer, Size);
    return FALSE;
}
