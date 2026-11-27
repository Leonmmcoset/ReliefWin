/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Bounded Windows I/O and detached metadata persistence
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"
#include <bcrypt.h>

static void *Allocate(void *Context, size_t Size)
{
    (void)Context;
    return HeapAlloc(GetProcessHeap(), 0, Size);
}

static void Free(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    VcWipe(Buffer, Size);
    HeapFree(GetProcessHeap(), 0, Buffer);
}

static int Random(void *Context, void *Buffer, size_t Size)
{
    (void)Context;
    return Size <= MAXDWORD && BCryptGenRandom(NULL, Buffer, (ULONG)Size, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
}

void VcCryptoFatal(uint32_t Code)
{
    TerminateProcess(GetCurrentProcess(), Code);
}

const VC_OPS DcOps = {NULL, Allocate, Free, Random, VcCryptoHash, VcCryptoHmac, VcCryptoPbkdf2, VcCryptoXts};

DWORD DcError(VC_RESULT Result)
{
    switch (Result)
    {
        case VcOk: case VcFinished: return ERROR_SUCCESS;
        case VcInvalid: return ERROR_INVALID_DATA;
        case VcUnsupported: return ERROR_NOT_SUPPORTED;
        case VcNoMemory: return ERROR_NOT_ENOUGH_MEMORY;
        case VcIoError: return ERROR_WRITE_FAULT;
        case VcWrongSecret: return ERROR_INVALID_PASSWORD;
        case VcIncomplete: return ERROR_NOT_READY;
        case VcConflict: return ERROR_REVISION_MISMATCH;
        default: return ERROR_CRC;
    }
}

static int FileTransfer(void *Context, uint64_t Offset, void *Buffer, size_t Size, BOOL Write)
{
    DC_FILE *File = Context;
    LARGE_INTEGER Position;
    DWORD Done;
    if (Offset > File->Size || Size > File->Size - Offset || Size > MAXDWORD ||
        File->Base > MAXLONGLONG || Offset > MAXLONGLONG - File->Base)
        return 0;
    Position.QuadPart = File->Base + Offset;
    return SetFilePointerEx(File->Handle, Position, NULL, FILE_BEGIN) &&
           (Write ? WriteFile(File->Handle, Buffer, (DWORD)Size, &Done, NULL) :
                    ReadFile(File->Handle, Buffer, (DWORD)Size, &Done, NULL)) && Done == Size;
}

static int FileRead(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    return FileTransfer(Context, Offset, Buffer, Size, FALSE);
}

static int FileWrite(void *Context, uint64_t Offset, const void *Buffer, size_t Size)
{
    return FileTransfer(Context, Offset, (void *)Buffer, Size, TRUE);
}

static int FileFlush(void *Context)
{
    return FlushFileBuffers(((DC_FILE *)Context)->Handle);
}

void DcFileIo(DC_FILE *File, HANDLE Handle, ULONGLONG Base, ULONGLONG Size, VC_IO *Io)
{
    File->Handle = Handle;
    File->Base = Base;
    File->Size = Size;
    Io->Context = File;
    Io->Size = Size;
    Io->Read = FileRead;
    Io->Write = FileWrite;
    Io->Flush = FileFlush;
}

DWORD DcOpenFile(PCWSTR Path, ULONGLONG Size, DWORD Creation, DC_FILE *File, VC_IO *Io)
{
    LARGE_INTEGER Length;
    DWORD Error;
    File->Handle = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE, 0, NULL, Creation,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if (File->Handle == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (Creation != OPEN_EXISTING)
    {
        Length.QuadPart = Size;
        if (!SetFilePointerEx(File->Handle, Length, NULL, FILE_BEGIN) ||
            !SetEndOfFile(File->Handle) || !FlushFileBuffers(File->Handle))
            goto Failure;
    }
    if (!GetFileSizeEx(File->Handle, &Length))
        goto Failure;
    if (Length.QuadPart != Size)
    {
        SetLastError(ERROR_INVALID_DATA);
        goto Failure;
    }
    DcFileIo(File, File->Handle, 0, Size, Io);
    return ERROR_SUCCESS;
Failure:
    Error = GetLastError();
    CloseHandle(File->Handle);
    File->Handle = INVALID_HANDLE_VALUE;
    if (Creation == CREATE_NEW) DeleteFileW(Path);
    return Error;
}

DWORD DcReadWhole(PCWSTR Path, BYTE *Buffer, DWORD Size)
{
    HANDLE File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    LARGE_INTEGER Length;
    DWORD Done, Error = ERROR_SUCCESS;
    if (File == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (!GetFileSizeEx(File, &Length) || Length.QuadPart != Size ||
        !ReadFile(File, Buffer, Size, &Done, NULL) || Done != Size)
        Error = ERROR_INVALID_DATA;
    CloseHandle(File);
    return Error;
}

DWORD DcWriteWhole(PCWSTR Path, const BYTE *Buffer, DWORD Size, DWORD Creation)
{
    DC_FILE File;
    VC_IO Io;
    DWORD Error = DcOpenFile(Path, Size, Creation, &File, &Io);
    if (Error)
        return Error;
    if (!Io.Write(Io.Context, 0, Buffer, Size) || !Io.Flush(Io.Context))
        Error = ERROR_WRITE_FAULT;
    CloseHandle(File.Handle);
    if (Error && Creation == CREATE_NEW) DeleteFileW(Path);
    return Error;
}

DWORD DcHeaderPath(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor, PWSTR Path, BOOL Journal)
{
    return FAILED(StringCchPrintfW(Path, DC_PATH, L"%sEFI\\ReliefWin\\Encryption\\%S.%s",
                                   Store->Root, Descriptor->Bytes + 72, Journal ? L"journal" : L"luks")) ?
           ERROR_FILENAME_EXCED_RANGE : ERROR_SUCCESS;
}

DWORD DcReadStore(DC_STORE *Store)
{
    BYTE *A = Allocate(NULL, RW_CRYPT_INDEX_SIZE), *B = Allocate(NULL, RW_CRYPT_INDEX_SIZE), *Newest;
    WCHAR Path[DC_PATH];
    uint64_t SeqA = 0, SeqB = 0;
    uint32_t CountA = 0, CountB = 0;
    DWORD ErrorA, ErrorB, Error = ERROR_SUCCESS;
    BOOL ValidA, ValidB;
    if (!A || !B)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Done;
    }
    StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root, &RW_CRYPT_INDEX_PATH[1]);
    ErrorA = DcReadWhole(Path, A, RW_CRYPT_INDEX_SIZE);
    StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root, &RW_CRYPT_INDEX_BACKUP_PATH[1]);
    ErrorB = DcReadWhole(Path, B, RW_CRYPT_INDEX_SIZE);
    if ((ErrorA == ERROR_FILE_NOT_FOUND || ErrorA == ERROR_PATH_NOT_FOUND) &&
        (ErrorB == ERROR_FILE_NOT_FOUND || ErrorB == ERROR_PATH_NOT_FOUND))
    {
        Store->Count = 0;
        Store->Sequence = 0;
        Store->First = 0;
        goto Done;
    }
    ValidA = !ErrorA && RwCryptIndexRead(&DcOps, A, RW_CRYPT_INDEX_SIZE, &SeqA, &CountA);
    ValidB = !ErrorB && RwCryptIndexRead(&DcOps, B, RW_CRYPT_INDEX_SIZE, &SeqB, &CountB);
    if ((!ValidA && !ValidB) ||
        (ValidA && ValidB && SeqA == SeqB && memcmp(A, B, RW_CRYPT_INDEX_SIZE)))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    if (ValidA && (!ValidB || SeqA >= SeqB))
    {
        Newest = A;
        Store->Sequence = SeqA;
        Store->Count = CountA;
        Store->First = 1;
    }
    else
    {
        Newest = B;
        Store->Sequence = SeqB;
        Store->Count = CountB;
        Store->First = 0;
    }
    memcpy(Store->Volumes, Newest + 512, Store->Count * sizeof(Store->Volumes[0]));
Done:
    if (A) Free(NULL, A, RW_CRYPT_INDEX_SIZE);
    if (B) Free(NULL, B, RW_CRYPT_INDEX_SIZE);
    return Error;
}

