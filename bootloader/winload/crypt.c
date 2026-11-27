/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Password and recovery-key unlock before kernel image loading
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "winload.h"
#include <reliefwin/libs/volumecrypt/crypto.h>
#include <reliefwin/drivers/volumecrypt.h>

BOOLEAN DkCryptEnable(PDK_DEVICE Device, const VC_META *Meta, const UCHAR *Key);

static RW_CRYPT_BOOT_STATE CryptState;

typedef struct OSL_CRYPT_MEMORY
{
    PUCHAR Data;
    ULONG Size;
} OSL_CRYPT_MEMORY;

static void *
CryptAllocate(void *Context, size_t Size)
{
    (void)Context;
    return BlAllocate(Size);
}

static void
CryptFree(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    VcWipe(Buffer, Size);
    BlFree(Buffer);
}

static int
CryptRead(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    OSL_CRYPT_MEMORY *Memory = Context;
    if (Offset > Memory->Size || Size > Memory->Size - Offset)
        return 0;
    RtlCopyMemory(Buffer, Memory->Data + (SIZE_T)Offset, Size);
    return 1;
}

static const VC_OPS CryptOps =
{
    NULL, CryptAllocate, CryptFree, NULL, VcCryptoHash, VcCryptoHmac, VcCryptoPbkdf2, VcCryptoXts
};

static BOOLEAN
CryptMatch(PDK_DEVICE Device, const RW_CRYPT_DESCRIPTOR *Descriptor)
{
    DK_DEVICE Disk, Partition;
    UCHAR Sector[512], DiskGuid[16];
    const UCHAR *Bytes = Descriptor->Bytes;
    if (Device->Memory || !FwGetDisk(Device->DiskIndex, &Disk) ||
        Device->Offset != RwCryptGet64(Bytes + 24) || Device->Length != RwCryptGet64(Bytes + 32))
        return FALSE;
    if (RwCryptGet32(Bytes + 12) == 0)
    {
        return DkReadRaw(&Disk, 0, sizeof(Sector), Sector) &&
               RwCryptDescriptorMatch(Descriptor, 0, LeUlong(Sector + 440),
                                      Device->Offset, Device->Length, NULL, NULL);
    }
    return GptGetDiskId(&Disk, DiskGuid) &&
           !memcmp(DiskGuid, Bytes + 40, 16) &&
           GptFindPartition(&Disk, DiskGuid, Bytes + 56, &Partition) &&
           Partition.Offset == Device->Offset && Partition.Length == Device->Length;
}

