/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Persistent volume identity, administrative lock and unlock
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefcrypt.h"

static RW_CRYPT_DESCRIPTOR Configuration[RW_CRYPT_MAX_VOLUMES];
static ULONG ConfigurationCount;

typedef struct CRYPT_MEMORY
{
    PUCHAR Bytes;
    ULONG Size;
} CRYPT_MEMORY;

static int
CryptReadMemory(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    CRYPT_MEMORY *Memory = Context;
    if (Offset > Memory->Size || Size > Memory->Size - Offset)
        return 0;
    RtlCopyMemory(Buffer, Memory->Bytes + (SIZE_T)Offset, Size);
    return 1;
}

static NTSTATUS
CryptResult(VC_RESULT Result)
{
    switch (Result)
    {
        case VcOk: return STATUS_SUCCESS;
        case VcNoMemory: return STATUS_INSUFFICIENT_RESOURCES;
        case VcWrongSecret: return STATUS_ACCESS_DENIED;
        case VcUnsupported: return STATUS_NOT_SUPPORTED;
        case VcIncomplete: return STATUS_DEVICE_NOT_READY;
        case VcConflict: return STATUS_OBJECT_NAME_COLLISION;
        case VcIoError: return STATUS_IO_DEVICE_ERROR;
        default: return STATUS_DATA_ERROR;
    }
}

NTSTATUS
CryptReadConfiguration(PCUNICODE_STRING RegistryPath)
{
    ULONG i, j;
    (void)RegistryPath;
    ConfigurationCount = ReliefCryptBootState.Count;
    for (i = 0; i < ConfigurationCount; ++i)
    {
        RW_CRYPT_DESCRIPTOR *Descriptor = &ReliefCryptBootState.Entries[i].Descriptor;
        if (!RwCryptDescriptorValid(&CryptOps, Descriptor))
            return STATUS_DATA_ERROR;
        for (j = 0; j < i; ++j)
            if (!memcmp(Configuration[j].Bytes + 12, Descriptor->Bytes + 12, 60))
                return STATUS_DATA_ERROR;
        Configuration[i] = *Descriptor;
    }
    return STATUS_SUCCESS;
}

static NTSTATUS
CryptSaveConfiguration(const RW_CRYPT_DESCRIPTOR *Descriptor, BOOLEAN Delete)
{
    ULONG i;
    for (i = 0; i < ConfigurationCount; ++i)
    {
        if (memcmp(Configuration[i].Bytes + 12, Descriptor->Bytes + 12, 60))
            continue;
        if (memcmp(&Configuration[i], Descriptor, sizeof(*Descriptor)))
            return STATUS_OBJECT_NAME_COLLISION;
        if (Delete)
        {
            --ConfigurationCount;
            RtlMoveMemory(Configuration + i, Configuration + i + 1,
                           (ConfigurationCount - i) * sizeof(Configuration[0]));
            RtlZeroMemory(Configuration + ConfigurationCount, sizeof(Configuration[0]));
        }
        return STATUS_SUCCESS;
    }
    if (Delete)
        return STATUS_SUCCESS;
    if (ConfigurationCount == RW_CRYPT_MAX_VOLUMES)
        return STATUS_INSUFFICIENT_RESOURCES;
    Configuration[ConfigurationCount++] = *Descriptor;
    return STATUS_SUCCESS;
}

NTSTATUS
CryptInitializeVolume(CRYPT_DEVICE *Device)
{
    ULONG LayoutSize = 65536, i;
    PDRIVE_LAYOUT_INFORMATION_EX Layout;
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    Status = CryptLowerControl(Device, IOCTL_DISK_GET_PARTITION_INFO_EX, &Device->Partition,
                               0, sizeof(Device->Partition));
    if (!NT_SUCCESS(Status))
        return Status;
    if (Device->Partition.PartitionStyle == PARTITION_STYLE_RAW)
        return STATUS_NOT_SUPPORTED;
    Layout = ExAllocatePoolWithTag(PagedPool, LayoutSize, CRYPT_TAG);
    if (!Layout)
        return STATUS_INSUFFICIENT_RESOURCES;
    Status = CryptLowerControl(Device, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, Layout, 0, LayoutSize);
    if (!NT_SUCCESS(Status))
    {
        ExFreePoolWithTag(Layout, CRYPT_TAG);
        return Status;
    }
    if (Layout->PartitionStyle == PARTITION_STYLE_MBR)
        Device->Signature = Layout->Mbr.Signature;
    else if (Layout->PartitionStyle == PARTITION_STYLE_GPT)
        Device->DiskGuid = Layout->Gpt.DiskId;
    ExFreePoolWithTag(Layout, CRYPT_TAG);
    CryptLock(&CryptListGate);
    CryptLock(&Device->Gate);
    for (i = 0; i < ConfigurationCount; ++i)
    {
        if (RwCryptDescriptorMatch(&Configuration[i], Device->Partition.PartitionStyle,
                                   Device->Signature, Device->Partition.StartingOffset.QuadPart,
                                   Device->Partition.PartitionLength.QuadPart, &Device->DiskGuid,
                                   &Device->Partition.Gpt.PartitionId))
        {
            Device->Descriptor = Configuration[i];
            Device->Configured = TRUE;
            Device->Unlocked = FALSE;
            break;
        }
    }
    for (i = 0; Device->Configured && i < ReliefCryptBootState.Count; ++i)
    {
        RW_CRYPT_BOOT_ENTRY *Entry = &ReliefCryptBootState.Entries[i];
        if (memcmp(&Entry->Descriptor, &Device->Descriptor, sizeof(Device->Descriptor)))
            continue;
        Device->Meta = Entry->Meta;
        Device->Offline = !!Entry->Meta.Incomplete;
        if (Entry->Unlocked && !Entry->Meta.Incomplete && !Entry->Meta.PayloadOffset &&
            !strcmp(Entry->Meta.Uuid, (const char *)Entry->Descriptor.Bytes + 72) &&
            VcCheckKey(&CryptOps, &Entry->Meta, Entry->Key) == VcOk)
        {
            RtlCopyMemory(Device->Key, Entry->Key, VC_KEY_SIZE);
            Device->Unlocked = TRUE;
            Device->System = TRUE;
        }
        VcWipe(Entry->Key, sizeof(Entry->Key));
        Entry->Unlocked = 0;
        break;
    }
    CryptUnlock(&Device->Gate);
    CryptUnlock(&CryptListGate);
    DPRINT1("ReliefWin encryption: partition at %I64x, configured=%u unlocked=%u\n",
            Device->Partition.StartingOffset.QuadPart, Device->Configured, Device->Unlocked);
    return STATUS_SUCCESS;
}

