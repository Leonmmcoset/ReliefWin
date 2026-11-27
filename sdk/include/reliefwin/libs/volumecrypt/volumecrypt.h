/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Portable LUKS2 and offline conversion contracts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#define VC_KEY_SIZE 64
#define VC_SALT_SIZE 32
#define VC_MAX_SLOTS 32
#define VC_HEADER_SIZE 16384
#define VC_METADATA_SIZE 1048576
#define VC_AF_STRIPES 4000
#define VC_SLOT_SIZE 258048
#define VC_JOURNAL_CHUNK 1048576
#define VC_JOURNAL_SIZE (8192 + VC_JOURNAL_CHUNK)
#define VC_PASSWORD_ITERATIONS 600000
#define VC_MAX_ITERATIONS 10000000

typedef enum VC_RESULT
{
    VcOk,
    VcInvalid,
    VcUnsupported,
    VcNoMemory,
    VcIoError,
    VcWrongSecret,
    VcIncomplete,
    VcConflict,
    VcCryptoError,
    VcFinished
} VC_RESULT;

typedef struct VC_IO
{
    void *Context;
    uint64_t Size;
    int (*Read)(void *Context, uint64_t Offset, void *Buffer, size_t Size);
    int (*Write)(void *Context, uint64_t Offset, const void *Buffer, size_t Size);
    int (*Flush)(void *Context);
} VC_IO;

typedef struct VC_OPS
{
    void *Context;
    void *(*Allocate)(void *Context, size_t Size);
    void (*Free)(void *Context, void *Buffer, size_t Size);
    int (*Random)(void *Context, void *Buffer, size_t Size);
    int (*Hash)(const void *Buffer, size_t Size, uint8_t Digest[32]);
    int (*Hmac)(const void *Key, size_t KeySize, const void *Buffer, size_t Size, uint8_t Digest[32]);
    int (*Pbkdf2)(const void *Secret, size_t SecretSize, const void *Salt, size_t SaltSize,
                  uint32_t Iterations, void *Output, size_t Size);
    int (*Xts)(const uint8_t Key[VC_KEY_SIZE], uint64_t Sector, void *Buffer, size_t Size, int Encrypt);
} VC_OPS;

typedef struct VC_SLOT
{
    uint32_t Id;
    uint32_t Iterations;
    uint64_t Offset;
    uint64_t Size;
    uint8_t Salt[VC_SALT_SIZE];
} VC_SLOT;

typedef struct VC_META
{
    uint64_t Sequence;
    uint64_t HeaderSize;
    uint64_t KeyslotsSize;
    uint64_t PayloadOffset;
    uint64_t PayloadSize;
    uint64_t IvTweak;
    uint32_t DigestIterations;
    uint32_t SlotCount;
    uint32_t Incomplete;
    uint8_t DigestSalt[VC_SALT_SIZE];
    uint8_t Digest[32];
    char Uuid[40];
    VC_SLOT Slots[VC_MAX_SLOTS];
} VC_META;

typedef struct VC_PROGRESS
{
    uint64_t Completed;
    uint64_t Total;
    uint64_t Sequence;
    uint32_t Decrypt;
    uint32_t Pending;
} VC_PROGRESS;

void VcWipe(void *Buffer, size_t Size);
int VcEqual(const void *Left, const void *Right, size_t Size);
VC_RESULT VcLoad(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta);
VC_RESULT VcCreate(const VC_OPS *Ops, const VC_IO *Header,
                   const void *Password, size_t PasswordSize,
                   const void *Recovery, size_t RecoverySize,
                   uint8_t Key[VC_KEY_SIZE], VC_META *Meta);
VC_RESULT VcUnlock(const VC_OPS *Ops, const VC_IO *Header, const VC_META *Meta,
                   const void *Secret, size_t SecretSize, uint8_t Key[VC_KEY_SIZE]);
VC_RESULT VcCheckKey(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE]);
VC_RESULT VcChangeSecret(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta,
                         const uint8_t Key[VC_KEY_SIZE], uint32_t SlotId,
                         const void *Secret, size_t SecretSize);
VC_RESULT VcSetOffline(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta, int Offline);
VC_RESULT VcCrypt(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE],
                  uint64_t Offset, void *Buffer, size_t Size, int Encrypt);
VC_RESULT VcJournalBegin(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
                         const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
                         uint64_t VolumeSize, int Decrypt);
VC_RESULT VcJournalRead(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
                        const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
                        uint64_t VolumeSize, VC_PROGRESS *Progress);
VC_RESULT VcConvertStep(const VC_OPS *Ops, const VC_IO *Journal, const VC_IO *Volume,
                        const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE],
                        const uint8_t Target[32], VC_PROGRESS *Progress);
