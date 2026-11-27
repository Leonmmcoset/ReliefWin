/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     LUKS2 AES-XTS and PBKDF2 detached metadata
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "internal.h"

void
VcWipe(void *Buffer, size_t Size)
{
    volatile uint8_t *Bytes = Buffer;
    while (Size--)
        *Bytes++ = 0;
}

int
VcEqual(const void *Left, const void *Right, size_t Size)
{
    const volatile uint8_t *A = Left, *B = Right;
    unsigned Difference = 0;
    while (Size--)
        Difference |= *A++ ^ *B++;
    return Difference == 0;
}

uint32_t
VcBe32(const uint8_t *Data)
{
    return ((uint32_t)Data[0] << 24) | ((uint32_t)Data[1] << 16) |
           ((uint32_t)Data[2] << 8) | Data[3];
}

uint64_t
VcBe64(const uint8_t *Data)
{
    return ((uint64_t)VcBe32(Data) << 32) | VcBe32(Data + 4);
}

void
VcPut32(uint8_t *Data, uint32_t Value)
{
    Data[0] = (uint8_t)(Value >> 24);
    Data[1] = (uint8_t)(Value >> 16);
    Data[2] = (uint8_t)(Value >> 8);
    Data[3] = (uint8_t)Value;
}

void
VcPut64(uint8_t *Data, uint64_t Value)
{
    VcPut32(Data, (uint32_t)(Value >> 32));
    VcPut32(Data + 4, (uint32_t)Value);
}

int
VcRead(const VC_IO *Io, uint64_t Offset, void *Buffer, size_t Size)
{
    return Offset <= Io->Size && Size <= Io->Size - Offset && Io->Read &&
           Io->Read(Io->Context, Offset, Buffer, Size);
}

int
VcWrite(const VC_IO *Io, uint64_t Offset, const void *Buffer, size_t Size)
{
    return Offset <= Io->Size && Size <= Io->Size - Offset && Io->Write &&
           Io->Write(Io->Context, Offset, Buffer, Size);
}

static int
VcZero(const void *Buffer, size_t Size)
{
    const uint8_t *Bytes = Buffer;
    while (Size--)
        if (*Bytes++)
            return 0;
    return 1;
}

static int
VcHeaderSize(uint64_t Size)
{
    return Size >= 16384 && Size <= 4194304 && !(Size & (Size - 1));
}

static int
VcFixedString(const uint8_t *Data, size_t Size, const char *String)
{
    size_t Length = strlen(String);
    return Length < Size && !memcmp(Data, String, Length + 1) &&
           VcZero(Data + Length + 1, Size - Length - 1);
}

static int
VcHeaderValid(const VC_OPS *Ops, uint8_t *Buffer, uint64_t Size, uint64_t Offset)
{
    uint8_t Digest[32], Saved[64];
    int Valid;
    if (memcmp(Buffer, Offset ? "SKUL\xba\xbe" : "LUKS\xba\xbe", 6) ||
        Buffer[6] != 0 || Buffer[7] != 2 || VcBe64(Buffer + 8) != Size ||
        VcBe64(Buffer + 256) != Offset ||
        !VcFixedString(Buffer + 72, 32, "sha256") ||
        !VcZero(Buffer + 264, 184) || !VcZero(Buffer + 512, 3584) ||
        !memchr(Buffer + 24, 0, 48) || !memchr(Buffer + 168, 0, 40) ||
        !memchr(Buffer + 208, 0, 48))
        return 0;
    memcpy(Saved, Buffer + 448, 64);
    memset(Buffer + 448, 0, 64);
    Valid = Ops->Hash(Buffer, (size_t)Size, Digest) &&
            VcEqual(Digest, Saved, 32) && VcZero(Saved + 32, 32);
    memcpy(Buffer + 448, Saved, 64);
    VcWipe(Digest, sizeof(Digest));
    return Valid;
}

