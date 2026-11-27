/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Stable detached metadata device identifiers
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "internal.h"
#include <reliefwin/drivers/volumecrypt.h>

uint64_t
RwCryptGet64(const uint8_t *Bytes)
{
    return VcBe64(Bytes);
}

uint32_t
RwCryptGet32(const uint8_t *Bytes)
{
    return VcBe32(Bytes);
}

int
RwCryptDescriptorValid(const VC_OPS *Ops, const RW_CRYPT_DESCRIPTOR *Descriptor)
{
    uint8_t Digest[32];
    const uint8_t *Bytes;
    unsigned i;
    if (!Ops || !Ops->Hash || !Descriptor)
        return 0;
    Bytes = Descriptor->Bytes;
    if (memcmp(Bytes, "RWCVOL01", 8) || VcBe32(Bytes + 8) != RW_CRYPT_VERSION ||
        VcBe32(Bytes + 12) > 1 || !VcBe64(Bytes + 32) || VcBe64(Bytes + 32) % 512 ||
        VcBe64(Bytes + 24) % 512 || VcBe64(Bytes + 24) > UINT64_MAX - VcBe64(Bytes + 32) ||
        Bytes[20] || Bytes[21] || Bytes[22] || Bytes[23] ||
        Bytes[108] || Bytes[109] || Bytes[110] || Bytes[111] || VcBe32(Bytes + 112) > 1)
        return 0;
    for (i = 0; i != 36; ++i)
    {
        uint8_t C = Bytes[72 + i];
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (C != '-')
                return 0;
        }
        else if (!((C >= '0' && C <= '9') || (C >= 'a' && C <= 'f')))
            return 0;
    }
    for (i = 116; i != 480; ++i)
        if (Bytes[i])
            return 0;
    return Ops->Hash(Bytes, 480, Digest) && VcEqual(Digest, Bytes + 480, 32);
}

int
RwCryptDescriptorSetSystem(const VC_OPS *Ops, RW_CRYPT_DESCRIPTOR *Descriptor)
{
    if (!RwCryptDescriptorValid(Ops, Descriptor)) return 0;
    VcPut32(Descriptor->Bytes + 112, 1);
    return Ops->Hash(Descriptor->Bytes, 480, Descriptor->Bytes + 480);
}

int
RwCryptDescriptorSystem(const RW_CRYPT_DESCRIPTOR *Descriptor)
{
    return VcBe32(Descriptor->Bytes + 112) == 1;
}

int
RwCryptDescriptorCreate(const VC_OPS *Ops, RW_CRYPT_DESCRIPTOR *Descriptor,
                        uint32_t Style, uint32_t Signature, uint64_t Offset, uint64_t Size,
                        const void *DiskGuid, const void *PartitionGuid, const char *Uuid)
{
    uint8_t *Bytes = Descriptor->Bytes;
    if (Style > 1 || !Uuid || strlen(Uuid) != 36 ||
        (Style == 1 && (!DiskGuid || !PartitionGuid)))
        return 0;
    memset(Descriptor, 0, sizeof(*Descriptor));
    memcpy(Bytes, "RWCVOL01", 8);
    VcPut32(Bytes + 8, RW_CRYPT_VERSION);
    VcPut32(Bytes + 12, Style);
    VcPut32(Bytes + 16, Style ? 0 : Signature);
    VcPut64(Bytes + 24, Offset);
    VcPut64(Bytes + 32, Size);
    if (Style)
    {
        memcpy(Bytes + 40, DiskGuid, 16);
        memcpy(Bytes + 56, PartitionGuid, 16);
    }
    memcpy(Bytes + 72, Uuid, 36);
    return Ops->Hash(Bytes, 480, Bytes + 480) && RwCryptDescriptorValid(Ops, Descriptor);
}

