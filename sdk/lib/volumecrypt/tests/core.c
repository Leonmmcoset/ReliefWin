/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     LUKS2 interoperability, corruption and interrupted-conversion tests
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "../internal.h"
#include <reliefwin/drivers/volumecrypt.h>
#include <stdio.h>
#include <stdlib.h>

extern const VC_OPS VcTestOps;
extern void VcTestInitialize(void);

static unsigned Checks, Failures;
#define CHECK(Condition) do { ++Checks; if (!(Condition)) { ++Failures; \
    printf("FAIL line %u: %s\n", (unsigned)__LINE__, #Condition); } } while (0)

typedef struct TEST_FAULT
{
    unsigned Count;
    unsigned At;
    int Tear;
} TEST_FAULT;

typedef struct TEST_DISK
{
    uint8_t *Bytes;
    uint8_t *Stable;
    size_t Size;
    TEST_FAULT *Fault;
} TEST_DISK;

static int
Fault(TEST_DISK *Disk)
{
    return Disk->Fault && ++Disk->Fault->Count == Disk->Fault->At;
}

static int
Read(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    TEST_DISK *Disk = Context;
    if (Offset > Disk->Size || Size > Disk->Size - Offset || Fault(Disk))
        return 0;
    memcpy(Buffer, Disk->Bytes + (size_t)Offset, Size);
    return 1;
}

static int
Write(void *Context, uint64_t Offset, const void *Buffer, size_t Size)
{
    TEST_DISK *Disk = Context;
    if (Offset > Disk->Size || Size > Disk->Size - Offset)
        return 0;
    if (Fault(Disk))
    {
        if (Disk->Fault->Tear)
        {
            size_t Torn = Disk->Fault->Tear == 2 && Size > 512 ? 512 : Size / 2;
            memcpy(Disk->Bytes + (size_t)Offset, Buffer, Torn);
            if (Disk->Stable)
                memcpy(Disk->Stable + (size_t)Offset, Buffer, Torn);
        }
        return 0;
    }
    memcpy(Disk->Bytes + (size_t)Offset, Buffer, Size);
    return 1;
}

static int
Flush(void *Context)
{
    TEST_DISK *Disk = Context;
    int Failed = Fault(Disk);
    if (Disk->Stable && (!Failed || (Disk->Fault && Disk->Fault->Tear)))
        memcpy(Disk->Stable, Disk->Bytes, Disk->Size);
    return !Failed;
}

static VC_IO
Io(TEST_DISK *Disk, size_t Size)
{
    VC_IO Result = {Disk, Size, Read, Write, Flush};
    Disk->Bytes = calloc(1, Size);
    Disk->Size = Size;
    Disk->Stable = NULL;
    Disk->Fault = NULL;
    CHECK(Disk->Bytes != NULL);
    if (!Disk->Bytes)
        exit(2);
    return Result;
}

static void
Drop(TEST_DISK *Disk)
{
    VcWipe(Disk->Bytes, Disk->Size);
    free(Disk->Bytes);
    free(Disk->Stable);
}

static void
Save(const char *Path, const void *Bytes, size_t Size)
{
    FILE *File = fopen(Path, "wb");
    CHECK(File != NULL);
    if (File)
    {
        CHECK(fwrite(Bytes, 1, Size, File) == Size);
        CHECK(fclose(File) == 0);
    }
}

static void
Rehash(uint8_t *Header, size_t Size)
{
    uint8_t Digest[32];
    memset(Header + 448, 0, 64);
    CHECK(VcTestOps.Hash(Header, Size, Digest));
    memcpy(Header + 448, Digest, 32);
}

static void
Metadata(TEST_DISK *Disk, VC_IO *Header, VC_META *Meta, uint8_t Key[64])
{
    VC_META Loaded;
    uint8_t Unlocked[64], *Saved = malloc(Disk->Size);
    unsigned i;
    CHECK(Saved != NULL);
    if (!Saved)
        exit(2);
    CHECK(VcCreate(&VcTestOps, Header, "fixture-password", 16, "fixture-recovery", 16, Key, Meta) == VcOk);
    CHECK(Meta->Incomplete && Meta->SlotCount == 2 && Meta->Sequence == 1);
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcOk);
    CHECK(!memcmp(Meta, &Loaded, sizeof(Loaded)));
    CHECK(VcUnlock(&VcTestOps, Header, &Loaded, "fixture-password", 16, Unlocked) == VcOk);
    CHECK(VcEqual(Key, Unlocked, 64));
    CHECK(VcUnlock(&VcTestOps, Header, &Loaded, "fixture-recovery", 16, Unlocked) == VcOk);
    CHECK(VcEqual(Key, Unlocked, 64));
    memset(Unlocked, 0x5a, 64);
    CHECK(VcUnlock(&VcTestOps, Header, &Loaded, "wrong", 5, Unlocked) == VcWrongSecret);
    for (i = 0; i != 64; ++i)
        CHECK(Unlocked[i] == 0);
    memcpy(Saved, Disk->Bytes, Disk->Size);
    Disk->Bytes[0] ^= 1;
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcOk);
    CHECK(Loaded.Sequence == Meta->Sequence);
    Disk->Bytes[VC_HEADER_SIZE] ^= 1;
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcInvalid);
    memcpy(Disk->Bytes, Saved, Disk->Size);
    CHECK(VcSetOffline(&VcTestOps, Header, Meta, 0) == VcOk);
    CHECK(!Meta->Incomplete && Meta->Sequence == 2);
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcOk && !Loaded.Incomplete);
    CHECK(VcSetOffline(&VcTestOps, Header, &Loaded, 1) == VcOk);
    CHECK(VcSetOffline(&VcTestOps, Header, Meta, 1) == VcConflict);
    *Meta = Loaded;
    memcpy(Saved, Disk->Bytes, Disk->Size);
    for (i = 0; i != 2; ++i)
    {
        uint8_t *Bytes = Disk->Bytes + i * VC_HEADER_SIZE;
        char *Match = strstr((char *)Bytes + 4096, "offline-reencrypt");
        CHECK(Match != NULL);
        if (Match)
            Match[0] = 'X';
        Rehash(Bytes, VC_HEADER_SIZE);
    }
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcUnsupported);
    memcpy(Disk->Bytes, Saved, Disk->Size);
    for (i = 0; i != 2; ++i)
    {
        uint8_t *Bytes = Disk->Bytes + i * VC_HEADER_SIZE;
        char *Match = strstr((char *)Bytes + 4096, "\"offset\":\"32768\"");
        CHECK(Match != NULL);
        if (Match)
            memcpy(Match + 10, "00000", 5);
        Rehash(Bytes, VC_HEADER_SIZE);
    }
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcInvalid);
    memcpy(Disk->Bytes, Saved, Disk->Size);
    VcPut64(Disk->Bytes + 16, UINT64_MAX);
    Rehash(Disk->Bytes, VC_HEADER_SIZE);
    CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcOk);
    CHECK(VcSetOffline(&VcTestOps, Header, &Loaded, 0) == VcConflict);
    memcpy(Disk->Bytes, Saved, Disk->Size);
    free(Saved);
    VcWipe(Unlocked, sizeof(Unlocked));
}

