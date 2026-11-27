/*
 * PROJECT:     ReactOS Kernel Security Support Provider Interface Driver
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Random number generation and entropy gathering
 * COPYRIGHT:   Copyright 2014-2017 Timo Kreuzer <timo.kreuzer@reactos.org>
 */

#include "ksecdd.h"
#include <drbg.h>

static KSPIN_LOCK KsecRandomLock;
static RW_DRBG KsecRandomState;
RW_BOOT_ENTROPY ReliefWinBootEntropy =
{
    RW_ENTROPY_MAGIC, RW_ENTROPY_VERSION, sizeof(RW_BOOT_ENTROPY), 0, {0}
};

VOID NTAPI KsecInitializeRandomSupport(VOID)
{
    KeInitializeSpinLock(&KsecRandomLock);
    RwDrbgInitialize(&KsecRandomState, &ReliefWinBootEntropy);
}

NTSTATUS NTAPI KsecGenRandom(PVOID Buffer, SIZE_T Length)
{
    UCHAR Block[RW_DRBG_BLOCK_SIZE];
    PUCHAR Destination = Buffer;
    SIZE_T Chunk;
    KIRQL OldIrql;
    BOOLEAN Success;
    if (!Length) return STATUS_SUCCESS;
    if (!Buffer) return STATUS_INVALID_PARAMETER;
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    while (Length)
    {
        Chunk = min(Length, sizeof(Block));
        KeAcquireSpinLock(&KsecRandomLock, &OldIrql);
        Success = RwDrbgGenerate(&KsecRandomState, Block, Chunk);
        KeReleaseSpinLock(&KsecRandomLock, OldIrql);
        if (!Success)
        {
            RtlSecureZeroMemory(Block, sizeof(Block));
            return STATUS_DEVICE_NOT_READY;
        }
        RtlCopyMemory(Destination, Block, Chunk);
        RtlSecureZeroMemory(Block, sizeof(Block));
        Destination += Chunk;
        Length -= Chunk;
    }
    return STATUS_SUCCESS;
}
