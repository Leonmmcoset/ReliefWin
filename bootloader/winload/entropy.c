/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Pass firmware or CPU entropy directly to the boot security driver
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "winload.h"
#include <reliefwin/bootentropy.h>

VOID OslEntropyHandoff(PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    RW_BOOT_ENTROPY *Entropy;
    PKLDR_DATA_TABLE_ENTRY Driver;
    if (!PeLdrCheckForLoadedDll(&LoaderBlock->LoadOrderListHead, "ksecdd.sys", &Driver)) return;
    Entropy = PeGetExport(&LoaderBlock->LoadOrderListHead, Driver, "ReliefWinBootEntropy");
    if (!Entropy) return;
    Entropy = VaToPa(Entropy);
    if (Entropy->Magic != RW_ENTROPY_MAGIC || Entropy->Version != RW_ENTROPY_VERSION ||
        Entropy->Size != sizeof(*Entropy)) return;
    Entropy->Source = 0;
    if (FwGetEntropy(Entropy->Seed, sizeof(Entropy->Seed)))
        Entropy->Source = RW_ENTROPY_FIRMWARE;
    else if (FwArchGetEntropy(Entropy->Seed, sizeof(Entropy->Seed)))
        Entropy->Source = RW_ENTROPY_CPU;
    else
        RtlSecureZeroMemory(Entropy->Seed, sizeof(Entropy->Seed));
    OslTrace("Cryptographic boot entropy source: %lu", (ULONG)Entropy->Source);
}
