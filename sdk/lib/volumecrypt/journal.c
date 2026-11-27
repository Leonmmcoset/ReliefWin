/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Authenticated write-ahead journal for offline in-place conversion
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "internal.h"

#define VC_RECORD_SIZE 4096
#define VC_RECORD_MAC 4064

typedef struct VC_RECORD
{
    uint64_t Sequence;
    uint64_t Total;
    uint64_t Completed;
    uint32_t Pending;
    uint32_t Decrypt;
    uint8_t Hash[32];
    unsigned Index;
} VC_RECORD;

static int
VcJournalMac(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE],
             uint8_t *Record, uint8_t Mac[32])
{
    uint8_t JournalKey[32], Context[80] = {0}, Saved[32];
    int Result;
    memcpy(Context, "ReliefWin conversion journal v1", 30);
    memcpy(Context + 32, Meta->Uuid, 40);
    if (!Ops->Hmac(Key, VC_KEY_SIZE, Context, sizeof(Context), JournalKey))
        return 0;
    memcpy(Saved, Record + VC_RECORD_MAC, 32);
    memset(Record + VC_RECORD_MAC, 0, 32);
    Result = Ops->Hmac(JournalKey, sizeof(JournalKey), Record, VC_RECORD_SIZE, Mac);
    memcpy(Record + VC_RECORD_MAC, Saved, 32);
    VcWipe(JournalKey, sizeof(JournalKey));
    VcWipe(Saved, sizeof(Saved));
    return Result;
}

static int
VcRecordDecode(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE],
               const uint8_t Target[32], uint64_t VolumeSize, uint8_t *Buffer, VC_RECORD *Record)
{
    uint8_t Mac[32];
    int Valid;
    if (memcmp(Buffer, "RWCVJNL1", 8) || VcBe32(Buffer + 8) != 1 ||
        VcBe32(Buffer + 12) != VC_RECORD_SIZE ||
        memcmp(Buffer + 48, Meta->Uuid, 40) || memcmp(Buffer + 88, Target, 32))
        return 0;
    Valid = VcJournalMac(Ops, Meta, Key, Buffer, Mac) && VcEqual(Mac, Buffer + VC_RECORD_MAC, 32);
    VcWipe(Mac, sizeof(Mac));
    if (!Valid)
        return 0;
    Record->Sequence = VcBe64(Buffer + 16);
    Record->Total = VcBe64(Buffer + 24);
    Record->Completed = VcBe64(Buffer + 32);
    Record->Pending = VcBe32(Buffer + 40);
    Record->Decrypt = VcBe32(Buffer + 44);
    memcpy(Record->Hash, Buffer + 120, 32);
    return Record->Sequence && Record->Total == VolumeSize && VolumeSize && !(VolumeSize % 512) &&
           Record->Completed <= VolumeSize && !(Record->Completed % 512) &&
           Record->Pending <= VC_JOURNAL_CHUNK && !(Record->Pending % 512) &&
           Record->Pending <= VolumeSize - Record->Completed && Record->Decrypt <= 1;
}

static VC_RESULT
VcRecordRead(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
             const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32], uint64_t VolumeSize,
             uint8_t *Buffer, VC_RECORD *Record)
{
    VC_RECORD A, B;
    int ValidA, ValidB;
    if (Journal->Size != VC_JOURNAL_SIZE ||
        !VcRead(Journal, 0, Buffer, 2 * VC_RECORD_SIZE))
        return VcIoError;
    ValidA = VcRecordDecode(Ops, Meta, Key, Target, VolumeSize, Buffer, &A);
    ValidB = VcRecordDecode(Ops, Meta, Key, Target, VolumeSize, Buffer + VC_RECORD_SIZE, &B);
    if (!ValidA && !ValidB)
        return VcInvalid;
    if (ValidA && ValidB && A.Sequence == B.Sequence &&
        memcmp(Buffer, Buffer + VC_RECORD_SIZE, VC_RECORD_SIZE))
        return VcConflict;
    if (!ValidB || (ValidA && A.Sequence >= B.Sequence))
    {
        *Record = A;
        Record->Index = 0;
    }
    else
    {
        *Record = B;
        Record->Index = 1;
    }
    return VcOk;
}

