/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Private disk encryption management and loader handoff ABI
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <reliefwin/libs/volumecrypt/volumecrypt.h>

#define RW_CRYPT_VERSION 1
#define RW_CRYPT_MAX_VOLUMES 16
#define RW_CRYPT_DESCRIPTOR_SIZE 512
#define RW_CRYPT_INDEX_SIZE 12288
#define RW_CRYPT_INDEX_BACKUP_PATH L"\\EFI\\ReliefWin\\Encryption\\Volumes.bak"
#define RW_CRYPT_INDEX_PATH L"\\EFI\\ReliefWin\\Encryption\\Volumes.dat"
#define RW_CRYPT_STATE_MAGIC 0x52435742
#define RW_CRYPT_IOCTL_VOLUME_STATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x984, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define RW_CRYPT_IOCTL_STATUS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x980, METHOD_BUFFERED, FILE_READ_ACCESS)
#define RW_CRYPT_IOCTL_UNLOCK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x981, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define RW_CRYPT_IOCTL_LOCK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x982, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define RW_CRYPT_IOCTL_REGISTER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x983, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define RW_CRYPT_IOCTL_UNREGISTER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x985, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define RW_CRYPT_FLAG_CONFIGURED 1
#define RW_CRYPT_FLAG_UNLOCKED 2
#define RW_CRYPT_FLAG_SYSTEM 4
#define RW_CRYPT_FLAG_OFFLINE 8

typedef struct RW_CRYPT_DESCRIPTOR
{
    uint8_t Bytes[RW_CRYPT_DESCRIPTOR_SIZE];
} RW_CRYPT_DESCRIPTOR;

typedef struct RW_CRYPT_BOOT_ENTRY
{
    RW_CRYPT_DESCRIPTOR Descriptor;
    uint8_t Key[VC_KEY_SIZE];
    uint32_t Unlocked;
    uint32_t Reserved;
    VC_META Meta;
} RW_CRYPT_BOOT_ENTRY;

typedef struct RW_CRYPT_BOOT_STATE
{
    uint32_t Magic;
    uint32_t Version;
    uint32_t Size;
    uint32_t Count;
    RW_CRYPT_BOOT_ENTRY Entries[RW_CRYPT_MAX_VOLUMES];
} RW_CRYPT_BOOT_STATE;

typedef struct RW_CRYPT_REQUEST
{
    uint32_t Version;
    uint32_t Size;
    RW_CRYPT_DESCRIPTOR Descriptor;
    uint8_t Key[VC_KEY_SIZE];
    uint32_t HeaderSize;
    uint32_t Reserved;
} RW_CRYPT_REQUEST;

typedef struct RW_CRYPT_VOLUME_STATUS
{
    RW_CRYPT_DESCRIPTOR Descriptor;
    uint32_t Flags;
    uint32_t Reserved;
} RW_CRYPT_VOLUME_STATUS;

typedef struct RW_CRYPT_STATUS
{
    uint32_t Version;
    uint32_t Size;
    uint32_t Count;
    uint32_t Reserved;
    RW_CRYPT_VOLUME_STATUS Volumes[RW_CRYPT_MAX_VOLUMES];
} RW_CRYPT_STATUS;

int RwCryptIndexCreate(const VC_OPS *Ops, void *Buffer, uint64_t Sequence,
                       const RW_CRYPT_DESCRIPTOR *Volumes, uint32_t Count);
int RwCryptIndexRead(const VC_OPS *Ops, const void *Buffer, size_t Size,
                     uint64_t *Sequence, uint32_t *Count);
int RwCryptDescriptorValid(const VC_OPS *Ops, const RW_CRYPT_DESCRIPTOR *Descriptor);
int RwCryptDescriptorSetSystem(const VC_OPS *Ops, RW_CRYPT_DESCRIPTOR *Descriptor);
int RwCryptDescriptorSystem(const RW_CRYPT_DESCRIPTOR *Descriptor);
int RwCryptDescriptorCreate(const VC_OPS *Ops, RW_CRYPT_DESCRIPTOR *Descriptor,
                            uint32_t Style, uint32_t Signature, uint64_t Offset, uint64_t Size,
                            const void *DiskGuid, const void *PartitionGuid, const char *Uuid);
int RwCryptDescriptorMatch(const RW_CRYPT_DESCRIPTOR *Descriptor, uint32_t Style,
                           uint32_t Signature, uint64_t Offset, uint64_t Size,
                           const void *DiskGuid, const void *PartitionGuid);
uint64_t RwCryptGet64(const uint8_t *Bytes);
uint32_t RwCryptGet32(const uint8_t *Bytes);
void RwCryptHeaderPath(const RW_CRYPT_DESCRIPTOR *Descriptor, char Path[96]);
