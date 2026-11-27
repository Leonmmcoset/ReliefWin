/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Boot-start encrypted-volume filter and device lifetime
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "reliefcrypt.h"

KMUTEX CryptListGate;
LIST_ENTRY CryptDevices;
RW_CRYPT_BOOT_STATE ReliefCryptBootState =
{
    RW_CRYPT_STATE_MAGIC, RW_CRYPT_VERSION, sizeof(RW_CRYPT_BOOT_STATE), 0, {{0}}
};

static void *
CryptAllocate(void *Context, size_t Size)
{
    (void)Context;
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    return ExAllocatePoolWithTag(NonPagedPoolNx, Size, CRYPT_TAG);
}

static void
CryptFree(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    VcWipe(Buffer, Size);
    ExFreePoolWithTag(Buffer, CRYPT_TAG);
}

void
VcCryptoFatal(uint32_t Code)
{
    KeBugCheckEx(0x139, Code, 0, 0, 0);
}

const VC_OPS CryptOps =
{
    NULL, CryptAllocate, CryptFree, NULL, VcCryptoHash, VcCryptoHmac, VcCryptoPbkdf2, VcCryptoXts
};

NTSTATUS
CryptComplete(PIRP Irp, NTSTATUS Status, ULONG_PTR Information)
{
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = Information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

static NTSTATUS NTAPI
CryptSyncCompletion(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    (void)DeviceObject;
    (void)Irp;
    KeSetEvent(Context, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

static NTSTATUS
CryptForwardSync(CRYPT_DEVICE *Device, PIRP Irp)
{
    KEVENT Event;
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, CryptSyncCompletion, &Event, TRUE, TRUE, TRUE);
    Status = IoCallDriver(Device->Lower, Irp);
    if (Status == STATUS_PENDING)
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
    return Irp->IoStatus.Status;
}

NTSTATUS
CryptLowerControl(CRYPT_DEVICE *Device, ULONG Code, PVOID Buffer, ULONG InSize, ULONG OutSize)
{
    KEVENT Event;
    IO_STATUS_BLOCK Iosb;
    PIRP Irp;
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    Irp = IoBuildDeviceIoControlRequest(Code, Device->Lower, Buffer, InSize, Buffer, OutSize,
                                       FALSE, &Event, &Iosb);
    if (!Irp)
        return STATUS_INSUFFICIENT_RESOURCES;
    Status = IoCallDriver(Device->Lower, Irp);
    if (Status == STATUS_PENDING)
    {
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
        Status = Iosb.Status;
    }
    return Status;
}

static NTSTATUS NTAPI
CryptForwardCompletion(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    CRYPT_DEVICE *Device = Context;
    (void)DeviceObject;
    if (Irp->PendingReturned)
        IoMarkIrpPending(Irp);
    IoReleaseRemoveLock(&Device->RemoveLock, Irp);
    return STATUS_CONTINUE_COMPLETION;
}

static NTSTATUS
CryptForward(CRYPT_DEVICE *Device, PIRP Irp)
{
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, CryptForwardCompletion, Device, TRUE, TRUE, TRUE);
    return IoCallDriver(Device->Lower, Irp);
}

static NTSTATUS
CryptPnp(CRYPT_DEVICE *Device, PIRP Irp)
{
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS Status;
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    if (Stack->MinorFunction == IRP_MN_START_DEVICE)
    {
        Status = CryptForwardSync(Device, Irp);
        if (NT_SUCCESS(Status))
            Status = CryptInitializeVolume(Device);
        IoReleaseRemoveLock(&Device->RemoveLock, Irp);
        return CryptComplete(Irp, Status, Irp->IoStatus.Information);
    }
    if ((Stack->MinorFunction == IRP_MN_QUERY_REMOVE_DEVICE || Stack->MinorFunction == IRP_MN_QUERY_STOP_DEVICE) &&
        (Device->System || Device->DumpPinned))
    {
        IoReleaseRemoveLock(&Device->RemoveLock, Irp);
        return CryptComplete(Irp, STATUS_DEVICE_BUSY, 0);
    }
    if (Stack->MinorFunction == IRP_MN_REMOVE_DEVICE)
    {
        LARGE_INTEGER Delay;
        CryptLock(&CryptListGate);
        InterlockedExchange(&Device->Removed, TRUE);
        RemoveEntryList(&Device->Link);
        CryptUnlock(&CryptListGate);
        IoSkipCurrentIrpStackLocation(Irp);
        Status = IoCallDriver(Device->Lower, Irp);
        IoReleaseRemoveLockAndWait(&Device->RemoveLock, Irp);
        IoDetachDevice(Device->Lower);
        Delay.QuadPart = -10000;
        while (InterlockedCompareExchange(&Device->DumpBusy, 1, 0))
            KeDelayExecutionThread(KernelMode, FALSE, &Delay);
        VcWipe(Device->Key, sizeof(Device->Key));
        if (Device->DumpBuffer)
            VcWipe(Device->DumpBuffer, CRYPT_TRANSFER);
        InterlockedExchange(&Device->DumpBusy, 0);
        if (!Device->DumpPinned)
        {
            if (Device->DumpBuffer)
            {
                VcWipe(Device->DumpBuffer, CRYPT_TRANSFER);
                MmFreeContiguousMemory(Device->DumpBuffer);
            }
            IoDeleteDevice(Device->Self);
        }
        return Status;
    }
    return CryptForward(Device, Irp);
}

NTSTATUS NTAPI
CryptDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    CRYPT_DEVICE *Device = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    CRYPT_WORK *Work;
    NTSTATUS Status;
    KIRQL OldIrql;
    BOOLEAN StartPump;
    if (Device->Control && (Stack->MajorFunction == IRP_MJ_CREATE || Stack->MajorFunction == IRP_MJ_CLOSE ||
                            Stack->MajorFunction == IRP_MJ_CLEANUP))
        return CryptComplete(Irp, STATUS_SUCCESS, 0);
    Status = IoAcquireRemoveLock(&Device->RemoveLock, Irp);
    if (!NT_SUCCESS(Status))
        return CryptComplete(Irp, Status, 0);
    if (!Device->Control && Stack->MajorFunction == IRP_MJ_PNP)
        return CryptPnp(Device, Irp);
    if (!Device->Control && Stack->MajorFunction == IRP_MJ_POWER)
    {
        PoStartNextPowerIrp(Irp);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutine(Irp, CryptForwardCompletion, Device, TRUE, TRUE, TRUE);
        return PoCallDriver(Device->Lower, Irp);
    }
    if (Device->Control || Stack->MajorFunction == IRP_MJ_READ || Stack->MajorFunction == IRP_MJ_WRITE ||
        (Stack->MajorFunction == IRP_MJ_DEVICE_CONTROL &&
         (Stack->Parameters.DeviceIoControl.IoControlCode == RW_CRYPT_IOCTL_VOLUME_STATE ||
          Stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_REACTOS_STORAGE_GET_DUMP_INTERFACE ||
          Stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES)) ||
        Stack->MajorFunction == IRP_MJ_FLUSH_BUFFERS ||
        Stack->MajorFunction == IRP_MJ_SHUTDOWN)
    {
        Work = ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(*Work), CRYPT_TAG);
        if (!Work)
        {
            IoReleaseRemoveLock(&Device->RemoveLock, Irp);
            return CryptComplete(Irp, STATUS_INSUFFICIENT_RESOURCES, 0);
        }
        Work->Item = IoAllocateWorkItem(DeviceObject);
        if (!Work->Item)
        {
            ExFreePoolWithTag(Work, CRYPT_TAG);
            IoReleaseRemoveLock(&Device->RemoveLock, Irp);
            return CryptComplete(Irp, STATUS_INSUFFICIENT_RESOURCES, 0);
        }
        Status = IoAcquireRemoveLock(&Device->RemoveLock, Work->Item);
        if (!NT_SUCCESS(Status))
        {
            IoFreeWorkItem(Work->Item);
            ExFreePoolWithTag(Work, CRYPT_TAG);
            IoReleaseRemoveLock(&Device->RemoveLock, Irp);
            return CryptComplete(Irp, Status, 0);
        }
        Work->Device = Device;
        Work->Irp = Irp;
        IoMarkIrpPending(Irp);
        KeAcquireSpinLock(&Device->QueueLock, &OldIrql);
        InsertTailList(&Device->Queue, &Work->Link);
        StartPump = !Device->PumpRunning;
        Device->PumpRunning = TRUE;
        KeReleaseSpinLock(&Device->QueueLock, OldIrql);
        if (StartPump)
            IoQueueWorkItem(Work->Item, CryptIoWorker, DelayedWorkQueue, Work);
        return STATUS_PENDING;
    }
    return CryptForward(Device, Irp);
}