static void
HeaderInterruption(TEST_DISK *Disk, VC_IO *Header)
{
    uint8_t *Saved = malloc(Disk->Size);
    VC_META Meta, Loaded;
    TEST_FAULT Injection = {0};
    unsigned At, Copies, Operations;
    CHECK(Saved != NULL);
    if (!Saved)
        exit(2);
    memcpy(Saved, Disk->Bytes, Disk->Size);
    for (Copies = 0; Copies != 2; ++Copies)
    {
        memcpy(Disk->Bytes, Saved, Disk->Size);
        VcPut64(Disk->Bytes + Copies * VC_HEADER_SIZE + 16, 100);
        Rehash(Disk->Bytes + Copies * VC_HEADER_SIZE, VC_HEADER_SIZE);
        CHECK(VcLoad(&VcTestOps, Header, &Meta) == VcOk && Meta.Sequence == 100);
        Disk->Fault = &Injection;
        Injection.Count = 0;
        Injection.At = 0;
        CHECK(VcSetOffline(&VcTestOps, Header, &Meta, !Meta.Incomplete) == VcOk);
        Operations = Injection.Count;
        Disk->Fault = NULL;
        for (At = 1; At <= Operations; ++At)
        {
            memcpy(Disk->Bytes, Saved, Disk->Size);
            VcPut64(Disk->Bytes + Copies * VC_HEADER_SIZE + 16, 100);
            Rehash(Disk->Bytes + Copies * VC_HEADER_SIZE, VC_HEADER_SIZE);
            CHECK(VcLoad(&VcTestOps, Header, &Meta) == VcOk && Meta.Sequence == 100);
            Injection.Count = 0;
            Injection.At = At;
            Injection.Tear = 2;
            Disk->Fault = &Injection;
            VcSetOffline(&VcTestOps, Header, &Meta, !Meta.Incomplete);
            Disk->Fault = NULL;
            CHECK(VcLoad(&VcTestOps, Header, &Loaded) == VcOk);
            CHECK(Loaded.Sequence == 100 || Loaded.Sequence == 101);
        }
    }
    memcpy(Disk->Bytes, Saved, Disk->Size);
    free(Saved);
}

