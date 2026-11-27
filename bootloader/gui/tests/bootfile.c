/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     BIOS boot-file persistence, cache coherence and I/O failures
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

static DK_DEVICE BootDevice;
static PUCHAR Disk;
static ULONG Checks, Writes, Flushes;
static BOOLEAN WriteFails, FlushFails, NoDevice, NtfsMode, NtfsFile, NtfsReadFails;
static CHAR NtfsData[5];

#define CHECK(x) do {++Checks; assert(x);} while (0)

PVOID FwAllocate(SIZE_T Size) {return calloc(1, Size);}
PVOID FwAllocatePages(SIZE_T Size) {return malloc(Size);}
VOID FwFree(PVOID Data) {free(Data);}
VOID FwFreePages(PVOID Data, SIZE_T Size) {free(Data);}

BOOLEAN DkRead(PDK_DEVICE Device, ULONGLONG Offset, ULONG Size, PVOID Data)
{
    if (NtfsMode || Offset > Device->Length || Size > Device->Length - Offset)
        return FALSE;
    memcpy(Data, Disk + Offset, Size);
    return TRUE;
}

BOOLEAN DkWrite(PDK_DEVICE Device, ULONGLONG Offset, ULONG Size, const VOID *Data)
{
    if (WriteFails || Offset > Device->Length || Size > Device->Length - Offset)
        return FALSE;
    ++Writes;
    memcpy(Disk + Offset, Data, Size);
    return TRUE;
}

BOOLEAN DkFlush(PDK_DEVICE Device) {++Flushes; return !FlushFails;}
BOOLEAN FwGetBootDevice(PDK_DEVICE Device) {*Device = BootDevice; return !NoDevice;}
PNTFS_VOLUME_INFO NtfsMount(PDK_DEVICE Device) {return NtfsMode ? &BootDevice : NULL;}
PVOID NtfsReadFile(PNTFS_VOLUME_INFO Volume, PCSTR Path, PULONG Size) {return NULL;}
PVOID NtfsOpenFile(PNTFS_VOLUME_INFO Volume, PCSTR Path, PULONGLONG Size) {return NULL;}
BOOLEAN NtfsReadFileAt(PVOID File, ULONGLONG Offset, PVOID Buffer, ULONG Size) {return FALSE;}
VOID NtfsCloseFile(PVOID File) {}
BOOLEAN CdfsMount(PDK_DEVICE Device) {return FALSE;}
PVOID CdfsReadFile(PCSTR Path, PULONG Size) {return NULL;}

NTSTATUS NtfsBlMount(PDK_DEVICE Device) {return NtfsMode ? 0 : -1;}
NTSTATUS NtfsBlUnmount(void) {return FlushFails ? -1 : 0;}
NTSTATUS NtfsBlOpenFile(PCSTR Path, PVOID *File, PULONGLONG Size, PBOOLEAN Directory)
{
    CHECK(!strcmp(Path, "ReliefWin.bootlang"));
    if (!NtfsFile) return -1;
    *File = NtfsData;
    *Size = sizeof(NtfsData);
    *Directory = FALSE;
    return 0;
}
NTSTATUS NtfsBlReadFileAt(PVOID File, ULONGLONG Offset, PVOID Buffer, ULONG Size, PULONG Read)
{
    CHECK(!Offset && Size == sizeof(NtfsData));
    if (NtfsReadFails) return -1;
    memcpy(Buffer, File, Size);
    *Read = Size;
    return 0;
}
VOID NtfsBlCloseFile(PVOID File) {}
NTSTATUS NtfsBlWriteFile(PCSTR Path, const VOID *Data, ULONG Size)
{
    CHECK(!strcmp(Path, "ReliefWin.bootlang") && Size == sizeof(NtfsData));
    if (WriteFails) return -1;
    memcpy(NtfsData, Data, Size);
    NtfsFile = TRUE;
    return 0;
}

static void ReadTag(PCWSTR Path, PCSTR Expected)
{
    ULONG Size = 0;
    PVOID Data = FwReadBootFile(Path, &Size);
    CHECK(Data && Size == 5 && !memcmp(Data, Expected, Size));
    FwFreePages(Data, Size);
}