static VC_RESULT
VcReadNewest(const VC_OPS *Ops, const VC_IO *Io, uint8_t **Newest, uint64_t *Size)
{
    uint8_t Binary[4096], *Candidate, *Best = NULL;
    uint64_t Offset, HeaderSize, Sequence = 0, BestSize = 0;
    unsigned i;
    VC_RESULT Result = VcInvalid;
    for (i = 0; i != 10; ++i)
    {
        Offset = i ? UINT64_C(8192) << i : 0;
        if (!VcRead(Io, Offset, Binary, sizeof(Binary)))
            continue;
        HeaderSize = VcBe64(Binary + 8);
        if (!VcHeaderSize(HeaderSize) || (Offset && HeaderSize != Offset) ||
            HeaderSize > Io->Size - Offset)
            continue;
        Candidate = Ops->Allocate(Ops->Context, (size_t)HeaderSize);
        if (!Candidate)
        {
            Result = VcNoMemory;
            goto Done;
        }
        if (!VcRead(Io, Offset, Candidate, (size_t)HeaderSize))
        {
            Ops->Free(Ops->Context, Candidate, (size_t)HeaderSize);
            Result = VcIoError;
            goto Done;
        }
        if (!VcHeaderValid(Ops, Candidate, HeaderSize, Offset))
        {
            Ops->Free(Ops->Context, Candidate, (size_t)HeaderSize);
            continue;
        }
        if (!Best || VcBe64(Candidate + 16) > Sequence)
        {
            if (Best)
                Ops->Free(Ops->Context, Best, (size_t)BestSize);
            Best = Candidate;
            BestSize = HeaderSize;
            Sequence = VcBe64(Candidate + 16);
        }
        else
        {
            if (Sequence == VcBe64(Candidate + 16) &&
                (BestSize != HeaderSize ||
                 memcmp(Best + 168, Candidate + 168, 40) ||
                 memcmp(Best + 4096, Candidate + 4096, (size_t)HeaderSize - 4096)))
            {
                Ops->Free(Ops->Context, Candidate, (size_t)HeaderSize);
                Result = VcConflict;
                goto Done;
            }
            Ops->Free(Ops->Context, Candidate, (size_t)HeaderSize);
        }
    }
    if (Best)
    {
        *Newest = Best;
        *Size = BestSize;
        return VcOk;
    }
Done:
    if (Best)
        Ops->Free(Ops->Context, Best, (size_t)BestSize);
    return Result;
}

static int
VcIs(const VC_JSON *Json, unsigned Object, const char *Name, const char *Value)
{
    return VcJsonString(Json, VcJsonMember(Json, Object, Name), Value);
}

static int
VcNumber(const VC_JSON *Json, unsigned Object, const char *Name, uint64_t *Value, char Type)
{
    return VcJsonNumber(Json, VcJsonMember(Json, Object, Name), Value, Type);
}

static int
VcObject(const VC_JSON *Json, unsigned Token, char Type)
{
    return Token && Json->Tokens[Token - 1].Type == Type;
}

static int
VcUuid(const char *Uuid)
{
    unsigned i;
    for (i = 0; i != 36; ++i)
    {
        char C = Uuid[i];
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (C != '-')
                return 0;
        }
        else if (!((C >= '0' && C <= '9') || (C >= 'a' && C <= 'f') || (C >= 'A' && C <= 'F')))
            return 0;
    }
    return Uuid[36] == 0;
}

