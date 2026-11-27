/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Ordered encrypted I/O without modifying write-source pages
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefcrypt.h"

static NTSTATUS
CryptLowerIo(CRYPT_DEVICE *Device, UCHAR Major, PVOID Buffer, ULONG Size,
             LONGLONG Offset, PIRP Original, PULONG_PTR Information)
{
    KEVENT Event;
    IO_STATUS_BLOCK Iosb;
    LARGE_INTEGER Position;
    PIRP Irp;
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    Position.QuadPart = Offset;
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    Irp = IoBuildSynchronousFsdRequest(Major, Device->Lower, Buffer, Size, &Position, &Event, &Iosb);
    if (!Irp)
        return STATUS_INSUFFICIENT_RESOURCES;
    Irp->Flags |= Original->Flags & (IRP_NOCACHE | IRP_WRITE_OPERATION);
    IoGetNextIrpStackLocation(Irp)->Flags = IoGetCurrentIrpStackLocation(Original)->Flags;
    Status = IoCallDriver(Device->Lower, Irp);
    if (Status == STATUS_PENDING)
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
    Status = Iosb.Status;
    *Information = Iosb.Information;
    return Status;
}

static NTSTATUS NTAPI
CryptWorkerCompletion(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    (void)DeviceObject;
    (void)Irp;
    KeSetEvent(Context, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

static NTSTATUS
CryptPassWorker(CRYPT_DEVICE *Device, PIRP Irp)
{
    KEVENT Event;
    NTSTATUS Status;
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, CryptWorkerCompletion, &Event, TRUE, TRUE, TRUE);
    Status = IoCallDriver(Device->Lower, Irp);
    if (Status == STATUS_PENDING)
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
    return Irp->IoStatus.Status;
}

static NTSTATUS
CryptTransfer(CRYPT_DEVICE *Device, PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    ULONGLONG Offset = (ULONGLONG)Stack->Parameters.Read.ByteOffset.QuadPart;
    ULONG Length = Stack->Parameters.Read.Length, Done = 0, Chunk;
    PUCHAR Pages, Scratch;
    ULONG_PTR Transferred;
    NTSTATUS Status = STATUS_SUCCESS;
    BOOLEAN Write = Stack->MajorFunction == IRP_MJ_WRITE;
    if (!Device->Configured)
        return CryptPassWorker(Device, Irp);
    if (!Device->Unlocked || Device->Offline)
        return STATUS_ACCESS_DENIED;
    if (Offset % 512 || Length % 512 || Offset > Device->Partition.PartitionLength.QuadPart ||
        Length > (ULONGLONG)Device->Partition.PartitionLength.QuadPart - Offset)
        return STATUS_INVALID_PARAMETER;
    if (!Length)
    {
        Irp->IoStatus.Information = 0;
        return STATUS_SUCCESS;
    }
    if (!Irp->MdlAddress || MmGetMdlByteCount(Irp->MdlAddress) < Length)
        return STATUS_INVALID_USER_BUFFER;
    Pages = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    if (!Pages)
        return STATUS_INSUFFICIENT_RESOURCES;
    Scratch = ExAllocatePoolWithTag(NonPagedPoolNx, CRYPT_TRANSFER, CRYPT_TAG);
    if (!Scratch)
        return STATUS_INSUFFICIENT_RESOURCES;
    while (Done < Length)
    {
        if (Irp->Cancel)
        {
            Status = STATUS_CANCELLED;
            break;
        }
        Chunk = min(Length - Done, CRYPT_TRANSFER);
        if (Write)
        {
            RtlCopyMemory(Scratch, Pages + Done, Chunk);
            if (VcCrypt(&CryptOps, &Device->Meta, Device->Key, Offset + Done, Scratch, Chunk, 1) != VcOk)
            {
                Status = STATUS_DATA_ERROR;
                break;
            }
        }
        Status = CryptLowerIo(Device, Stack->MajorFunction, Scratch, Chunk, Offset + Done, Irp, &Transferred);
        if (!NT_SUCCESS(Status) || Transferred != Chunk)
        {
            if (NT_SUCCESS(Status))
                Status = STATUS_DEVICE_DATA_ERROR;
            break;
        }
        if (!Write)
        {
            if (VcCrypt(&CryptOps, &Device->Meta, Device->Key, Offset + Done, Scratch, Chunk, 0) != VcOk)
            {
                Status = STATUS_DATA_ERROR;
                break;
            }
            RtlCopyMemory(Pages + Done, Scratch, Chunk);
        }
        Done += Chunk;
    }
    VcWipe(Scratch, CRYPT_TRANSFER);
    ExFreePoolWithTag(Scratch, CRYPT_TAG);
    Irp->IoStatus.Information = Done;
    return Status;
}

static NTSTATUS
CryptProcess(CRYPT_DEVICE *Device, PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS Status;
    if (Irp->Cancel)
        return STATUS_CANCELLED;
    if (Device->Control)
        return CryptControl(Device, Irp);
    CryptLock(&Device->Gate);
    if (CryptIsRemoved(Device))
        Status = STATUS_DELETE_PENDING;
    else if (Stack->MajorFunction == IRP_MJ_READ || Stack->MajorFunction == IRP_MJ_WRITE)
        Status = CryptTransfer(Device, Irp);
    else if (Stack->MajorFunction == IRP_MJ_DEVICE_CONTROL &&
             Stack->Parameters.DeviceIoControl.IoControlCode == RW_CRYPT_IOCTL_VOLUME_STATE)
    {
        if (Stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(ULONG))
            Status = STATUS_BUFFER_TOO_SMALL;
        else
        {
            *(PULONG)Irp->AssociatedIrp.SystemBuffer =
                (Device->Configured ? RW_CRYPT_FLAG_CONFIGURED : 0) |
                (Device->Unlocked ? RW_CRYPT_FLAG_UNLOCKED : 0) |
                (Device->System ? RW_CRYPT_FLAG_SYSTEM : 0);
            Irp->IoStatus.Information = sizeof(ULONG);
            Status = STATUS_SUCCESS;
        }
    }
    else if (Stack->MajorFunction == IRP_MJ_DEVICE_CONTROL &&
             Stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_REACTOS_STORAGE_GET_DUMP_INTERFACE &&
             Device->Configured)
        Status = CryptQueryDump(Device, Irp);
    else if (Stack->MajorFunction == IRP_MJ_DEVICE_CONTROL && Device->Configured &&
             Stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES)
        Status = STATUS_NOT_SUPPORTED;
    else
        Status = CryptPassWorker(Device, Irp);
    CryptUnlock(&Device->Gate);
    return Status;
}

VOID NTAPI
CryptIoWorker(PDEVICE_OBJECT DeviceObject, PVOID Context)
{
    CRYPT_WORK *Initial = Context, *Work;
    CRYPT_DEVICE *Device = DeviceObject->DeviceExtension;
    PIO_WORKITEM PumpItem = Initial->Item;
    KIRQL OldIrql;
    NTSTATUS Status;
    PIRP Irp;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    for (;;)
    {
        KeAcquireSpinLock(&Device->QueueLock, &OldIrql);
        if (IsListEmpty(&Device->Queue))
        {
            Device->PumpRunning = FALSE;
            KeReleaseSpinLock(&Device->QueueLock, OldIrql);
            break;
        }
        Work = CONTAINING_RECORD(RemoveHeadList(&Device->Queue), CRYPT_WORK, Link);
        KeReleaseSpinLock(&Device->QueueLock, OldIrql);
        ASSERT(Work->Device == Device);
        Irp = Work->Irp;
        Status = CryptProcess(Device, Irp);
        if (Work->Item != PumpItem)
        {
            IoReleaseRemoveLock(&Device->RemoveLock, Work->Item);
            IoFreeWorkItem(Work->Item);
        }
        ExFreePoolWithTag(Work, CRYPT_TAG);
        IoReleaseRemoveLock(&Device->RemoveLock, Irp);
        CryptComplete(Irp, Status, Irp->IoStatus.Information);
    }
    IoFreeWorkItem(PumpItem);
    IoReleaseRemoveLock(&Device->RemoveLock, PumpItem);
}