DWORD DcWriteStore(DC_STORE *Store)
{
    BYTE *Buffer = Allocate(NULL, RW_CRYPT_INDEX_SIZE), *Check = Allocate(NULL, RW_CRYPT_INDEX_SIZE);
    WCHAR Path[DC_PATH];
    unsigned i;
    DWORD Error = ERROR_SUCCESS;
    if (!Buffer || !Check)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Done;
    }
    if (Store->Sequence == UINT64_MAX ||
        !RwCryptIndexCreate(&DcOps, Buffer, Store->Sequence + 1, Store->Volumes, Store->Count))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    StringCchPrintfW(Path, DC_PATH, L"%sEFI", Store->Root);
    CreateDirectoryW(Path, NULL);
    StringCchPrintfW(Path, DC_PATH, L"%sEFI\\ReliefWin", Store->Root);
    CreateDirectoryW(Path, NULL);
    StringCchPrintfW(Path, DC_PATH, L"%sEFI\\ReliefWin\\Encryption", Store->Root);
    if (!CreateDirectoryW(Path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        Error = GetLastError();
        goto Done;
    }
    for (i = 0; i != 2; ++i)
    {
        StringCchPrintfW(Path, DC_PATH, L"%s%s", Store->Root,
                         (Store->First ^ i) ? &RW_CRYPT_INDEX_BACKUP_PATH[1] : &RW_CRYPT_INDEX_PATH[1]);
        Error = DcWriteWhole(Path, Buffer, RW_CRYPT_INDEX_SIZE, OPEN_ALWAYS);
        if (Error)
            goto Done;
        Error = DcReadWhole(Path, Check, RW_CRYPT_INDEX_SIZE);
        if (!Error && memcmp(Buffer, Check, RW_CRYPT_INDEX_SIZE))
            Error = ERROR_CRC;
        if (Error)
            goto Done;
    }
    ++Store->Sequence;
    Store->First = 1;
Done:
    if (Buffer) Free(NULL, Buffer, RW_CRYPT_INDEX_SIZE);
    if (Check) Free(NULL, Check, RW_CRYPT_INDEX_SIZE);
    return Error;
}