static VC_RESULT
VcParseMeta(const VC_IO *Io, uint8_t *Header, uint64_t HeaderSize, VC_JSON *Json, VC_META *Meta)
{
    unsigned Config, Slots, Segments, Segment, Digests, Digest, Array, Token, i, j;
    unsigned Slot, Af, Area, Kdf;
    uint64_t Number, Id, Mask = 0, DigestMask = 0;
    const char *Text = (const char *)Header + 4096;
    const char *End = memchr(Text, 0, (size_t)HeaderSize - 4096);
    if (!End || !VcZero(End, (size_t)HeaderSize - 4096 - (size_t)(End - Text)) ||
        !VcJsonParse(Json, Text, (size_t)(End - Text)))
        return VcInvalid;
    memset(Meta, 0, sizeof(*Meta));
    Meta->Sequence = VcBe64(Header + 16);
    Meta->HeaderSize = HeaderSize;
    memcpy(Meta->Uuid, Header + 168, sizeof(Meta->Uuid));
    if (!VcUuid(Meta->Uuid))
        return VcInvalid;
    Config = VcJsonMember(Json, 1, "config");
    Slots = VcJsonMember(Json, 1, "keyslots");
    Digests = VcJsonMember(Json, 1, "digests");
    Segments = VcJsonMember(Json, 1, "segments");
    if (!VcObject(Json, Config, '{') || !VcObject(Json, Slots, '{') ||
        !VcObject(Json, Digests, '{') || !VcObject(Json, Segments, '{') ||
        !VcObject(Json, VcJsonMember(Json, 1, "tokens"), '{') ||
        !VcNumber(Json, Config, "json_size", &Number, '"') || Number != HeaderSize - 4096 ||
        !VcNumber(Json, Config, "keyslots_size", &Meta->KeyslotsSize, '"') ||
        Meta->KeyslotsSize % 4096 || Io->Size < 2 * HeaderSize ||
        Meta->KeyslotsSize > Io->Size - 2 * HeaderSize)
        return VcInvalid;
    Array = VcJsonMember(Json, Config, "requirements");
    if (Array)
    {
        if (!VcObject(Json, Array, '{') || Json->Tokens[Array - 1].Count != 1)
            return VcUnsupported;
        Array = VcJsonMember(Json, Array, "mandatory");
        if (!VcObject(Json, Array, '['))
            return VcInvalid;
        for (Token = Array + 1; Token < Json->Tokens[Array - 1].Next;
             Token = Json->Tokens[Token - 1].Next)
        {
            if (!VcJsonString(Json, Token, "offline-reencrypt"))
                return VcUnsupported;
            Meta->Incomplete = 1;
        }
    }
    if (Json->Tokens[Segments - 1].Count != 1 || Json->Tokens[Digests - 1].Count != 1)
        return VcUnsupported;
    Segment = VcJsonMember(Json, Segments, "0");
    Digest = VcJsonMember(Json, Digests, "0");
    if (!VcIs(Json, Segment, "type", "crypt") ||
        !VcIs(Json, Segment, "encryption", "aes-xts-plain64") ||
        !VcNumber(Json, Segment, "sector_size", &Number, 'n') || Number != 512 ||
        !VcNumber(Json, Segment, "offset", &Meta->PayloadOffset, '"') || Meta->PayloadOffset % 512 ||
        !VcNumber(Json, Segment, "iv_tweak", &Meta->IvTweak, '"'))
        return VcUnsupported;
    if (!VcIs(Json, Segment, "size", "dynamic") &&
        (!VcNumber(Json, Segment, "size", &Meta->PayloadSize, '"') || !Meta->PayloadSize ||
         Meta->PayloadSize % 512))
        return VcInvalid;
    Array = VcJsonMember(Json, Segment, "flags");
    if (Array && (!VcObject(Json, Array, '[') || Json->Tokens[Array - 1].Count))
        return VcUnsupported;
    if (!VcIs(Json, Digest, "type", "pbkdf2") || !VcIs(Json, Digest, "hash", "sha256") ||
        !VcNumber(Json, Digest, "iterations", &Number, 'n') || !Number || Number > VC_MAX_ITERATIONS ||
        !VcJsonBinary(Json, VcJsonMember(Json, Digest, "salt"), Meta->DigestSalt, 32) ||
        !VcJsonBinary(Json, VcJsonMember(Json, Digest, "digest"), Meta->Digest, 32))
        return VcUnsupported;
    Meta->DigestIterations = (uint32_t)Number;
    Array = VcJsonMember(Json, Digest, "segments");
    if (!VcObject(Json, Array, '[') || Json->Tokens[Array - 1].Count != 1 ||
        !VcJsonString(Json, Array + 1, "0"))
        return VcInvalid;
    if (!Json->Tokens[Slots - 1].Count || Json->Tokens[Slots - 1].Count > VC_MAX_SLOTS)
        return VcInvalid;
    for (Token = Slots + 1; Token < Json->Tokens[Slots - 1].Next;)
    {
        VC_SLOT *Entry = &Meta->Slots[Meta->SlotCount];
        if (!VcJsonNumber(Json, Token, &Id, '"') || Id >= VC_MAX_SLOTS || (Mask & (UINT64_C(1) << Id)))
            return VcInvalid;
        Mask |= UINT64_C(1) << Id;
        Entry->Id = (uint32_t)Id;
        Slot = Token + 1;
        Af = VcJsonMember(Json, Slot, "af");
        Area = VcJsonMember(Json, Slot, "area");
        Kdf = VcJsonMember(Json, Slot, "kdf");
        if (!VcIs(Json, Slot, "type", "luks2") ||
            !VcNumber(Json, Slot, "key_size", &Number, 'n') || Number != VC_KEY_SIZE ||
            !VcIs(Json, Af, "type", "luks1") || !VcIs(Json, Af, "hash", "sha256") ||
            !VcNumber(Json, Af, "stripes", &Number, 'n') || Number != VC_AF_STRIPES ||
            !VcIs(Json, Area, "type", "raw") || !VcIs(Json, Area, "encryption", "aes-xts-plain64") ||
            !VcNumber(Json, Area, "key_size", &Number, 'n') || Number != VC_KEY_SIZE ||
            !VcIs(Json, Kdf, "type", "pbkdf2") || !VcIs(Json, Kdf, "hash", "sha256") ||
            !VcNumber(Json, Kdf, "iterations", &Number, 'n') || !Number || Number > VC_MAX_ITERATIONS ||
            !VcJsonBinary(Json, VcJsonMember(Json, Kdf, "salt"), Entry->Salt, 32))
            return VcUnsupported;
        Entry->Iterations = (uint32_t)Number;
        if (!VcNumber(Json, Area, "offset", &Entry->Offset, '"') || Entry->Offset % 4096 ||
            !VcNumber(Json, Area, "size", &Entry->Size, '"') || Entry->Size % 4096 ||
            Entry->Size < VC_KEY_SIZE * VC_AF_STRIPES || Entry->Size > VC_METADATA_SIZE ||
            Entry->Offset < 2 * HeaderSize || Entry->Offset > 2 * HeaderSize + Meta->KeyslotsSize ||
            Entry->Size > 2 * HeaderSize + Meta->KeyslotsSize - Entry->Offset)
            return VcInvalid;
        for (i = 0; i < Meta->SlotCount; ++i)
        {
            const VC_SLOT *Other = &Meta->Slots[i];
            if (Entry->Offset < Other->Offset + Other->Size && Other->Offset < Entry->Offset + Entry->Size)
                return VcInvalid;
        }
        ++Meta->SlotCount;
        Token = Json->Tokens[Slot - 1].Next;
    }
    Array = VcJsonMember(Json, Digest, "keyslots");
    if (!VcObject(Json, Array, '[') || Json->Tokens[Array - 1].Count != Meta->SlotCount)
        return VcInvalid;
    for (j = Array + 1; j < Json->Tokens[Array - 1].Next; j = Json->Tokens[j - 1].Next)
    {
        if (!VcJsonNumber(Json, j, &Id, '"') || Id >= VC_MAX_SLOTS || (DigestMask & (UINT64_C(1) << Id)))
            return VcInvalid;
        DigestMask |= UINT64_C(1) << Id;
    }
    return DigestMask == Mask ? VcOk : VcInvalid;
}

