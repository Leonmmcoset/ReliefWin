/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Encrypted-volume filter state and worker contracts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <ntifs.h>
#include <ntdddisk.h>
#include <ntddstor.h>
#include <wdmsec.h>
#include <reliefwin/drivers/volumecrypt.h>
#include <reliefwin/drivers/dumpstor.h>
#include <reliefwin/libs/volumecrypt/crypto.h>
#include <debug.h>

#define CRYPT_TAG 'cVWR'
#define CRYPT_TRANSFER 65536

typedef struct CRYPT_DEVICE
{
    BOOLEAN Control;
    BOOLEAN Configured;
    BOOLEAN Unlocked;
    BOOLEAN System;
    BOOLEAN Offline;
    volatile LONG Removed;
    BOOLEAN DumpPinned;
    PDEVICE_OBJECT Self;
    PDEVICE_OBJECT Lower;
    PDEVICE_OBJECT Pdo;
    IO_REMOVE_LOCK RemoveLock;
    KMUTEX Gate;
    KSPIN_LOCK QueueLock;
    LIST_ENTRY Queue;
    BOOLEAN PumpRunning;
    LIST_ENTRY Link;
    RW_CRYPT_DESCRIPTOR Descriptor;
    VC_META Meta;
    UCHAR Key[VC_KEY_SIZE];
    PARTITION_INFORMATION_EX Partition;
    ULONG Signature;
    GUID DiskGuid;
    ROS_STORAGE_DUMP_INTERFACE Dump;
    PVOID DumpBuffer;
    PHYSICAL_ADDRESS DumpPhysical;
    volatile LONG DumpBusy;
} CRYPT_DEVICE;

typedef struct CRYPT_WORK
{
    LIST_ENTRY Link;
    PIO_WORKITEM Item;
    PIRP Irp;
    CRYPT_DEVICE *Device;
} CRYPT_WORK;

extern const VC_OPS CryptOps;
extern KMUTEX CryptListGate;
extern LIST_ENTRY CryptDevices;
extern RW_CRYPT_BOOT_STATE ReliefCryptBootState;

FORCEINLINE BOOLEAN CryptIsRemoved(CRYPT_DEVICE *Device)
{
    return InterlockedCompareExchange(&Device->Removed, 0, 0) != 0;
}

NTSTATUS CryptComplete(PIRP Irp, NTSTATUS Status, ULONG_PTR Information);
NTSTATUS CryptLowerControl(CRYPT_DEVICE *Device, ULONG Code, PVOID Buffer, ULONG InSize, ULONG OutSize);
NTSTATUS NTAPI CryptDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS NTAPI CryptAddDevice(PDRIVER_OBJECT Driver, PDEVICE_OBJECT Pdo);
NTSTATUS CryptControl(CRYPT_DEVICE *Device, PIRP Irp);
NTSTATUS CryptQueryDump(CRYPT_DEVICE *Device, PIRP Irp);
NTSTATUS CryptReadConfiguration(PCUNICODE_STRING RegistryPath);
NTSTATUS CryptInitializeVolume(CRYPT_DEVICE *Device);
VOID NTAPI CryptIoWorker(PDEVICE_OBJECT DeviceObject, PVOID Context);

FORCEINLINE VOID CryptLock(PKMUTEX Mutex)
{
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    KeWaitForSingleObject(Mutex, Executive, KernelMode, FALSE, NULL);
}

FORCEINLINE VOID CryptUnlock(PKMUTEX Mutex)
{
    KeReleaseMutex(Mutex, FALSE);
}