static VC_RESULT
VcRecordWrite(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
              const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
              uint8_t *Buffer, VC_RECORD *Record)
{
    uint8_t Mac[32];
    unsigned Copy;
    if (Record->Sequence == UINT64_MAX)
        return VcConflict;
    ++Record->Sequence;
    Record->Index ^= 1;
    memset(Buffer, 0, VC_RECORD_SIZE);
    memcpy(Buffer, "RWCVJNL1", 8);
    VcPut32(Buffer + 8, 1);
    VcPut32(Buffer + 12, VC_RECORD_SIZE);
    VcPut64(Buffer + 16, Record->Sequence);
    VcPut64(Buffer + 24, Record->Total);
    VcPut64(Buffer + 32, Record->Completed);
    VcPut32(Buffer + 40, Record->Pending);
    VcPut32(Buffer + 44, Record->Decrypt);
    memcpy(Buffer + 48, Meta->Uuid, 40);
    memcpy(Buffer + 88, Target, 32);
    memcpy(Buffer + 120, Record->Hash, 32);
    if (!VcJournalMac(Ops, Meta, Key, Buffer, Mac))
        return VcCryptoError;
    memcpy(Buffer + VC_RECORD_MAC, Mac, sizeof(Mac));
    VcWipe(Mac, sizeof(Mac));
    for (Copy = 0; Copy != 2; ++Copy)
    {
        if (!VcWrite(Journal, (Record->Index ^ Copy) * VC_RECORD_SIZE, Buffer, VC_RECORD_SIZE) ||
            !Journal->Flush || !Journal->Flush(Journal->Context))
            return VcIoError;
    }
    return VcOk;
}

static void
VcProgress(const VC_RECORD *Record, VC_PROGRESS *Progress)
{
    Progress->Completed = Record->Completed;
    Progress->Total = Record->Total;
    Progress->Sequence = Record->Sequence;
    Progress->Decrypt = Record->Decrypt;
    Progress->Pending = Record->Pending;
}

VC_RESULT
VcJournalBegin(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
               const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
               uint64_t VolumeSize, int Decrypt)
{
    VC_RECORD Record;
    uint8_t *Buffer;
    size_t i;
    VC_RESULT Result;
    if (!Meta->Incomplete || Meta->PayloadOffset || !VolumeSize || VolumeSize % 512 ||
        (Meta->PayloadSize && Meta->PayloadSize != VolumeSize) ||
        Journal->Size != VC_JOURNAL_SIZE || (Decrypt != 0 && Decrypt != 1))
        return VcInvalid;
    Result = VcCheckKey(Ops, Meta, Key);
    if (Result != VcOk)
        return Result;
    Buffer = Ops->Allocate(Ops->Context, 2 * VC_RECORD_SIZE);
    if (!Buffer)
        return VcNoMemory;
    if (!VcRead(Journal, 0, Buffer, 2 * VC_RECORD_SIZE))
        Result = VcIoError;
    else
    {
        Result = VcOk;
        for (i = 0; i != 2 * VC_RECORD_SIZE; ++i)
        {
            if (Buffer[i])
            {
                Result = VcConflict;
                break;
            }
        }
        if (Result == VcOk)
        {
            memset(&Record, 0, sizeof(Record));
            Record.Total = VolumeSize;
            Record.Decrypt = !!Decrypt;
            Record.Index = 1;
            Result = VcRecordWrite(Ops, Journal, Meta, Key, Target, Buffer, &Record);
            if (Result == VcOk)
                Result = VcRecordWrite(Ops, Journal, Meta, Key, Target, Buffer, &Record);
        }
    }
    Ops->Free(Ops->Context, Buffer, 2 * VC_RECORD_SIZE);
    return Result;
}

VC_RESULT
VcJournalRead(const VC_OPS *Ops, const VC_IO *Journal, const VC_META *Meta,
              const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
              uint64_t VolumeSize, VC_PROGRESS *Progress)
{
    VC_RECORD Record;
    uint8_t *Buffer = Ops->Allocate(Ops->Context, 2 * VC_RECORD_SIZE);
    VC_RESULT Result;
    memset(Progress, 0, sizeof(*Progress));
    if (!Buffer)
        return VcNoMemory;
    Result = VcRecordRead(Ops, Journal, Meta, Key, Target, VolumeSize, Buffer, &Record);
    if (Result == VcOk)
        VcProgress(&Record, Progress);
    Ops->Free(Ops->Context, Buffer, 2 * VC_RECORD_SIZE);
    return Result;
}