static void
Descriptors(void)
{
    static const char Uuid[] = "12345678-1234-4234-8234-123456789abc";
    RW_CRYPT_DESCRIPTOR Volumes[2];
    uint8_t *Index = malloc(RW_CRYPT_INDEX_SIZE);
    uint8_t Guid[16] = {1}, Part[16] = {2};
    char Path[96];
    uint64_t Sequence;
    uint32_t Count;
    unsigned i;
    CHECK(Index != NULL);
    if (!Index)
        exit(2);
    CHECK(RwCryptDescriptorCreate(&VcTestOps, Volumes, 0, 0x12345678, 1048576, 16777216, NULL, NULL, Uuid));
    CHECK(!RwCryptDescriptorSystem(Volumes));
    CHECK(RwCryptDescriptorSetSystem(&VcTestOps, Volumes));
    CHECK(RwCryptDescriptorValid(&VcTestOps, Volumes));
    CHECK(RwCryptDescriptorSystem(Volumes));
    Volumes[0].Bytes[115] = 2;
    CHECK(!RwCryptDescriptorValid(&VcTestOps, Volumes));
    CHECK(!RwCryptDescriptorSetSystem(&VcTestOps, Volumes));
    Volumes[0].Bytes[115] = 1;
    CHECK(RwCryptDescriptorMatch(Volumes, 0, 0x12345678, 1048576, 16777216, NULL, NULL));
    CHECK(!RwCryptDescriptorMatch(Volumes, 0, 0x12345679, 1048576, 16777216, NULL, NULL));
    CHECK(!RwCryptDescriptorMatch(Volumes, 0, 0x12345678, 1049088, 16777216, NULL, NULL));
    CHECK(!RwCryptDescriptorMatch(Volumes, 0, 0x12345678, 1048576, 16777728, NULL, NULL));
    RwCryptHeaderPath(Volumes, Path);
    CHECK(!strcmp(Path, "\\EFI\\ReliefWin\\Encryption\\12345678-1234-4234-8234-123456789abc.luks"));
    CHECK(RwCryptIndexCreate(&VcTestOps, Index, 5, Volumes, 1));
    CHECK(RwCryptIndexRead(&VcTestOps, Index, RW_CRYPT_INDEX_SIZE, &Sequence, &Count));
    CHECK(Sequence == 5 && Count == 1);
    for (i = 0; i < RW_CRYPT_INDEX_SIZE; i += 512)
    {
        Index[i] ^= 1;
        CHECK(!RwCryptIndexRead(&VcTestOps, Index, RW_CRYPT_INDEX_SIZE, &Sequence, &Count));
        Index[i] ^= 1;
    }
    CHECK(!RwCryptIndexRead(&VcTestOps, Index, RW_CRYPT_INDEX_SIZE - 1, &Sequence, &Count));
    Volumes[1] = Volumes[0];
    CHECK(!RwCryptIndexCreate(&VcTestOps, Index, 6, Volumes, 2));
    CHECK(RwCryptDescriptorCreate(&VcTestOps, Volumes + 1, 1, 0, 1048576, 16777216,
                                 Guid, Part, "98765432-4321-4321-8321-123456789abc"));
    CHECK(RwCryptDescriptorMatch(Volumes + 1, 1, 0, 1048576, 16777216, Guid, Part));
    Part[1] = 3;
    CHECK(!RwCryptDescriptorMatch(Volumes + 1, 1, 0, 1048576, 16777216, Guid, Part));
    CHECK(RwCryptIndexCreate(&VcTestOps, Index, UINT64_MAX, Volumes, 2));
    CHECK(RwCryptIndexRead(&VcTestOps, Index, RW_CRYPT_INDEX_SIZE, &Sequence, &Count));
    CHECK(Sequence == UINT64_MAX && Count == 2);
    CHECK(!RwCryptIndexCreate(&VcTestOps, Index, 0, Volumes, 1));
    CHECK(!RwCryptDescriptorCreate(&VcTestOps, Volumes, 0, 1, UINT64_MAX - 511, 1024, NULL, NULL, Uuid));
    free(Index);
}

