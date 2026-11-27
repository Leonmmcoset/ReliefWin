/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Unaligned encrypted boot-volume reads and writes
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bl.h>
#include <reliefwin/libs/volumecrypt/crypto.h>
#include <reliefwin/drivers/volumecrypt.h>

static DK_DEVICE CryptDevice;
static VC_META CryptMeta;
static UCHAR CryptKey[VC_KEY_SIZE];
static BOOLEAN CryptEnabled;

void
VcCryptoFatal(uint32_t Code)
{
    FwPrint(L"Disk encryption self-test failed (%08lx).\r\n", (ULONG)Code);
    FwHalt();
}

BOOLEAN
DkCryptActive(PDK_DEVICE Device)
{
    return CryptEnabled && Device->Firmware == CryptDevice.Firmware &&
           Device->DiskIndex == CryptDevice.DiskIndex &&
           Device->Memory == CryptDevice.Memory && Device->Offset == CryptDevice.Offset &&
           Device->Length == CryptDevice.Length;
}

BOOLEAN
DkCryptEnable(PDK_DEVICE Device, const VC_META *Meta, const UCHAR *Key)
{
    if (CryptEnabled || Meta->Incomplete || Meta->PayloadOffset || Device->Memory ||
        Device->Length % 512 || (Meta->PayloadSize && Meta->PayloadSize != Device->Length))
        return FALSE;
    CryptDevice = *Device;
    CryptMeta = *Meta;
    RtlCopyMemory(CryptKey, Key, sizeof(CryptKey));
    CryptEnabled = TRUE;
    return TRUE;
}

VOID
DkCryptForget(VOID)
{
    CryptEnabled = FALSE;
    VcWipe(CryptKey, sizeof(CryptKey));
    VcWipe(&CryptMeta, sizeof(CryptMeta));
    VcWipe(&CryptDevice, sizeof(CryptDevice));
}

BOOLEAN
DkCryptTransfer(PDK_DEVICE Device, ULONGLONG Offset, ULONG Length, PVOID Buffer, BOOLEAN Write)
{
    PUCHAR Scratch, Cursor = Buffer;
    ULONGLONG Aligned;
    ULONG Skip, Chunk, Span;
    BOOLEAN Success = TRUE;
    if (Offset > Device->Length || Length > Device->Length - Offset || !DkCryptActive(Device))
        return FALSE;
    if (!Length)
        return TRUE;
    Scratch = FwAllocatePages(65536);
    if (!Scratch)
        return FALSE;
    while (Length)
    {
        Aligned = Offset & ~UINT64_C(511);
        Skip = (ULONG)(Offset - Aligned);
        Chunk = min(Length, 65536 - Skip);
        Span = (Skip + Chunk + 511) & ~511;
        if ((!Write || Skip || Chunk != Span) &&
            (!DkReadRaw(Device, Aligned, Span, Scratch) ||
             !VcCryptoXts(CryptKey, Aligned / 512 + CryptMeta.IvTweak, Scratch, Span, 0)))
        {
            Success = FALSE;
            break;
        }
        if (Write)
        {
            RtlCopyMemory(Scratch + Skip, Cursor, Chunk);
            if (!VcCryptoXts(CryptKey, Aligned / 512 + CryptMeta.IvTweak, Scratch, Span, 1) ||
                !DkWriteRaw(Device, Aligned, Span, Scratch))
            {
                Success = FALSE;
                break;
            }
        }
        else
            RtlCopyMemory(Cursor, Scratch + Skip, Chunk);
        Cursor += Chunk;
        Offset += Chunk;
        Length -= Chunk;
    }
    VcWipe(Scratch, 65536);
    FwFreePages(Scratch, 65536);
    return Success;
}