VC_RESULT
VcConvertStep(const VC_OPS *Ops, const VC_IO *Journal, const VC_IO *Volume,
              const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE], const uint8_t Target[32],
              VC_PROGRESS *Progress)
{
    VC_RECORD Record;
    uint8_t *Buffer, *Data, Hash[32];
    VC_RESULT Result;
    memset(Progress, 0, sizeof(*Progress));
    if (!Meta->Incomplete || Meta->PayloadOffset || !Volume->Flush ||
        !Volume->Size || Volume->Size % 512 ||
        (Meta->PayloadSize && Meta->PayloadSize != Volume->Size))
        return VcInvalid;
    Buffer = Ops->Allocate(Ops->Context, 2 * VC_RECORD_SIZE + VC_JOURNAL_CHUNK);
    if (!Buffer)
        return VcNoMemory;
    Data = Buffer + 2 * VC_RECORD_SIZE;
    Result = VcRecordRead(Ops, Journal, Meta, Key, Target, Volume->Size, Buffer, &Record);
    if (Result != VcOk)
        goto Done;
    VcProgress(&Record, Progress);
    if (Record.Completed == Record.Total)
    {
        Result = VcFinished;
        goto Done;
    }
    if (!Record.Pending)
    {
        uint64_t Remaining = Record.Total - Record.Completed;
        Result = VcRecordWrite(Ops, Journal, Meta, Key, Target, Buffer, &Record);
        if (Result != VcOk)
            goto Done;
        Record.Pending = Remaining > VC_JOURNAL_CHUNK ? VC_JOURNAL_CHUNK : (uint32_t)Remaining;
        if (!VcRead(Volume, Record.Completed, Data, Record.Pending))
        {
            Result = VcIoError;
            goto Done;
        }
        if (!Record.Decrypt)
        {
            Result = VcCrypt(Ops, Meta, Key, Record.Completed, Data, Record.Pending, 1);
            if (Result != VcOk)
                goto Done;
        }
        if (!Ops->Hash(Data, Record.Pending, Record.Hash))
        {
            Result = VcCryptoError;
            goto Done;
        }
        if (!VcWrite(Journal, 2 * VC_RECORD_SIZE, Data, Record.Pending) ||
            !Journal->Flush || !Journal->Flush(Journal->Context))
        {
            Result = VcIoError;
            goto Done;
        }
        Result = VcRecordWrite(Ops, Journal, Meta, Key, Target, Buffer, &Record);
        if (Result != VcOk)
            goto Done;
    }
    else
    {
        if (!VcRead(Journal, 2 * VC_RECORD_SIZE, Data, Record.Pending))
        {
            Result = VcIoError;
            goto Done;
        }
        if (!Ops->Hash(Data, Record.Pending, Hash))
        {
            Result = VcCryptoError;
            goto Done;
        }
        if (!VcEqual(Hash, Record.Hash, 32))
        {
            Result = VcInvalid;
            goto Done;
        }
    }
    if (Record.Decrypt)
    {
        Result = VcCrypt(Ops, Meta, Key, Record.Completed, Data, Record.Pending, 0);
        if (Result != VcOk)
            goto Done;
    }
    if (!VcWrite(Volume, Record.Completed, Data, Record.Pending) || !Volume->Flush(Volume->Context))
    {
        Result = VcIoError;
        goto Done;
    }
    Record.Completed += Record.Pending;
    Record.Pending = 0;
    memset(Record.Hash, 0, 32);
    Result = VcRecordWrite(Ops, Journal, Meta, Key, Target, Buffer, &Record);
    if (Result == VcOk)
        VcProgress(&Record, Progress);
Done:
    VcWipe(Buffer, 2 * VC_RECORD_SIZE + VC_JOURNAL_CHUNK);
    Ops->Free(Ops->Context, Buffer, 2 * VC_RECORD_SIZE + VC_JOURNAL_CHUNK);
    VcWipe(Hash, sizeof(Hash));
    return Result;
}