static void
ChangeSecret(TEST_DISK *Disk, const VC_IO *Header, VC_META *Meta, const uint8_t Key[64])
{
    uint8_t *Saved = malloc(Disk->Size), Unlocked[64];
    VC_META Current;
    TEST_FAULT Injection = {0};
    unsigned Calls, At;
    CHECK(Saved != NULL);
    if (!Saved)
        exit(2);
    memcpy(Saved, Disk->Bytes, Disk->Size);
    Disk->Fault = &Injection;
    CHECK(VcChangeSecret(&VcTestOps, Header, Meta, Key, 0, "new-fixture-password", 20) == VcOk);
    Calls = Injection.Count;
    Disk->Fault = NULL;
    CHECK(VcUnlock(&VcTestOps, Header, Meta, "fixture-password", 16, Unlocked) == VcWrongSecret);
    CHECK(VcUnlock(&VcTestOps, Header, Meta, "new-fixture-password", 20, Unlocked) == VcOk);
    CHECK(VcEqual(Key, Unlocked, 64));
    CHECK(VcUnlock(&VcTestOps, Header, Meta, "fixture-recovery", 16, Unlocked) == VcOk);
    for (At = 1; At <= Calls; ++At)
    {
        VC_RESULT Result;
        memcpy(Disk->Bytes, Saved, Disk->Size);
        CHECK(VcLoad(&VcTestOps, Header, &Current) == VcOk);
        Injection.At = At;
        Injection.Count = 0;
        Injection.Tear = 2;
        Disk->Fault = &Injection;
        VcChangeSecret(&VcTestOps, Header, &Current, Key, 0, "new-fixture-password", 20);
        Disk->Fault = NULL;
        CHECK(VcLoad(&VcTestOps, Header, &Current) == VcOk);
        CHECK(VcUnlock(&VcTestOps, Header, &Current, "fixture-recovery", 16, Unlocked) == VcOk);
        CHECK(VcEqual(Key, Unlocked, 64));
        Result = VcUnlock(&VcTestOps, Header, &Current, "fixture-password", 16, Unlocked);
        if (Result != VcOk)
            Result = VcUnlock(&VcTestOps, Header, &Current, "new-fixture-password", 20, Unlocked);
        CHECK(Result == VcOk && VcEqual(Key, Unlocked, 64));
    }
    memcpy(Disk->Bytes, Saved, Disk->Size);
    CHECK(VcLoad(&VcTestOps, Header, Meta) == VcOk);
    VcWipe(Unlocked, sizeof(Unlocked));
    free(Saved);
}

