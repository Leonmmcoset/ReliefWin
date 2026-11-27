/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Encrypted I/O on a signature-guarded disposable test partition
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <windows.h>
#include <winioctl.h>
#include <ntddstor.h>
#include <reliefwin/drivers/volumecrypt.h>
#include <reliefwin/libs/volumecrypt/crypto.h>
#include <stdio.h>

extern const VC_OPS VcTestOps;
extern void VcTestInitialize(void);

static unsigned Checks, Failures;
#define CHECK(C) do { ++Checks; if (!(C)) { ++Failures; \
    printf("FAIL data %u: %s (%lu)\n", (unsigned)__LINE__, #C, GetLastError()); } } while (0)

typedef struct MEMORY
{
    BYTE *Bytes;
    size_t Size;
} MEMORY;

static int ReadMemory(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    MEMORY *Memory = Context;
    if (Offset > Memory->Size || Size > Memory->Size - Offset)
        return 0;
    memcpy(Buffer, Memory->Bytes + (size_t)Offset, Size);
    return 1;
}

static int WriteMemory(void *Context, uint64_t Offset, const void *Buffer, size_t Size)
{
    MEMORY *Memory = Context;
    if (Offset > Memory->Size || Size > Memory->Size - Offset)
        return 0;
    memcpy(Memory->Bytes + (size_t)Offset, Buffer, Size);
    return 1;
}

static int FlushMemory(void *Context)
{
    (void)Context;
    return 1;
}

static BOOL Transfer(HANDLE File, BOOL Write, ULONGLONG Offset, BYTE *Buffer)
{
    LARGE_INTEGER Position;
    DWORD Done;
    Position.QuadPart = Offset;
    return SetFilePointerEx(File, Position, NULL, FILE_BEGIN) &&
           (Write ? WriteFile(File, Buffer, 512, &Done, NULL) : ReadFile(File, Buffer, 512, &Done, NULL)) &&
           Done == 512;
}

int CryptTestData(LPCWSTR Path)
{
    HANDLE Volume = INVALID_HANDLE_VALUE, Disk = INVALID_HANDLE_VALUE, Control = INVALID_HANDLE_VALUE;
    STORAGE_DEVICE_NUMBER Number;
    PARTITION_INFORMATION_EX Partition;
    BYTE LayoutBuffer[65536];
    PDRIVE_LAYOUT_INFORMATION_EX Layout = (PDRIVE_LAYOUT_INFORMATION_EX)LayoutBuffer;
    WCHAR DiskPath[64];
    BYTE Key[64], Plain[512], Expected[512], *Aligned = NULL;
    RW_CRYPT_REQUEST *Request = NULL;
    MEMORY Memory = {NULL, VC_METADATA_SIZE};
    VC_IO Header = {&Memory, VC_METADATA_SIZE, ReadMemory, WriteMemory, FlushMemory};
    VC_META Meta;
    DWORD Bytes, i;
    BOOL Registered = FALSE, Mutated = FALSE;
    VcTestInitialize();
    Volume = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    CHECK(Volume != INVALID_HANDLE_VALUE);
    if (Volume == INVALID_HANDLE_VALUE)
        goto Done;
    CHECK(DeviceIoControl(Volume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                          &Number, sizeof(Number), &Bytes, NULL));
    CHECK(DeviceIoControl(Volume, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0,
                          &Partition, sizeof(Partition), &Bytes, NULL));
    CHECK(DeviceIoControl(Volume, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0,
                          Layout, sizeof(LayoutBuffer), &Bytes, NULL));
    if (Failures)
        goto Done;
    CHECK(Layout->PartitionStyle == PARTITION_STYLE_MBR && Layout->Mbr.Signature == 0x52574354 &&
          Partition.PartitionStyle == PARTITION_STYLE_MBR && Partition.PartitionNumber == 2 &&
          Partition.PartitionLength.QuadPart == 48 * 1024 * 1024);
    if (Failures)
        goto Done;
    swprintf(DiskPath, _countof(DiskPath), L"\\\\.\\PhysicalDrive%lu", Number.DeviceNumber);
    Disk = CreateFileW(DiskPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
    CHECK(Disk != INVALID_HANDLE_VALUE);
    Aligned = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(Aligned != NULL);
    if (Disk == INVALID_HANDLE_VALUE || !Aligned)
        goto Done;
    CHECK(Transfer(Disk, FALSE, Partition.StartingOffset.QuadPart, Aligned));
    CHECK(!memcmp(Aligned, "RWCRYPTTESTV1", 12));
    if (Failures)
        goto Done;
    memcpy(Plain, Aligned, 512);
    Memory.Bytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, VC_METADATA_SIZE);
    Request = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*Request) + VC_METADATA_SIZE);
    CHECK(Memory.Bytes && Request);
    if (!Memory.Bytes || !Request)
        goto Done;
    CHECK(VcCreate(&VcTestOps, &Header, "fixture-password", 16, "fixture-recovery", 16, Key, &Meta) == VcOk);
    CHECK(VcSetOffline(&VcTestOps, &Header, &Meta, 0) == VcOk);
    CHECK(RwCryptDescriptorCreate(&VcTestOps, &Request->Descriptor, 0, Layout->Mbr.Signature,
                                  Partition.StartingOffset.QuadPart, Partition.PartitionLength.QuadPart,
                                  NULL, NULL, Meta.Uuid));
    if (Failures)
        goto Done;
    Request->Version = RW_CRYPT_VERSION;
    Request->Size = sizeof(*Request);
    Request->HeaderSize = VC_METADATA_SIZE;
    memcpy(Request + 1, Memory.Bytes, VC_METADATA_SIZE);
    CHECK(VcCryptoXts(Key, 0, Aligned, 512, 1));
    Mutated = TRUE;
    CHECK(Transfer(Disk, TRUE, Partition.StartingOffset.QuadPart, Aligned));
    CHECK(FlushFileBuffers(Disk));
    Control = CreateFileW(L"\\\\.\\ReliefWinEncryption", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    CHECK(Control != INVALID_HANDLE_VALUE);
    if (Control == INVALID_HANDLE_VALUE || Failures)
        goto Done;
    Registered = DeviceIoControl(Control, RW_CRYPT_IOCTL_REGISTER, Request,
                                  sizeof(*Request) + VC_METADATA_SIZE, NULL, 0, &Bytes, NULL);
    CHECK(Registered);
    if (!Registered)
        goto Done;
    CHECK(!Transfer(Volume, FALSE, 0, Aligned) && GetLastError() == ERROR_ACCESS_DENIED);
    memset(Request->Key, 0, sizeof(Request->Key));
    CHECK(!DeviceIoControl(Control, RW_CRYPT_IOCTL_UNLOCK, Request,
                           sizeof(*Request) + VC_METADATA_SIZE, NULL, 0, &Bytes, NULL));
    memcpy(Request->Key, Key, sizeof(Key));
    CHECK(DeviceIoControl(Control, RW_CRYPT_IOCTL_UNLOCK, Request,
                          sizeof(*Request) + VC_METADATA_SIZE, NULL, 0, &Bytes, NULL));
    CHECK(Transfer(Volume, FALSE, 0, Aligned));
    CHECK(!memcmp(Aligned, Plain, 512));
    for (i = 0; i != 512; ++i)
        Aligned[i] = Expected[i] = (BYTE)(i * 13 + 7);
    CHECK(Transfer(Volume, TRUE, 0, Aligned));
    CHECK(!memcmp(Aligned, Expected, 512));
    CHECK(FlushFileBuffers(Volume));
    CHECK(VcCryptoXts(Key, 0, Expected, 512, 1));
    CHECK(Transfer(Disk, FALSE, Partition.StartingOffset.QuadPart, Aligned));
    CHECK(!memcmp(Aligned, Expected, 512));
    CHECK(DeviceIoControl(Control, RW_CRYPT_IOCTL_LOCK, Request, sizeof(*Request), NULL, 0, &Bytes, NULL));
    CHECK(!Transfer(Volume, FALSE, 0, Aligned) && GetLastError() == ERROR_ACCESS_DENIED);
Done:
    if (Registered)
        CHECK(DeviceIoControl(Control, RW_CRYPT_IOCTL_UNREGISTER, Request, sizeof(*Request), NULL, 0, &Bytes, NULL));
    if (Mutated)
    {
        memcpy(Aligned, Plain, 512);
        CHECK(Transfer(Disk, TRUE, Partition.StartingOffset.QuadPart, Aligned));
        CHECK(FlushFileBuffers(Disk));
    }
    if (Aligned)
    {
        VcWipe(Aligned, 4096);
        VirtualFree(Aligned, 0, MEM_RELEASE);
    }
    if (Memory.Bytes)
    {
        VcWipe(Memory.Bytes, VC_METADATA_SIZE);
        HeapFree(GetProcessHeap(), 0, Memory.Bytes);
    }
    if (Request)
    {
        VcWipe(Request, sizeof(*Request) + VC_METADATA_SIZE);
        HeapFree(GetProcessHeap(), 0, Request);
    }
    if (Control != INVALID_HANDLE_VALUE) CloseHandle(Control);
    if (Disk != INVALID_HANDLE_VALUE) CloseHandle(Disk);
    if (Volume != INVALID_HANDLE_VALUE) CloseHandle(Volume);
    VcWipe(Key, sizeof(Key));
    printf("%u data checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