VC_RESULT
VcLoad(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta)
{
    VC_JSON *Json;
    uint8_t *Buffer;
    uint64_t Size;
    VC_RESULT Result;
    memset(Meta, 0, sizeof(*Meta));
    Result = VcReadNewest(Ops, Header, &Buffer, &Size);
    if (Result != VcOk)
        return Result;
    Json = Ops->Allocate(Ops->Context, sizeof(*Json));
    if (!Json)
        Result = VcNoMemory;
    else
    {
        Result = VcParseMeta(Header, Buffer, Size, Json, Meta);
        Ops->Free(Ops->Context, Json, sizeof(*Json));
    }
    Ops->Free(Ops->Context, Buffer, (size_t)Size);
    if (Result != VcOk)
        VcWipe(Meta, sizeof(*Meta));
    return Result;
}

static int
VcDiffuse(const VC_OPS *Ops, uint8_t Data[VC_KEY_SIZE])
{
    uint8_t Input[36], Digest[32];
    unsigned i;
    for (i = 0; i != 2; ++i)
    {
        VcPut32(Input, i);
        memcpy(Input + 4, Data + i * 32, 32);
        if (!Ops->Hash(Input, sizeof(Input), Digest))
        {
            VcWipe(Input, sizeof(Input));
            VcWipe(Digest, sizeof(Digest));
            return 0;
        }
        memcpy(Data + i * 32, Digest, 32);
    }
    VcWipe(Input, sizeof(Input));
    VcWipe(Digest, sizeof(Digest));
    return 1;
}

static int
VcAf(const VC_OPS *Ops, uint8_t *Area, uint8_t Key[VC_KEY_SIZE], int Split)
{
    uint8_t Data[VC_KEY_SIZE] = {0};
    unsigned i, j;
    int Result = 0;
    if (Split && !Ops->Random(Ops->Context, Area, VC_KEY_SIZE * (VC_AF_STRIPES - 1)))
        goto Done;
    for (i = 0; i != VC_AF_STRIPES - 1; ++i)
    {
        for (j = 0; j != VC_KEY_SIZE; ++j)
            Data[j] ^= Area[i * VC_KEY_SIZE + j];
        if (!VcDiffuse(Ops, Data))
            goto Done;
    }
    for (j = 0; j != VC_KEY_SIZE; ++j)
    {
        if (Split)
            Area[(VC_AF_STRIPES - 1) * VC_KEY_SIZE + j] = Data[j] ^ Key[j];
        else
            Key[j] = Data[j] ^ Area[(VC_AF_STRIPES - 1) * VC_KEY_SIZE + j];
    }
    Result = 1;
Done:
    VcWipe(Data, sizeof(Data));
    return Result;
}

VC_RESULT
VcCheckKey(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE])
{
    uint8_t Digest[32];
    VC_RESULT Result = VcCryptoError;
    if (Ops->Pbkdf2(Key, VC_KEY_SIZE, Meta->DigestSalt, 32, Meta->DigestIterations, Digest, 32))
        Result = VcEqual(Digest, Meta->Digest, 32) ? VcOk : VcWrongSecret;
    VcWipe(Digest, sizeof(Digest));
    return Result;
}

VC_RESULT
VcUnlock(const VC_OPS *Ops, const VC_IO *Header, const VC_META *Meta,
         const void *Secret, size_t SecretSize, uint8_t Key[VC_KEY_SIZE])
{
    uint8_t Derived[VC_KEY_SIZE], *Area;
    VC_RESULT Result = VcWrongSecret;
    uint32_t i;
    VcWipe(Key, VC_KEY_SIZE);
    if (!Secret || !SecretSize || SecretSize > 1024 || !Meta->SlotCount)
        return VcInvalid;
    for (i = 0; i < Meta->SlotCount; ++i)
    {
        const VC_SLOT *Slot = &Meta->Slots[i];
        Area = Ops->Allocate(Ops->Context, (size_t)Slot->Size);
        if (!Area)
        {
            Result = VcNoMemory;
            break;
        }
        if (!VcRead(Header, Slot->Offset, Area, (size_t)Slot->Size))
            Result = VcIoError;
        else if (!Ops->Pbkdf2(Secret, SecretSize, Slot->Salt, 32, Slot->Iterations, Derived, VC_KEY_SIZE) ||
                 !Ops->Xts(Derived, 0, Area, (size_t)Slot->Size, 0) || !VcAf(Ops, Area, Key, 0))
            Result = VcCryptoError;
        else
            Result = VcCheckKey(Ops, Meta, Key);
        VcWipe(Derived, sizeof(Derived));
        VcWipe(Area, (size_t)Slot->Size);
        Ops->Free(Ops->Context, Area, (size_t)Slot->Size);
        if (Result != VcWrongSecret)
            break;
        VcWipe(Key, VC_KEY_SIZE);
    }
    if (Result != VcOk)
        VcWipe(Key, VC_KEY_SIZE);
    return Result;
}