BOOLEAN
OslCryptUnlock(PDK_DEVICE Device)
{
    RW_CRYPT_DESCRIPTOR *Index;
    PUCHAR Primary, Backup, Newest;
    ULONG PrimarySize = 0, BackupSize = 0;
    uint32_t PrimaryCount, BackupCount, Count;
    uint64_t PrimarySequence, BackupSequence;
    BOOLEAN PrimaryValid, BackupValid;
    RW_CRYPT_BOOT_ENTRY *Entry;
    OSL_CRYPT_MEMORY Memory;
    VC_IO Header;
    ULONG Slot, i, Length, Attempts;
    CHAR Path[96], Secret[129], Id[37] = {0};
    WCHAR Wide[96];
    VC_RESULT Result;
    BOOLEAN Match, Found = FALSE, Cancelled = FALSE, Incomplete = FALSE;

    RtlZeroMemory(&CryptState, sizeof(CryptState));
    Primary = FwReadBootFile(RW_CRYPT_INDEX_PATH, &PrimarySize);
    Backup = FwReadBootFile(RW_CRYPT_INDEX_BACKUP_PATH, &BackupSize);
    if (!Primary && !Backup)
        return TRUE;
    VcCryptoInitialize();
    PrimaryValid = Primary && RwCryptIndexRead(&CryptOps, Primary, PrimarySize, &PrimarySequence, &PrimaryCount);
    BackupValid = Backup && RwCryptIndexRead(&CryptOps, Backup, BackupSize, &BackupSequence, &BackupCount);
    if ((!PrimaryValid && !BackupValid) ||
        (PrimaryValid && BackupValid && PrimarySequence == BackupSequence &&
         memcmp(Primary, Backup, RW_CRYPT_INDEX_SIZE)))
        goto Failed;
    if (PrimaryValid && (!BackupValid || PrimarySequence >= BackupSequence))
    {
        Newest = Primary;
        Count = PrimaryCount;
    }
    else
    {
        Newest = Backup;
        Count = BackupCount;
    }
    Index = (RW_CRYPT_DESCRIPTOR *)(Newest + 512);
    CryptState.Magic = RW_CRYPT_STATE_MAGIC;
    CryptState.Version = RW_CRYPT_VERSION;
    CryptState.Size = sizeof(CryptState);
    for (Slot = 0; Slot != Count; ++Slot)
    {
        Entry = &CryptState.Entries[CryptState.Count++];
        Entry->Descriptor = Index[Slot];
        Match = CryptMatch(Device, &Index[Slot]);
        if (Match && Found)
            goto Failed;
        Found |= Match;
        RwCryptHeaderPath(&Index[Slot], Path);
        for (i = 0; Path[i]; ++i)
            Wide[i] = (UCHAR)Path[i];
        Wide[i] = 0;
        Memory.Data = FwReadBootFile(Wide, &Memory.Size);
        if (!Memory.Data)
        {
            if (Match)
                goto Failed;
            continue;
        }
        RtlZeroMemory(&Header, sizeof(Header));
        Header.Context = &Memory;
        Header.Size = Memory.Size;
        Header.Read = CryptRead;
        Result = VcLoad(&CryptOps, &Header, &Entry->Meta);
        if (Result != VcOk || strcmp(Entry->Meta.Uuid, (const char *)Entry->Descriptor.Bytes + 72))
        {
            FwFreePages(Memory.Data, Memory.Size);
            if (Match)
                goto Failed;
            RtlZeroMemory(&Entry->Meta, sizeof(Entry->Meta));
            continue;
        }
        if (Match)
        {
            RtlCopyMemory(Id, Entry->Meta.Uuid, sizeof(Id));
            BmUiEndBoot();
            if (Entry->Meta.Incomplete)
            {
                FwFreePages(Memory.Data, Memory.Size);
                Incomplete = TRUE;
                goto Failed;
            }
            for (Attempts = 0; Attempts != 3; ++Attempts)
            {
                RtlZeroMemory(Secret, sizeof(Secret));
                if (!BmUiCryptPassword(Id, Attempts != 0, Secret, RTL_NUMBER_OF(Secret), &Length, &Cancelled))
                    break;
                Result = VcUnlock(&CryptOps, &Header, &Entry->Meta, Secret, Length, Entry->Key);
                VcWipe(Secret, sizeof(Secret));
                if (Result == VcOk)
                {
                    Entry->Unlocked = DkCryptEnable(Device, &Entry->Meta, Entry->Key);
                    break;
                }
                if (Result != VcWrongSecret)
                    break;
            }
            VcWipe(Secret, sizeof(Secret));
            FwFreePages(Memory.Data, Memory.Size);
            if (!Entry->Unlocked)
                goto Failed;
            BmUiShutdown();
            BmUiBeginBoot();
        }
        else
            FwFreePages(Memory.Data, Memory.Size);
    }
    if (Primary)
        FwFreePages(Primary, PrimarySize);
    if (Backup)
        FwFreePages(Backup, BackupSize);
    return TRUE;
Failed:
    if (Primary)
        FwFreePages(Primary, PrimarySize);
    if (Backup)
        FwFreePages(Backup, BackupSize);
    VcWipe(&CryptState, sizeof(CryptState));
    DkCryptForget();
    BmUiEndBoot();
    if (!Cancelled) BmUiCryptFailure(Id[0] ? Id : NULL, Incomplete);
    else BmUiShutdown();
    return FALSE;
}

BOOLEAN
OslCryptHandoff(PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    PKLDR_DATA_TABLE_ENTRY Driver;
    RW_CRYPT_BOOT_STATE *State;
    if (!CryptState.Count)
        return TRUE;
    if (!PeLdrCheckForLoadedDll(&LoaderBlock->LoadOrderListHead, "reliefcrypt.sys", &Driver))
        return FALSE;
    State = PeGetExport(&LoaderBlock->LoadOrderListHead, Driver, "ReliefCryptBootState");
    if (!State)
        return FALSE;
    State = VaToPa(State);
    if (State->Magic != RW_CRYPT_STATE_MAGIC || State->Version != RW_CRYPT_VERSION ||
        State->Size != sizeof(*State))
        return FALSE;
    RtlCopyMemory(State, &CryptState, sizeof(*State));
    VcWipe(&CryptState, sizeof(CryptState));
    DkCryptForget();
    return TRUE;
}