static void FormatFat(BYTE Format, ULONG Size, BYTE Type)
{
    BYTE Work[4096];
    MKFS_PARM Parameters = {Format | FM_SFD, 1, 0, 0, 512};
    PEXFAT_VOLUME Volume;
    BYTE Input[512], Output[512];
    ULONG Before, ReadSize;
    PVOID Data;
    CHAR TooLong[EXFAT_PATH_CHARS + 1];

    ExFatUnmount();
    FsMountedVolume = NULL;
    FsMountedExFat = FsMountedCdfs = FALSE;
    memset(Disk, 0, BootDevice.Length);
    BootDevice.Length = Size;
    Volume = FwAllocate(sizeof(*Volume));
    Volume->Device = BootDevice;
    Volume->BytesPerSector = 512;
    Volume->SectorCount = Size / 512;
    ExFatVolume = Volume;
    CHECK(f_mkfs("0:", &Parameters, Work, sizeof(Work)) == FR_OK);
    ExFatUnmount();

    CHECK(FwWriteBootFile(L"\\ReliefWin.bootlang", "en-US", 5));
    CHECK(ExFatVolume->FileSystem.fs_type == Type);
    ReadTag(L"\\ReliefWin.bootlang", "en-US");
    CHECK(FwWriteBootFile(L"\\untouched.bin", "other", 5));
    CHECK(FwWriteBootFile(L"\\ReliefWin.bootlang", "ja-JP", 5));
    ReadTag(L"\\ReliefWin.bootlang", "ja-JP");
    ReadTag(L"\\untouched.bin", "other");
    CHECK(ExFatMount(&BootDevice));
    ReadTag(L"\\ReliefWin.bootlang", "ja-JP");

    CHECK(disk_read(0, Output, 0, 0) == RES_PARERR);
    CHECK(disk_read(0, Output, Size / 512 - 1, 1) == RES_OK);
    memset(Input, 0x6b, sizeof(Input));
    CHECK(disk_write(0, Input, Size / 512 - 1, 1) == RES_OK);
    CHECK(disk_read(0, Output, Size / 512 - 1, 1) == RES_OK);
    CHECK(!memcmp(Input, Output, sizeof(Input)));
    Before = Writes;
    CHECK(disk_write(1, Input, 0, 1) == RES_PARERR);
    CHECK(disk_write(0, Input, Size / 512, 1) == RES_PARERR);
    CHECK(disk_write(0, Input, Size / 512 - 1, 2) == RES_PARERR);
    CHECK(disk_write(0, NULL, 0, 1) == RES_PARERR);
    CHECK(Writes == Before);
    memset(TooLong, 'x', sizeof(TooLong) - 1);
    TooLong[sizeof(TooLong) - 1] = 0;
    CHECK(!ExFatWriteFile(TooLong, Input, 1));
    CHECK(Writes == Before);
    WriteFails = TRUE;
    CHECK(!FwWriteBootFile(L"\\ReliefWin.bootlang", "zh-CN", 5));
    WriteFails = FALSE;
    FlushFails = TRUE;
    CHECK(!FwWriteBootFile(L"\\ReliefWin.bootlang", "zh-CN", 5));
    FlushFails = FALSE;
    CHECK(FwWriteBootFile(L"\\ReliefWin.bootlang", "zh-CN", 5));
    ReadTag(L"\\ReliefWin.bootlang", "zh-CN");
    Data = FwReadBootFile(L"\\untouched.bin", &ReadSize);
    CHECK(Data && ReadSize == 5 && !memcmp(Data, "other", 5));
    free(Data);
}

int main(void)
{
    ULONG Size, Before;
    Disk = calloc(1, 96 * 1024 * 1024);
    CHECK(Disk != NULL);
    BootDevice.Firmware = Disk;
    FormatFat(FM_FAT, 1024 * 1024, FS_FAT12);
    FormatFat(FM_FAT, 8 * 1024 * 1024, FS_FAT16);
    FormatFat(FM_FAT32, 64 * 1024 * 1024, FS_FAT32);
    FormatFat(FM_EXFAT, 96 * 1024 * 1024, FS_EXFAT);
    ExFatUnmount();
    Before = Writes;
    CHECK(!FwWriteBootFile(NULL, "en-US", 5));
    CHECK(!FwWriteBootFile(L"/", "en-US", 5));
    CHECK(!FwWriteBootFile(L"\\\u4e2d", "en-US", 5));
    CHECK(!FwWriteBootFile(L"\\ReliefWin.bootlang", NULL, 5));
    NoDevice = TRUE;
    CHECK(!FwWriteBootFile(L"\\ReliefWin.bootlang", "en-US", 5));
    CHECK(!FwReadBootFile(L"\\ReliefWin.bootlang", &Size) && !Size);
    NoDevice = FALSE;
    CHECK(Writes == Before);
    NtfsMode = TRUE;
    FsMountedExFat = FALSE;
    CHECK(!FwReadBootFile(L"\\ReliefWin.bootlang", &Size) && !Size);
    CHECK(FwWriteBootFile(L"\\ReliefWin.bootlang", "ja-JP", 5));
    ReadTag(L"\\ReliefWin.bootlang", "ja-JP");
    NtfsReadFails = TRUE;
    CHECK(!FwReadBootFile(L"\\ReliefWin.bootlang", &Size) && !Size);
    NtfsReadFails = FALSE;
    FlushFails = TRUE;
    CHECK(!FwWriteBootFile(L"\\ReliefWin.bootlang", "en-US", 5));
    CHECK(!FwReadBootFile(L"\\ReliefWin.bootlang", &Size) && !Size);
    FlushFails = FALSE;
    CHECK(FwWriteBootFile(L"\\ReliefWin.bootlang", "en-US", 5));
    ReadTag(L"\\ReliefWin.bootlang", "en-US");
    free(Disk);
    printf("%u BIOS boot-file checks passed; %u writes, %u flushes\n",
           (unsigned)Checks, (unsigned)Writes, (unsigned)Flushes);
    return 0;
}