static int
VcAppend(char **Cursor, const char *End, const char *Text)
{
    size_t Size = strlen(Text);
    if ((size_t)(End - *Cursor) <= Size)
        return 0;
    memcpy(*Cursor, Text, Size + 1);
    *Cursor += Size;
    return 1;
}

static int
VcAppendNumber(char **Cursor, const char *End, uint64_t Number)
{
    char Text[21];
    unsigned i = 20;
    Text[i] = 0;
    do
    {
        Text[--i] = '0' + Number % 10;
        Number /= 10;
    } while (Number);
    return VcAppend(Cursor, End, Text + i);
}

static int
VcBuildJson(uint8_t *Buffer, const VC_META *Meta)
{
    char *Cursor = (char *)Buffer + 4096;
    const char *End = (const char *)Buffer + Meta->HeaderSize;
    char Base64[45];
    unsigned i;
#define VC_ADD(Text) do { if (!VcAppend(&Cursor, End, (Text))) return 0; } while (0)
#define VC_NUM(Number) do { if (!VcAppendNumber(&Cursor, End, (Number))) return 0; } while (0)
    VC_ADD("{\"keyslots\":{");
    for (i = 0; i < Meta->SlotCount; ++i)
    {
        const VC_SLOT *Slot = &Meta->Slots[i];
        if (i)
            VC_ADD(",");
        VC_ADD("\""); VC_NUM(Slot->Id);
        VC_ADD("\":{\"type\":\"luks2\",\"key_size\":64,\"af\":{\"type\":\"luks1\",\"stripes\":4000,\"hash\":\"sha256\"},"
               "\"area\":{\"type\":\"raw\",\"encryption\":\"aes-xts-plain64\",\"key_size\":64,\"offset\":\"");
        VC_NUM(Slot->Offset); VC_ADD("\",\"size\":\""); VC_NUM(Slot->Size);
        VC_ADD("\"},\"kdf\":{\"type\":\"pbkdf2\",\"hash\":\"sha256\",\"iterations\":");
        VC_NUM(Slot->Iterations); VC_ADD(",\"salt\":\"");
        VcBase64(Slot->Salt, 32, Base64); VC_ADD(Base64); VC_ADD("\"}}");
    }
    VC_ADD("},\"tokens\":{},\"segments\":{\"0\":{\"type\":\"crypt\",\"offset\":\"");
    VC_NUM(Meta->PayloadOffset); VC_ADD("\",\"iv_tweak\":\""); VC_NUM(Meta->IvTweak);
    VC_ADD("\",\"size\":\"");
    if (Meta->PayloadSize)
        VC_NUM(Meta->PayloadSize);
    else
        VC_ADD("dynamic");
    VC_ADD("\",\"encryption\":\"aes-xts-plain64\",\"sector_size\":512}},"
           "\"digests\":{\"0\":{\"type\":\"pbkdf2\",\"keyslots\":[");
    for (i = 0; i < Meta->SlotCount; ++i)
    {
        if (i)
            VC_ADD(",");
        VC_ADD("\""); VC_NUM(Meta->Slots[i].Id); VC_ADD("\"");
    }
    VC_ADD("],\"segments\":[\"0\"],\"hash\":\"sha256\",\"iterations\":");
    VC_NUM(Meta->DigestIterations); VC_ADD(",\"salt\":\"");
    VcBase64(Meta->DigestSalt, 32, Base64); VC_ADD(Base64); VC_ADD("\",\"digest\":\"");
    VcBase64(Meta->Digest, 32, Base64); VC_ADD(Base64); VC_ADD("\"}},\"config\":{\"json_size\":\"");
    VC_NUM(Meta->HeaderSize - 4096); VC_ADD("\",\"keyslots_size\":\""); VC_NUM(Meta->KeyslotsSize);
    VC_ADD("\",\"requirements\":{\"mandatory\":[");
    if (Meta->Incomplete)
        VC_ADD("\"offline-reencrypt\"");
    VC_ADD("]}}}");
#undef VC_ADD
#undef VC_NUM
    return 1;
}