int
RwCryptDescriptorMatch(const RW_CRYPT_DESCRIPTOR *Descriptor, uint32_t Style,
                       uint32_t Signature, uint64_t Offset, uint64_t Size,
                       const void *DiskGuid, const void *PartitionGuid)
{
    const uint8_t *Bytes = Descriptor->Bytes;
    if (Style != VcBe32(Bytes + 12) || Offset != VcBe64(Bytes + 24) || Size != VcBe64(Bytes + 32))
        return 0;
    if (Style == 0)
        return Signature == VcBe32(Bytes + 16);
    return Style == 1 && DiskGuid && PartitionGuid &&
           !memcmp(DiskGuid, Bytes + 40, 16) && !memcmp(PartitionGuid, Bytes + 56, 16);
}

void
RwCryptHeaderPath(const RW_CRYPT_DESCRIPTOR *Descriptor, char Path[96])
{
    memcpy(Path, "\\EFI\\ReliefWin\\Encryption\\", 26);
    memcpy(Path + 26, Descriptor->Bytes + 72, 36);
    memcpy(Path + 62, ".luks", 6);
}

int
RwCryptIndexRead(const VC_OPS *Ops, const void *Buffer, size_t Size,
                 uint64_t *Sequence, uint32_t *Count)
{
    const uint8_t *Bytes = Buffer;
    uint8_t Digest[32];
    uint32_t i, j, Entries;
    const RW_CRYPT_DESCRIPTOR *Volumes;
    *Sequence = 0;
    *Count = 0;
    if (Size != RW_CRYPT_INDEX_SIZE || memcmp(Bytes, "RWCIDX01", 8) ||
        VcBe32(Bytes + 8) != RW_CRYPT_VERSION || !VcBe64(Bytes + 16))
        return 0;
    Entries = VcBe32(Bytes + 12);
    if (Entries > RW_CRYPT_MAX_VOLUMES)
        return 0;
    for (i = 24; i != 512; ++i)
        if (Bytes[i])
            return 0;
    for (i = 512 + Entries * RW_CRYPT_DESCRIPTOR_SIZE; i != RW_CRYPT_INDEX_SIZE - 32; ++i)
        if (Bytes[i])
            return 0;
    if (!Ops->Hash(Bytes, RW_CRYPT_INDEX_SIZE - 32, Digest) ||
        !VcEqual(Digest, Bytes + RW_CRYPT_INDEX_SIZE - 32, 32))
        return 0;
    Volumes = (const RW_CRYPT_DESCRIPTOR *)(Bytes + 512);
    for (i = 0; i != Entries; ++i)
    {
        if (!RwCryptDescriptorValid(Ops, Volumes + i))
            return 0;
        for (j = 0; j != i; ++j)
        {
            if (!memcmp(Volumes[i].Bytes + 12, Volumes[j].Bytes + 12, 60) ||
                !memcmp(Volumes[i].Bytes + 72, Volumes[j].Bytes + 72, 40))
                return 0;
        }
    }
    *Sequence = VcBe64(Bytes + 16);
    *Count = Entries;
    return 1;
}

int
RwCryptIndexCreate(const VC_OPS *Ops, void *Buffer, uint64_t Sequence,
                   const RW_CRYPT_DESCRIPTOR *Volumes, uint32_t Count)
{
    uint8_t *Bytes = Buffer;
    uint64_t CheckedSequence;
    uint32_t CheckedCount;
    if (!Sequence || Count > RW_CRYPT_MAX_VOLUMES)
        return 0;
    memset(Bytes, 0, RW_CRYPT_INDEX_SIZE);
    memcpy(Bytes, "RWCIDX01", 8);
    VcPut32(Bytes + 8, RW_CRYPT_VERSION);
    VcPut32(Bytes + 12, Count);
    VcPut64(Bytes + 16, Sequence);
    if (Count)
        memcpy(Bytes + 512, Volumes, Count * sizeof(*Volumes));
    return Ops->Hash(Bytes, RW_CRYPT_INDEX_SIZE - 32, Bytes + RW_CRYPT_INDEX_SIZE - 32) &&
           RwCryptIndexRead(Ops, Buffer, RW_CRYPT_INDEX_SIZE, &CheckedSequence, &CheckedCount);
}