static int MemoryRead(void *Context, uint64_t Offset, void *Buffer, size_t Size)
{
    if (Offset > VC_METADATA_SIZE || Size > VC_METADATA_SIZE - Offset)
        return 0;
    memcpy(Buffer, (BYTE *)Context + (size_t)Offset, Size);
    return 1;
}

static int MemoryWrite(void *Context, uint64_t Offset, const void *Buffer, size_t Size)
{
    if (Offset > VC_METADATA_SIZE || Size > VC_METADATA_SIZE - Offset)
        return 0;
    memcpy((BYTE *)Context + (size_t)Offset, Buffer, Size);
    return 1;
}

static int MemoryFlush(void *Context)
{
    (void)Context;
    return 1;
}

void DcMemoryIo(BYTE *Bytes, VC_IO *Io)
{
    Io->Context = Bytes;
    Io->Size = VC_METADATA_SIZE;
    Io->Read = MemoryRead;
    Io->Write = MemoryWrite;
    Io->Flush = MemoryFlush;
}

DWORD DcLoadHeader(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor, BYTE **Bytes, VC_META *Meta)
{
    WCHAR Path[DC_PATH];
    DWORD Error;
    VC_IO Io = {0};
    *Bytes = Allocate(NULL, VC_METADATA_SIZE);
    if (!*Bytes)
        return ERROR_NOT_ENOUGH_MEMORY;
    DcHeaderPath(Store, Descriptor, Path, FALSE);
    Error = DcReadWhole(Path, *Bytes, VC_METADATA_SIZE);
    if (!Error)
    {
        Io.Context = *Bytes;
        Io.Size = VC_METADATA_SIZE;
        Io.Read = MemoryRead;
        Error = DcError(VcLoad(&DcOps, &Io, Meta));
        if (!Error && (strcmp(Meta->Uuid, (char *)Descriptor->Bytes + 72) || Meta->PayloadOffset ||
            (Meta->PayloadSize && Meta->PayloadSize != RwCryptGet64(Descriptor->Bytes + 32))))
            Error = ERROR_INVALID_DATA;
    }
    if (Error)
    {
        Free(NULL, *Bytes, VC_METADATA_SIZE);
        *Bytes = NULL;
    }
    return Error;
}

DWORD DcSecret(DC_STORE *Store, const RW_CRYPT_DESCRIPTOR *Descriptor, const char *Secret,
                BYTE **Bytes, VC_META *Meta, BYTE Key[VC_KEY_SIZE])
{
    VC_IO Io = {0};
    DWORD Error = DcLoadHeader(Store, Descriptor, Bytes, Meta);
    VcWipe(Key, VC_KEY_SIZE);
    if (Error)
        return Error;
    Io.Context = *Bytes;
    Io.Size = VC_METADATA_SIZE;
    Io.Read = MemoryRead;
    Error = DcError(VcUnlock(&DcOps, &Io, Meta, Secret, strlen(Secret), Key));
    if (Error)
    {
        Free(NULL, *Bytes, VC_METADATA_SIZE);
        *Bytes = NULL;
    }
    return Error;
}

DWORD DcControl(ULONG Code, const RW_CRYPT_DESCRIPTOR *Descriptor, const BYTE *Header, const BYTE *Key)
{
    DWORD Size = sizeof(RW_CRYPT_REQUEST) + (Header ? VC_METADATA_SIZE : 0), Bytes, Error;
    RW_CRYPT_REQUEST *Request = Allocate(NULL, Size);
    HANDLE Control;
    if (!Request)
        return ERROR_NOT_ENOUGH_MEMORY;
    memset(Request, 0, Size);
    Request->Version = RW_CRYPT_VERSION;
    Request->Size = sizeof(*Request);
    Request->Descriptor = *Descriptor;
    Request->HeaderSize = Header ? VC_METADATA_SIZE : 0;
    if (Header) memcpy(Request + 1, Header, VC_METADATA_SIZE);
    if (Key) memcpy(Request->Key, Key, VC_KEY_SIZE);
    Control = CreateFileW(L"\\\\.\\ReliefWinEncryption", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (Control == INVALID_HANDLE_VALUE)
        Error = GetLastError();
    else
    {
        Error = DeviceIoControl(Control, Code, Request, Size, NULL, 0, &Bytes, NULL) ? ERROR_SUCCESS : GetLastError();
        CloseHandle(Control);
    }
    Free(NULL, Request, Size);
    return Error;
}