static void
XtsVector(void)
{
    static const uint8_t Expected[32] =
    {
        0x1e,0x2d,0xd4,0xe2,0xfc,0x82,0x7e,0x8a,0xd1,0xf2,0x88,0x6d,0x8d,0x9f,0xc3,0xf3,
        0x01,0xc1,0x21,0x89,0xb1,0xbf,0xe3,0x45,0xba,0x92,0x2a,0x7a,0x2f,0x57,0xa1,0xda
    };
    uint8_t Key[64], Data[512], Digest[32];
    unsigned i;
    for (i = 0; i != sizeof(Key); ++i)
        Key[i] = (uint8_t)i;
    for (i = 0; i != sizeof(Data); ++i)
        Data[i] = (uint8_t)i;
    CHECK(VcTestOps.Xts(Key, UINT64_C(0x123456789abcdef0), Data, sizeof(Data), 1));
    CHECK(VcTestOps.Hash(Data, sizeof(Data), Digest));
    CHECK(VcEqual(Digest, Expected, sizeof(Digest)));
    CHECK(VcTestOps.Xts(Key, UINT64_C(0x123456789abcdef0), Data, sizeof(Data), 0));
    for (i = 0; i != sizeof(Data); ++i)
        if (Data[i] != (uint8_t)i)
            break;
    CHECK(i == sizeof(Data));
    VcWipe(Key, sizeof(Key));
    VcWipe(Data, sizeof(Data));
}

static void
Json(void)
{
    VC_JSON *Json = malloc(sizeof(*Json));
    uint64_t Number;
    uint8_t Bytes[32];
    CHECK(Json != NULL);
    if (!Json)
        exit(2);
    CHECK(VcJsonParse(Json, "{\"a\":[0,1,{\"c\":false}],\"b\":true}", strlen("{\"a\":[0,1,{\"c\":false}],\"b\":true}")));
    CHECK(!VcJsonParse(Json, "{\"a\":1,\"a\":2}", 13));
    CHECK(!VcJsonParse(Json, "{\"a\":1,}", 8));
    CHECK(!VcJsonParse(Json, "{\"a\":01}", 8));
    CHECK(!VcJsonParse(Json, "{\"a\":1}x", 8));
    CHECK(VcJsonParse(Json, "{\"a\":\"18446744073709551615\"}", 28));
    CHECK(VcJsonNumber(Json, VcJsonMember(Json, 1, "a"), &Number, '"') && Number == UINT64_MAX);
    CHECK(VcJsonParse(Json, "{\"a\":\"18446744073709551616\"}", 28));
    CHECK(!VcJsonNumber(Json, VcJsonMember(Json, 1, "a"), &Number, '"'));
    {
        char Text[128], Encoded[45];
        memset(Bytes, 0xa5, sizeof(Bytes));
        VcBase64(Bytes, sizeof(Bytes), Encoded);
        snprintf(Text, sizeof(Text), "{\"a\":\"%s\"}", Encoded);
        CHECK(VcJsonParse(Json, Text, strlen(Text)));
        CHECK(VcJsonBinary(Json, VcJsonMember(Json, 1, "a"), Bytes, sizeof(Bytes)));
        Text[strlen(Text) - 4] = '!';
        CHECK(VcJsonParse(Json, Text, strlen(Text)));
        CHECK(!VcJsonBinary(Json, VcJsonMember(Json, 1, "a"), Bytes, sizeof(Bytes)));
    }
    free(Json);
}