NTSTATUS NTAPI
CryptAddDevice(PDRIVER_OBJECT Driver, PDEVICE_OBJECT Pdo)
{
    PDEVICE_OBJECT Fdo;
    CRYPT_DEVICE *Device;
    NTSTATUS Status;
    PAGED_CODE();
    Status = IoCreateDevice(Driver, sizeof(*Device), NULL, FILE_DEVICE_DISK, FILE_DEVICE_SECURE_OPEN, FALSE, &Fdo);
    if (!NT_SUCCESS(Status))
        return Status;
    Device = Fdo->DeviceExtension;
    RtlZeroMemory(Device, sizeof(*Device));
    Device->Self = Fdo;
    Device->Pdo = Pdo;
    IoInitializeRemoveLock(&Device->RemoveLock, CRYPT_TAG, 0, 0);
    KeInitializeMutex(&Device->Gate, 0);
    KeInitializeSpinLock(&Device->QueueLock);
    InitializeListHead(&Device->Queue);
    Device->Lower = IoAttachDeviceToDeviceStack(Fdo, Pdo);
    if (!Device->Lower)
    {
        IoDeleteDevice(Fdo);
        return STATUS_NO_SUCH_DEVICE;
    }
    Fdo->Flags |= DO_DIRECT_IO | (Device->Lower->Flags & DO_POWER_PAGABLE);
    Fdo->AlignmentRequirement = Device->Lower->AlignmentRequirement;
    CryptLock(&CryptListGate);
    InsertTailList(&CryptDevices, &Device->Link);
    CryptUnlock(&CryptListGate);
    Fdo->Flags &= ~DO_DEVICE_INITIALIZING;
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI
DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING RegistryPath)
{
    UNICODE_STRING Name = RTL_CONSTANT_STRING(L"\\Device\\ReliefWinEncryption");
    UNICODE_STRING Link = RTL_CONSTANT_STRING(L"\\DosDevices\\ReliefWinEncryption");
    UNICODE_STRING Sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;SY)(A;;GA;;;BA)");
    PDEVICE_OBJECT Control;
    CRYPT_DEVICE *Device;
    ULONG i;
    NTSTATUS Status;
    VcCryptoInitialize();
    KeInitializeMutex(&CryptListGate, 0);
    InitializeListHead(&CryptDevices);
    if (ReliefCryptBootState.Magic != RW_CRYPT_STATE_MAGIC ||
        ReliefCryptBootState.Version != RW_CRYPT_VERSION ||
        ReliefCryptBootState.Size != sizeof(ReliefCryptBootState) ||
        ReliefCryptBootState.Count > RW_CRYPT_MAX_VOLUMES)
        return STATUS_DATA_ERROR;
    Status = CryptReadConfiguration(RegistryPath);
    if (!NT_SUCCESS(Status))
        return Status;
    for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
        Driver->MajorFunction[i] = CryptDispatch;
    Driver->DriverExtension->AddDevice = CryptAddDevice;
    Status = IoCreateDeviceSecure(Driver, sizeof(*Device), &Name, FILE_DEVICE_UNKNOWN,
                                  FILE_DEVICE_SECURE_OPEN, FALSE, &Sddl, NULL, &Control);
    if (!NT_SUCCESS(Status))
        return Status;
    Device = Control->DeviceExtension;
    RtlZeroMemory(Device, sizeof(*Device));
    Device->Control = TRUE;
    Device->Self = Control;
    IoInitializeRemoveLock(&Device->RemoveLock, CRYPT_TAG, 0, 0);
    KeInitializeMutex(&Device->Gate, 0);
    KeInitializeSpinLock(&Device->QueueLock);
    InitializeListHead(&Device->Queue);
    Status = IoCreateSymbolicLink(&Link, &Name);
    if (!NT_SUCCESS(Status))
    {
        IoDeleteDevice(Control);
        return Status;
    }
    Control->Flags &= ~DO_DEVICE_INITIALIZING;
    return STATUS_SUCCESS;
}