static VC_RESULT
VcWriteHeaders(const VC_OPS *Ops, const VC_IO *Io, VC_META *Meta, uint8_t *Buffer, unsigned First)
{
    unsigned Copy;
    uint8_t Digest[32];
    for (Copy = 0; Copy != 2; ++Copy)
    {
        unsigned Index = First ^ Copy;
        uint64_t Offset = Index ? Meta->HeaderSize : 0;
        memset(Buffer, 0, 4096);
        memcpy(Buffer, Index ? "SKUL\xba\xbe" : "LUKS\xba\xbe", 6);
        Buffer[7] = 2;
        VcPut64(Buffer + 8, Meta->HeaderSize);
        VcPut64(Buffer + 16, Meta->Sequence);
        memcpy(Buffer + 24, "ReliefWin", 9);
        memcpy(Buffer + 72, "sha256", 6);
        memcpy(Buffer + 168, Meta->Uuid, 40);
        VcPut64(Buffer + 256, Offset);
        if (!Ops->Random(Ops->Context, Buffer + 104, 64) ||
            !Ops->Hash(Buffer, (size_t)Meta->HeaderSize, Digest))
            return VcCryptoError;
        memcpy(Buffer + 448, Digest, 32);
        if (!VcWrite(Io, Offset, Buffer, (size_t)Meta->HeaderSize) || !Io->Flush(Io->Context))
            return VcIoError;
    }
    return VcOk;
}

VC_RESULT
VcCreate(const VC_OPS *Ops, const VC_IO *Io,
         const void *Password, size_t PasswordSize, const void *Recovery, size_t RecoverySize,
         uint8_t Key[VC_KEY_SIZE], VC_META *Meta)
{
    uint8_t *Area, *Buffer = NULL, Uuid[16], Derived[VC_KEY_SIZE];
    unsigned i, j;
    VC_RESULT Result = VcCryptoError;
    const char Hex[] = "0123456789abcdef";
    const void *Secrets[2] = {Password, Recovery};
    size_t SecretSizes[2] = {PasswordSize, RecoverySize};
    VcWipe(Key, VC_KEY_SIZE);
    memset(Meta, 0, sizeof(*Meta));
    if (Io->Size != VC_METADATA_SIZE || !Io->Flush || !Password || !Recovery ||
        !PasswordSize || PasswordSize > 1024 || !RecoverySize || RecoverySize > 1024 ||
        (PasswordSize == RecoverySize && VcEqual(Password, Recovery, PasswordSize)))
        return VcInvalid;
    Area = Ops->Allocate(Ops->Context, VC_SLOT_SIZE);
    if (!Area)
        return VcNoMemory;
    Buffer = Ops->Allocate(Ops->Context, VC_HEADER_SIZE);
    if (!Buffer)
    {
        Result = VcNoMemory;
        goto Done;
    }
    Meta->HeaderSize = VC_HEADER_SIZE;
    Meta->KeyslotsSize = VC_METADATA_SIZE - 2 * VC_HEADER_SIZE;
    Meta->Sequence = 1;
    Meta->Incomplete = 1;
    Meta->SlotCount = 2;
    Meta->DigestIterations = 100000;
    if (!Ops->Random(Ops->Context, Key, VC_KEY_SIZE) || VcEqual(Key, Key + 32, 32) ||
        !Ops->Random(Ops->Context, Uuid, sizeof(Uuid)) ||
        !Ops->Random(Ops->Context, Meta->DigestSalt, 32) ||
        !Ops->Pbkdf2(Key, VC_KEY_SIZE, Meta->DigestSalt, 32, Meta->DigestIterations, Meta->Digest, 32))
        goto Done;
    Uuid[6] = (Uuid[6] & 15) | 0x40;
    Uuid[8] = (Uuid[8] & 63) | 0x80;
    for (i = 0, j = 0; i != sizeof(Uuid); ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            Meta->Uuid[j++] = '-';
        Meta->Uuid[j++] = Hex[Uuid[i] >> 4];
        Meta->Uuid[j++] = Hex[Uuid[i] & 15];
    }
    for (i = 0; i != 2; ++i)
    {
        VC_SLOT *Slot = &Meta->Slots[i];
        memset(Area, 0, VC_SLOT_SIZE);
        Slot->Id = i;
        Slot->Offset = 2 * VC_HEADER_SIZE + i * VC_SLOT_SIZE;
        Slot->Size = VC_SLOT_SIZE;
        Slot->Iterations = VC_PASSWORD_ITERATIONS;
        if (!Ops->Random(Ops->Context, Slot->Salt, 32) ||
            !Ops->Pbkdf2(Secrets[i], SecretSizes[i], Slot->Salt, 32, Slot->Iterations, Derived, VC_KEY_SIZE) ||
            !VcAf(Ops, Area, Key, 1) || !Ops->Xts(Derived, 0, Area, VC_SLOT_SIZE, 1))
            goto Done;
        if (!VcWrite(Io, Slot->Offset, Area, VC_SLOT_SIZE))
        {
            Result = VcIoError;
            goto Done;
        }
    }
    if (!Io->Flush(Io->Context))
    {
        Result = VcIoError;
        goto Done;
    }
    memset(Buffer, 0, VC_HEADER_SIZE);
    if (!VcBuildJson(Buffer, Meta))
    {
        Result = VcInvalid;
        goto Done;
    }
    Result = VcWriteHeaders(Ops, Io, Meta, Buffer, 1);
Done:
    VcWipe(Derived, sizeof(Derived));
    VcWipe(Area, VC_SLOT_SIZE);
    Ops->Free(Ops->Context, Area, VC_SLOT_SIZE);
    if (Buffer)
        Ops->Free(Ops->Context, Buffer, VC_HEADER_SIZE);
    if (Result != VcOk)
    {
        VcWipe(Key, VC_KEY_SIZE);
        VcWipe(Meta, sizeof(*Meta));
    }
    return Result;
}

