/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Management and offline conversion contracts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once
#include <windows.h>
#include <winioctl.h>
#include <ntddstor.h>
#include <strsafe.h>
#include <stdio.h>
#include <reliefwin/drivers/volumecrypt.h>
#include <reliefwin/libs/volumecrypt/crypto.h>

#define DC_MAX_VOLUMES 128
#define DC_PATH 512

typedef struct DC_VOLUME
{
    WCHAR Root[DC_PATH];
    WCHAR Device[DC_PATH];
    WCHAR Label[128];
    DWORD Disk;
    PARTITION_INFORMATION_EX Partition;
    DWORD Signature;
    GUID DiskGuid;
    BOOL System;
    BOOL Boot;
    BOOL Running;
    int Index;
    DWORD Flags;
} DC_VOLUME;

typedef struct DC_STORE
{
    WCHAR Root[DC_PATH];
    RW_CRYPT_DESCRIPTOR Volumes[RW_CRYPT_MAX_VOLUMES];
    uint64_t Sequence;
    uint32_t Count;
    unsigned First;
} DC_STORE;

typedef struct DC_FILE
{
    HANDLE Handle;
    ULONGLONG Base;
    ULONGLONG Size;
} DC_FILE;

typedef void (*DC_PROGRESS)(void *Context, const VC_PROGRESS *Progress);
extern const VC_OPS DcOps;
DWORD DcError(VC_RESULT Result);
DWORD DcEnumerate(DC_VOLUME *Volumes, DWORD *Count);
DWORD DcReadStore(DC_STORE *Store);
DWORD DcWriteStore(DC_STORE *Store);
DWORD DcBindVolumes(DC_STORE *Store, DC_VOLUME *Volumes, DWORD Count);
DWORD DcLoadHeader(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor,
                    BYTE **Bytes, VC_META *Meta);
DWORD DcSecret(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor,
                const char *Secret, BYTE **Bytes, VC_META *Meta, BYTE Key[VC_KEY_SIZE]);
DWORD DcControl(ULONG Code, const RW_CRYPT_DESCRIPTOR *Descriptor,
                 const BYTE *Header, const BYTE *Key);
DWORD DcConvert(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret,
                 const char *Recovery, PCWSTR Backup, BOOL Decrypt, volatile LONG *Cancel,
                 DC_PROGRESS Progress, void *Context);
DWORD DcChange(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret,
                const char *Replacement, BOOL Recovery, PCWSTR Backup);
void DcMemoryIo(BYTE *Bytes, VC_IO *Io);
DWORD DcExport(DC_STORE *Store, DC_VOLUME *Volume, const RW_CRYPT_DESCRIPTOR *Descriptor,
                const BYTE *Header, const char *Recovery, PCWSTR Path);
DWORD DcImport(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret, PCWSTR Path);
DWORD DcHeaderPath(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor, PWSTR Path, BOOL Journal);
DWORD DcOpenFile(PCWSTR Path, ULONGLONG Size, DWORD Creation, DC_FILE *File, VC_IO *Io);
DWORD DcReadWhole(PCWSTR Path, BYTE *Buffer, DWORD Size);
DWORD DcWriteWhole(PCWSTR Path, const BYTE *Buffer, DWORD Size, DWORD Creation);
void DcFileIo(DC_FILE *File, HANDLE Handle, ULONGLONG Base, ULONGLONG Size, VC_IO *Io);
DWORD DcExclusive(DC_VOLUME *Volume, HANDLE *Handle);
DWORD DcSetLock(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret, BOOL Unlock);
BOOL DcRecoveryEnvironment(void);
DWORD DcPrepareBoot(DC_STORE *Store, DC_VOLUME *Volume);
