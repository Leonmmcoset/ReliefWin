/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Encryption of raw crash-dump and hibernation writes
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefcrypt.h"

static NTSTATUS NTAPI
CryptDumpPrepare(PVOID Context)
{
    CRYPT_DEVICE *Device = Context;
    NTSTATUS Status;
    if (InterlockedCompareExchange(&Device->DumpBusy, 1, 0))
        return STATUS_DEVICE_BUSY;
    if (CryptIsRemoved(Device) || !Device->Unlocked || !Device->DumpPinned)
        Status = STATUS_DEVICE_NOT_READY;
    else
        Status = Device->Dump.Prepare(Device->Dump.Context);
    InterlockedExchange(&Device->DumpBusy, 0);
    return Status;
}

static NTSTATUS NTAPI
CryptDumpWrite(PVOID Context, ULONG64 DiskOffset, PHYSICAL_ADDRESS Buffer, ULONG Length)
{
    CRYPT_DEVICE *Device = Context;
    ULONGLONG Relative, Physical;
    ULONG Done = 0, Chunk, Copied, PageBytes;
    PVOID Source;
    PHYSICAL_ADDRESS Page;
    NTSTATUS Status = STATUS_SUCCESS;
    if (InterlockedCompareExchange(&Device->DumpBusy, 1, 0))
        return STATUS_DEVICE_BUSY;
    if (CryptIsRemoved(Device) || !Device->Unlocked || !Device->DumpPinned)
    {
        Status = STATUS_DEVICE_NOT_READY;
        goto Done;
    }
    if (DiskOffset < (ULONGLONG)Device->Partition.StartingOffset.QuadPart)
    {
        Status = STATUS_INVALID_PARAMETER;
        goto Done;
    }
    Relative = DiskOffset - Device->Partition.StartingOffset.QuadPart;
    Physical = Buffer.QuadPart;
    if (Relative % 512 || Length % 512 || Relative > Device->Partition.PartitionLength.QuadPart ||
        Length > (ULONGLONG)Device->Partition.PartitionLength.QuadPart - Relative ||
        Physical > UINT64_MAX - Length)
    {
        Status = STATUS_INVALID_PARAMETER;
        goto Done;
    }
    while (Done < Length)
    {
        Chunk = min(Length - Done, min(CRYPT_TRANSFER, Device->Dump.MaximumTransferLength)) & ~511;
        for (Copied = 0; Copied < Chunk; Copied += PageBytes)
        {
            Page.QuadPart = Physical + Done + Copied;
            PageBytes = min(Chunk - Copied, PAGE_SIZE - (ULONG)(Page.QuadPart % PAGE_SIZE));
            Source = MmGetVirtualForPhysical(Page);
            if (!Source)
            {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto Done;
            }
            RtlCopyMemory((PUCHAR)Device->DumpBuffer + Copied, Source, PageBytes);
        }
        if (VcCrypt(&CryptOps, &Device->Meta, Device->Key, Relative + Done,
                    Device->DumpBuffer, Chunk, 1) != VcOk)
        {
            Status = STATUS_DATA_ERROR;
            goto Done;
        }
        Status = Device->Dump.WriteRoutine(Device->Dump.Context, DiskOffset + Done,
                                           Device->DumpPhysical, Chunk);
        if (!NT_SUCCESS(Status))
            goto Done;
        Done += Chunk;
    }
Done:
    if (Device->DumpBuffer)
        VcWipe(Device->DumpBuffer, CRYPT_TRANSFER);
    InterlockedExchange(&Device->DumpBusy, 0);
    return Status;
}

static NTSTATUS NTAPI
CryptDumpFlush(PVOID Context)
{
    CRYPT_DEVICE *Device = Context;
    NTSTATUS Status;
    if (InterlockedCompareExchange(&Device->DumpBusy, 1, 0))
        return STATUS_DEVICE_BUSY;
    Status = CryptIsRemoved(Device) ? STATUS_DEVICE_NOT_READY : Device->Dump.Flush(Device->Dump.Context);
    InterlockedExchange(&Device->DumpBusy, 0);
    return Status;
}

NTSTATUS
CryptQueryDump(CRYPT_DEVICE *Device, PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    ROS_STORAGE_DUMP_INTERFACE *Output = Irp->AssociatedIrp.SystemBuffer;
    PHYSICAL_ADDRESS Highest;
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(*Output) ||
        Stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(*Output) ||
        (Output->Version && Output->Version != ROS_STORAGE_DUMP_INTERFACE_VERSION) ||
        (Output->Size && Output->Size != sizeof(*Output)))
        return STATUS_INVALID_PARAMETER;
    if (!Device->Unlocked || Device->Offline)
        return STATUS_ACCESS_DENIED;
    if (!Device->DumpPinned)
    {
        RtlZeroMemory(&Device->Dump, sizeof(Device->Dump));
        Device->Dump.Version = ROS_STORAGE_DUMP_INTERFACE_VERSION;
        Device->Dump.Size = sizeof(Device->Dump);
        Device->Dump.BytesPerSector = Output->BytesPerSector;
        Status = CryptLowerControl(Device, IOCTL_REACTOS_STORAGE_GET_DUMP_INTERFACE,
                                   &Device->Dump, sizeof(Device->Dump), sizeof(Device->Dump));
        if (!NT_SUCCESS(Status))
            return Status;
        if (Device->Dump.Version != ROS_STORAGE_DUMP_INTERFACE_VERSION ||
            Device->Dump.Size != sizeof(Device->Dump) || !Device->Dump.Prepare ||
            !Device->Dump.WriteRoutine || !Device->Dump.Flush ||
            Device->Dump.BytesPerSector != 512 || Device->Dump.MaximumTransferLength < 512)
            return STATUS_NOT_SUPPORTED;
        Highest.QuadPart = MAXULONG;
        Device->DumpBuffer = MmAllocateContiguousMemory(CRYPT_TRANSFER, Highest);
        if (!Device->DumpBuffer)
            return STATUS_INSUFFICIENT_RESOURCES;
        Device->DumpPhysical = MmGetPhysicalAddress(Device->DumpBuffer);
        Device->DumpPinned = TRUE;
    }
    RtlZeroMemory(Output, sizeof(*Output));
    Output->Version = ROS_STORAGE_DUMP_INTERFACE_VERSION;
    Output->Size = sizeof(*Output);
    Output->BytesPerSector = Device->Dump.BytesPerSector;
    Output->MaximumTransferLength = min(Device->Dump.MaximumTransferLength, CRYPT_TRANSFER);
    Output->Context = Device;
    Output->Prepare = CryptDumpPrepare;
    Output->WriteRoutine = CryptDumpWrite;
    Output->Flush = CryptDumpFlush;
    Irp->IoStatus.Information = sizeof(*Output);
    return STATUS_SUCCESS;
}