VC_RESULT
VcChangeSecret(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta,
               const uint8_t Key[VC_KEY_SIZE], uint32_t SlotId,
               const void *Secret, size_t SecretSize)
{
    VC_META Current;
    uint8_t *Buffer = NULL, *Area = NULL, *Canonical = NULL, Derived[VC_KEY_SIZE], CopyKey[VC_KEY_SIZE];
    uint64_t Size, Offset, OldOffset;
    uint32_t i, SlotIndex = UINT32_MAX;
    unsigned First;
    VC_RESULT Result;
    if (!Secret || !SecretSize || SecretSize > 1024 || SlotId > 1 || !Header->Flush)
        return VcInvalid;
    Result = VcCheckKey(Ops, Meta, Key);
    if (Result != VcOk)
        return Result;
    Result = VcReadNewest(Ops, Header, &Buffer, &Size);
    if (Result != VcOk)
        return Result;
    Result = VcLoad(Ops, Header, &Current);
    if (Result != VcOk)
        goto Done;
    if (memcmp(Meta, &Current, sizeof(Current)) || Current.Sequence == UINT64_MAX)
    {
        Result = VcConflict;
        goto Done;
    }
    if (Current.HeaderSize != VC_HEADER_SIZE || Current.SlotCount != 2 ||
        Header->Size != VC_METADATA_SIZE || Current.Incomplete)
    {
        Result = VcUnsupported;
        goto Done;
    }
    Canonical = Ops->Allocate(Ops->Context, VC_HEADER_SIZE);
    Area = Ops->Allocate(Ops->Context, VC_SLOT_SIZE);
    if (!Canonical || !Area)
    {
        Result = VcNoMemory;
        goto Done;
    }
    memset(Canonical, 0, VC_HEADER_SIZE);
    if (!VcBuildJson(Canonical, &Current) ||
        memcmp(Buffer + 4096, Canonical + 4096, VC_HEADER_SIZE - 4096))
    {
        Result = VcUnsupported;
        goto Done;
    }
    for (i = 0; i != Current.SlotCount; ++i)
    {
        if (Current.Slots[i].Id > 1 || Current.Slots[i].Size != VC_SLOT_SIZE)
        {
            Result = VcUnsupported;
            goto Done;
        }
        if (Current.Slots[i].Id == SlotId)
            SlotIndex = i;
    }
    if (SlotIndex == UINT32_MAX)
    {
        Result = VcInvalid;
        goto Done;
    }
    for (Offset = 2 * VC_HEADER_SIZE; Offset + VC_SLOT_SIZE <= Header->Size; Offset += VC_SLOT_SIZE)
    {
        for (i = 0; i != Current.SlotCount; ++i)
            if (Offset < Current.Slots[i].Offset + Current.Slots[i].Size &&
                Current.Slots[i].Offset < Offset + VC_SLOT_SIZE)
                break;
        if (i == Current.SlotCount)
            break;
    }
    if (Offset + VC_SLOT_SIZE > Header->Size)
    {
        Result = VcUnsupported;
        goto Done;
    }
    First = VcBe64(Buffer + 256) ? 0 : 1;
    Result = VcWriteHeaders(Ops, Header, &Current, Buffer, First);
    if (Result != VcOk)
        goto Done;
    OldOffset = Current.Slots[SlotIndex].Offset;
    Current.Slots[SlotIndex].Offset = Offset;
    Current.Slots[SlotIndex].Iterations = VC_PASSWORD_ITERATIONS;
    memset(Area, 0, VC_SLOT_SIZE);
    memcpy(CopyKey, Key, VC_KEY_SIZE);
    if (!Ops->Random(Ops->Context, Current.Slots[SlotIndex].Salt, 32) ||
        !Ops->Pbkdf2(Secret, SecretSize, Current.Slots[SlotIndex].Salt, 32,
                     VC_PASSWORD_ITERATIONS, Derived, VC_KEY_SIZE) ||
        !VcAf(Ops, Area, CopyKey, 1) || !Ops->Xts(Derived, 0, Area, VC_SLOT_SIZE, 1))
    {
        Result = VcCryptoError;
        goto Done;
    }
    if (!VcWrite(Header, Offset, Area, VC_SLOT_SIZE) || !Header->Flush(Header->Context))
    {
        Result = VcIoError;
        goto Done;
    }
    ++Current.Sequence;
    memset(Buffer, 0, VC_HEADER_SIZE);
    if (!VcBuildJson(Buffer, &Current))
    {
        Result = VcInvalid;
        goto Done;
    }
    Result = VcWriteHeaders(Ops, Header, &Current, Buffer, 1);
    if (Result != VcOk)
        goto Done;
    *Meta = Current;
    memset(Area, 0, VC_SLOT_SIZE);
    if (!VcWrite(Header, OldOffset, Area, VC_SLOT_SIZE) || !Header->Flush(Header->Context))
        Result = VcIoError;
Done:
    if (Buffer) Ops->Free(Ops->Context, Buffer, (size_t)Size);
    if (Canonical) Ops->Free(Ops->Context, Canonical, VC_HEADER_SIZE);
    if (Area) Ops->Free(Ops->Context, Area, VC_SLOT_SIZE);
    VcWipe(Derived, sizeof(Derived));
    VcWipe(CopyKey, sizeof(CopyKey));
    return Result;
}