static void
Convert(const VC_META *Meta, const uint8_t Key[64], int Decrypt, int Durable)
{
    TEST_DISK VolumeDisk, JournalDisk;
    VC_IO Volume = Io(&VolumeDisk, VC_JOURNAL_CHUNK + 512);
    VC_IO Journal = Io(&JournalDisk, VC_JOURNAL_SIZE);
    uint8_t Target[32] = {0x12, 0x34}, Wrong[32] = {0x56};
    uint8_t *Original = malloc(VolumeDisk.Size), *Expected = malloc(VolumeDisk.Size);
    uint8_t *Clean = malloc(JournalDisk.Size);
    uint8_t BadKey[64];
    VC_PROGRESS Progress;
    VC_RESULT Result;
    TEST_FAULT Injection;
    size_t i;
    unsigned At, Tear, Calls;
    CHECK(Original && Expected && Clean);
    if (!Original || !Expected || !Clean)
        exit(2);
    for (i = 0; i != VolumeDisk.Size; ++i)
        Original[i] = (uint8_t)(i * 37 + i / 512);
    memcpy(Expected, Original, VolumeDisk.Size);
    CHECK(VcTestOps.Xts(Key, 0, Decrypt ? Original : Expected, VolumeDisk.Size, 1));
    memcpy(VolumeDisk.Bytes, Original, VolumeDisk.Size);
    CHECK(VcJournalBegin(&VcTestOps, &Journal, Meta, Key, Target, Volume.Size, Decrypt) == VcOk);
    CHECK(VcJournalBegin(&VcTestOps, &Journal, Meta, Key, Target, Volume.Size, Decrypt) == VcConflict);
    memcpy(Clean, JournalDisk.Bytes, JournalDisk.Size);
    CHECK(VcJournalRead(&VcTestOps, &Journal, Meta, Key, Wrong, Volume.Size, &Progress) == VcInvalid);
    memcpy(BadKey, Key, sizeof(BadKey));
    BadKey[0] ^= 1;
    CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, BadKey, Target, &Progress) == VcInvalid);
    CHECK(!memcmp(VolumeDisk.Bytes, Original, VolumeDisk.Size));
    Injection.Count = Injection.At = Injection.Tear = 0;
    JournalDisk.Fault = VolumeDisk.Fault = &Injection;
    CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress) == VcOk);
    Calls = Injection.Count;
    CHECK(Progress.Completed == VC_JOURNAL_CHUNK);
    if (Durable)
    {
        JournalDisk.Stable = malloc(JournalDisk.Size);
        VolumeDisk.Stable = malloc(VolumeDisk.Size);
        CHECK(JournalDisk.Stable && VolumeDisk.Stable);
        if (!JournalDisk.Stable || !VolumeDisk.Stable)
            exit(2);
    }
    for (Tear = 0; Tear != 2; ++Tear)
    {
        for (At = 1; At <= Calls; ++At)
        {
            memcpy(JournalDisk.Bytes, Clean, JournalDisk.Size);
            memcpy(VolumeDisk.Bytes, Original, VolumeDisk.Size);
            if (Durable)
            {
                memcpy(JournalDisk.Stable, Clean, JournalDisk.Size);
                memcpy(VolumeDisk.Stable, Original, VolumeDisk.Size);
            }
            Injection.Count = 0;
            Injection.At = At;
            Injection.Tear = Tear;
            CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress) == VcIoError);
            Injection.At = 0;
            if (Durable)
            {
                memcpy(JournalDisk.Bytes, JournalDisk.Stable, JournalDisk.Size);
                memcpy(VolumeDisk.Bytes, VolumeDisk.Stable, VolumeDisk.Size);
            }
            for (i = 0; i != 5; ++i)
            {
                Result = VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress);
                if (Result == VcFinished)
                    break;
                CHECK(Result == VcOk);
                if (Result != VcOk)
                    break;
            }
            CHECK(Result == VcFinished && Progress.Completed == Volume.Size && !Progress.Pending);
            CHECK(!memcmp(VolumeDisk.Bytes, Expected, VolumeDisk.Size));
            CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress) == VcFinished);
            CHECK(!memcmp(VolumeDisk.Bytes, Expected, VolumeDisk.Size));
        }
    }
    memcpy(JournalDisk.Bytes, Clean, JournalDisk.Size);
    memcpy(VolumeDisk.Bytes, Original, VolumeDisk.Size);
    Injection.Count = 0;
    Injection.At = Calls - 5;
    CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress) == VcIoError);
    Injection.At = 0;
    CHECK(VcJournalRead(&VcTestOps, &Journal, Meta, Key, Target, Volume.Size, &Progress) == VcOk);
    CHECK(Progress.Pending != 0);
    JournalDisk.Bytes[8192 + 17] ^= 1;
    memcpy(Original, VolumeDisk.Bytes, VolumeDisk.Size);
    CHECK(VcConvertStep(&VcTestOps, &Journal, &Volume, Meta, Key, Target, &Progress) == VcInvalid);
    CHECK(!memcmp(Original, VolumeDisk.Bytes, VolumeDisk.Size));
    free(Clean);
    free(Expected);
    free(Original);
    Drop(&VolumeDisk);
    Drop(&JournalDisk);
}