static BOOLEAN
CryptMounted(CRYPT_DEVICE *Device)
{
    KIRQL OldIrql;
    BOOLEAN Mounted;
    IoAcquireVpbSpinLock(&OldIrql);
    Mounted = Device->Pdo->Vpb && (Device->Pdo->Vpb->Flags & VPB_MOUNTED) &&
              ((Device->Pdo->Vpb->Flags & (VPB_LOCKED | VPB_DIRECT_WRITES_ALLOWED)) !=
               (VPB_LOCKED | VPB_DIRECT_WRITES_ALLOWED));
    IoReleaseVpbSpinLock(OldIrql);
    return Mounted;
}

static NTSTATUS
CryptStatus(PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    RW_CRYPT_STATUS *Output = Irp->AssociatedIrp.SystemBuffer;
    PLIST_ENTRY Link;
    CRYPT_DEVICE *Device;
    if (Stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(*Output))
        return STATUS_BUFFER_TOO_SMALL;
    RtlZeroMemory(Output, sizeof(*Output));
    Output->Version = RW_CRYPT_VERSION;
    Output->Size = sizeof(*Output);
    CryptLock(&CryptListGate);
    for (Link = CryptDevices.Flink; Link != &CryptDevices; Link = Link->Flink)
    {
        RW_CRYPT_VOLUME_STATUS *Volume;
        Device = CONTAINING_RECORD(Link, CRYPT_DEVICE, Link);
        CryptLock(&Device->Gate);
        if (Device->Configured && Output->Count < RW_CRYPT_MAX_VOLUMES)
        {
            Volume = &Output->Volumes[Output->Count++];
            Volume->Descriptor = Device->Descriptor;
            Volume->Flags = RW_CRYPT_FLAG_CONFIGURED |
                (Device->Unlocked ? RW_CRYPT_FLAG_UNLOCKED : 0) |
                (Device->System ? RW_CRYPT_FLAG_SYSTEM : 0) |
                (Device->Offline ? RW_CRYPT_FLAG_OFFLINE : 0);
        }
        CryptUnlock(&Device->Gate);
    }
    CryptUnlock(&CryptListGate);
    Irp->IoStatus.Information = sizeof(*Output);
    return STATUS_SUCCESS;
}