VC_RESULT
VcSetOffline(const VC_OPS *Ops, const VC_IO *Header, VC_META *Meta, int Offline)
{
    VC_META Current;
    VC_JSON *Json = NULL;
    uint8_t *Buffer;
    uint64_t Size;
    unsigned Config, Requirements, Mandatory, First;
    size_t Start, End, Length, Tail;
    const char *Value = Offline ? "[\"offline-reencrypt\"]" : "[]";
    VC_RESULT Result;
    if (!Header->Flush)
        return VcInvalid;
    Result = VcReadNewest(Ops, Header, &Buffer, &Size);
    if (Result != VcOk)
        return Result;
    Json = Ops->Allocate(Ops->Context, sizeof(*Json));
    if (!Json)
    {
        Result = VcNoMemory;
        goto Done;
    }
    Result = VcParseMeta(Header, Buffer, Size, Json, &Current);
    if (Result != VcOk)
        goto Done;
    if (Current.Sequence != Meta->Sequence || strcmp(Current.Uuid, Meta->Uuid) ||
        Current.Sequence == UINT64_MAX)
    {
        Result = VcConflict;
        goto Done;
    }
    if (Current.Incomplete == !!Offline)
        goto Done;
    First = VcBe64(Buffer + 256) ? 0 : 1;
    Config = VcJsonMember(Json, 1, "config");
    Requirements = VcJsonMember(Json, Config, "requirements");
    Mandatory = VcJsonMember(Json, Requirements, "mandatory");
    if (!Mandatory)
    {
        Start = Json->Tokens[Config - 1].End - 1;
        End = Start;
        Value = Offline ? ",\"requirements\":{\"mandatory\":[\"offline-reencrypt\"]}" : "";
    }
    else
    {
        Start = Json->Tokens[Mandatory - 1].Start;
        End = Json->Tokens[Mandatory - 1].End;
    }
    Length = strlen(Value);
    Tail = Json->Size - End + 1;
    if (4096 + Start + Length + Tail > Size)
    {
        Result = VcInvalid;
        goto Done;
    }
    memmove(Buffer + 4096 + Start + Length, Buffer + 4096 + End, Tail);
    memcpy(Buffer + 4096 + Start, Value, Length);
    memset(Buffer + 4096 + Start + Length + Tail, 0,
           (size_t)Size - 4096 - Start - Length - Tail);
    Current.Sequence++;
    Current.Incomplete = !!Offline;
    Result = VcWriteHeaders(Ops, Header, &Current, Buffer, First);
    if (Result == VcOk)
        *Meta = Current;
Done:
    if (Json)
        Ops->Free(Ops->Context, Json, sizeof(*Json));
    Ops->Free(Ops->Context, Buffer, (size_t)Size);
    return Result;
}

VC_RESULT
VcCrypt(const VC_OPS *Ops, const VC_META *Meta, const uint8_t Key[VC_KEY_SIZE],
        uint64_t Offset, void *Buffer, size_t Size, int Encrypt)
{
    uint64_t Sector;
    if (Offset % 512 || Size % 512 || Size > UINT64_MAX - Offset ||
        (Meta->PayloadSize && (Offset > Meta->PayloadSize || Size > Meta->PayloadSize - Offset)))
        return VcInvalid;
    Sector = Offset / 512;
    if (Sector > UINT64_MAX - Meta->IvTweak ||
        (Size && Size / 512 - 1 > UINT64_MAX - Sector - Meta->IvTweak))
        return VcInvalid;
    if (!Size)
        return VcOk;
    return Ops->Xts(Key, Sector + Meta->IvTweak, Buffer, Size, Encrypt) ? VcOk : VcCryptoError;
}