static int
Import(const char *HeaderPath, const char *KeyPath)
{
    FILE *File = fopen(HeaderPath, "rb");
    TEST_DISK Disk;
    VC_IO Header;
    VC_META Meta;
    uint8_t Key[64], Expected[64];
    long Length;
    CHECK(File != NULL);
    if (!File)
        return 1;
    CHECK(fseek(File, 0, SEEK_END) == 0);
    Length = ftell(File);
    CHECK(Length > 0);
    CHECK(fseek(File, 0, SEEK_SET) == 0);
    if (Length <= 0)
        return 1;
    Header = Io(&Disk, (size_t)Length);
    CHECK(fread(Disk.Bytes, 1, Disk.Size, File) == Disk.Size);
    fclose(File);
    CHECK(VcLoad(&VcTestOps, &Header, &Meta) == VcOk);
    CHECK(VcUnlock(&VcTestOps, &Header, &Meta, "fixture-password", 16, Key) == VcOk);
    File = fopen(KeyPath, "rb");
    CHECK(File != NULL);
    if (File)
    {
        CHECK(fread(Expected, 1, 64, File) == 64);
        CHECK(VcEqual(Expected, Key, 64));
        fclose(File);
    }
    Drop(&Disk);
    VcWipe(Key, 64);
    VcWipe(Expected, 64);
    printf("%u checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}

int
main(int Argc, char **Argv)
{
    TEST_DISK Disk;
    VC_IO Header;
    VC_META Meta;
    uint8_t Key[64], Buffer[1024] = {0};
    VcTestInitialize();
    if (Argc == 4 && !strcmp(Argv[1], "--import"))
        return Import(Argv[2], Argv[3]);
    Json();
    XtsVector();
    Descriptors();
    Header = Io(&Disk, VC_METADATA_SIZE);
    Metadata(&Disk, &Header, &Meta, Key);
    HeaderInterruption(&Disk, &Header);
    CHECK(VcCrypt(&VcTestOps, &Meta, Key, 1, Buffer, 512, 1) == VcInvalid);
    CHECK(VcCrypt(&VcTestOps, &Meta, Key, 0, Buffer, 513, 1) == VcInvalid);
    CHECK(VcCrypt(&VcTestOps, &Meta, Key, UINT64_MAX - 511, Buffer, 1024, 1) == VcInvalid);
    Meta.IvTweak = UINT64_MAX;
    CHECK(VcCrypt(&VcTestOps, &Meta, Key, 0, Buffer, 1024, 1) == VcInvalid);
    Meta.IvTweak = 0;
    Convert(&Meta, Key, 0, 0);
    Convert(&Meta, Key, 1, 0);
    Convert(&Meta, Key, 0, 1);
    Convert(&Meta, Key, 1, 1);
    CHECK(VcSetOffline(&VcTestOps, &Header, &Meta, 0) == VcOk);
    ChangeSecret(&Disk, &Header, &Meta, Key);
    if (Argc == 2 && !strcmp(Argv[1], "--export"))
    {
        Save("/tmp/reliefwin-crypt-header", Disk.Bytes, Disk.Size);
        Save("/tmp/reliefwin-crypt-expected-key", Key, 64);
        Save("/tmp/reliefwin-crypt-password", "fixture-password", 16);
        Save("/tmp/reliefwin-crypt-recovery", "fixture-recovery", 16);
    }
    Drop(&Disk);
    VcWipe(Key, sizeof(Key));
    printf("%u checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