NTSTATUS
CryptControl(CRYPT_DEVICE *Control, PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    RW_CRYPT_REQUEST *Request = Irp->AssociatedIrp.SystemBuffer;
    CRYPT_MEMORY Memory;
    VC_IO Header;
    VC_META Meta;
    VC_RESULT Result;
    PLIST_ENTRY Link;
    CRYPT_DEVICE *Device = NULL;
    ULONG Code = Stack->Parameters.DeviceIoControl.IoControlCode;
    NTSTATUS Status;
    (void)Control;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    if (Stack->MajorFunction != IRP_MJ_DEVICE_CONTROL)
        return STATUS_INVALID_DEVICE_REQUEST;
    if (Code == RW_CRYPT_IOCTL_STATUS)
        return CryptStatus(Irp);
    if (Code != RW_CRYPT_IOCTL_UNLOCK && Code != RW_CRYPT_IOCTL_LOCK && Code != RW_CRYPT_IOCTL_REGISTER &&
        Code != RW_CRYPT_IOCTL_UNREGISTER)
        return STATUS_INVALID_DEVICE_REQUEST;
    if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(*Request) ||
        Request->Version != RW_CRYPT_VERSION || Request->Size != sizeof(*Request) || Request->Reserved ||
        !RwCryptDescriptorValid(&CryptOps, &Request->Descriptor))
        return STATUS_INVALID_PARAMETER;
    RtlZeroMemory(&Meta, sizeof(Meta));
    if (Code != RW_CRYPT_IOCTL_LOCK && Code != RW_CRYPT_IOCTL_UNREGISTER)
    {
        if (Request->HeaderSize != VC_METADATA_SIZE ||
            Stack->Parameters.DeviceIoControl.InputBufferLength != sizeof(*Request) + Request->HeaderSize)
            return STATUS_INVALID_PARAMETER;
        Memory.Bytes = (PUCHAR)(Request + 1);
        Memory.Size = Request->HeaderSize;
        RtlZeroMemory(&Header, sizeof(Header));
        Header.Context = &Memory;
        Header.Size = Memory.Size;
        Header.Read = CryptReadMemory;
        Result = VcLoad(&CryptOps, &Header, &Meta);
        if (Result != VcOk)
            return CryptResult(Result);
        if (strcmp(Meta.Uuid, (const char *)Request->Descriptor.Bytes + 72) || Meta.PayloadOffset ||
            (Meta.PayloadSize && Meta.PayloadSize != RwCryptGet64(Request->Descriptor.Bytes + 32)))
            return STATUS_DATA_ERROR;
        if (Code == RW_CRYPT_IOCTL_UNLOCK)
        {
            if (Meta.Incomplete)
                return STATUS_DEVICE_NOT_READY;
            Result = VcCheckKey(&CryptOps, &Meta, Request->Key);
            if (Result != VcOk)
                return CryptResult(Result);
        }
    }
    CryptLock(&CryptListGate);
    for (Link = CryptDevices.Flink; Link != &CryptDevices; Link = Link->Flink)
    {
        CRYPT_DEVICE *Candidate = CONTAINING_RECORD(Link, CRYPT_DEVICE, Link);
        if (RwCryptDescriptorMatch(&Request->Descriptor, Candidate->Partition.PartitionStyle,
                                   Candidate->Signature, Candidate->Partition.StartingOffset.QuadPart,
                                   Candidate->Partition.PartitionLength.QuadPart, &Candidate->DiskGuid,
                                   &Candidate->Partition.Gpt.PartitionId))
        {
            Device = Candidate;
            break;
        }
    }
    if (!Device)
        Status = STATUS_NO_SUCH_DEVICE;
    else
    {
        Status = IoAcquireRemoveLock(&Device->RemoveLock, Request);
        if (NT_SUCCESS(Status))
        {
            CryptLock(&Device->Gate);
            if (CryptIsRemoved(Device))
                Status = STATUS_DELETE_PENDING;
            else if (Device->System || Device->DumpPinned || CryptMounted(Device))
                Status = STATUS_DEVICE_BUSY;
            else if (Code == RW_CRYPT_IOCTL_REGISTER)
            {
                if (Device->Configured && memcmp(&Device->Descriptor, &Request->Descriptor, sizeof(Device->Descriptor)))
                    Status = STATUS_OBJECT_NAME_COLLISION;
                else
                {
                    Status = CryptSaveConfiguration(&Request->Descriptor, FALSE);
                    if (NT_SUCCESS(Status))
                    {
                        Device->Descriptor = Request->Descriptor;
                        Device->Meta = Meta;
                        Device->Configured = TRUE;
                        Device->Offline = !!Meta.Incomplete;
                        Device->Unlocked = FALSE;
                        VcWipe(Device->Key, VC_KEY_SIZE);
                    }
                }
            }
            else if (!Device->Configured || memcmp(&Device->Descriptor, &Request->Descriptor, sizeof(Device->Descriptor)))
                Status = STATUS_NOT_FOUND;
            else if (Code == RW_CRYPT_IOCTL_UNREGISTER)
            {
                Status = CryptSaveConfiguration(&Request->Descriptor, TRUE);
                if (NT_SUCCESS(Status))
                {
                    Device->Configured = FALSE;
                    Device->Unlocked = FALSE;
                    Device->Offline = FALSE;
                    VcWipe(Device->Key, VC_KEY_SIZE);
                    RtlZeroMemory(&Device->Meta, sizeof(Device->Meta));
                    RtlZeroMemory(&Device->Descriptor, sizeof(Device->Descriptor));
                }
            }
            else if (Code == RW_CRYPT_IOCTL_LOCK)
            {
                Device->Unlocked = FALSE;
                VcWipe(Device->Key, VC_KEY_SIZE);
            }
            else
            {
                Device->Meta = Meta;
                RtlCopyMemory(Device->Key, Request->Key, VC_KEY_SIZE);
                Device->Offline = FALSE;
                Device->Unlocked = TRUE;
            }
            CryptUnlock(&Device->Gate);
            IoReleaseRemoveLock(&Device->RemoveLock, Request);
        }
    }
    CryptUnlock(&CryptListGate);
    VcWipe(Request->Key, VC_KEY_SIZE);
    return Status;
}
