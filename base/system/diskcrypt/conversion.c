/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Exclusive journaled in-place conversion and credential management
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"

DWORD DcExclusive(DC_VOLUME *Volume, HANDLE *Handle)
{
    DWORD Done, Error;
    *Handle = CreateFileW(Volume->Device, GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                          FILE_FLAG_WRITE_THROUGH |
                          (((Volume->Flags & (RW_CRYPT_FLAG_CONFIGURED | RW_CRYPT_FLAG_UNLOCKED)) ==
                            RW_CRYPT_FLAG_CONFIGURED) ? FILE_FLAG_NO_BUFFERING : 0), NULL);
    if (*Handle == INVALID_HANDLE_VALUE)
        return GetLastError();
    if (!(Volume->Flags & RW_CRYPT_FLAG_CONFIGURED) ||
        (Volume->Flags & RW_CRYPT_FLAG_UNLOCKED))
    {
        if (!DeviceIoControl(*Handle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &Done, NULL) ||
            !FlushFileBuffers(*Handle) ||
            !DeviceIoControl(*Handle, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &Done, NULL))
        {
            Error = GetLastError();
            CloseHandle(*Handle);
            *Handle = INVALID_HANDLE_VALUE;
            return Error;
        }
    }
    return ERROR_SUCCESS;
}

DWORD DcSetLock(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret, BOOL Unlock)
{
    HANDLE Hold = INVALID_HANDLE_VALUE;
    BYTE *Header = NULL, Key[VC_KEY_SIZE];
    VC_META Meta;
    DWORD Error;
    if (Volume->Index < 0 || Volume->Running || (Volume->Flags & RW_CRYPT_FLAG_SYSTEM))
        return ERROR_ACCESS_DENIED;
    if (!Unlock)
    {
        Error = DcExclusive(Volume, &Hold);
        if (Error)
            return Error;
        Error = DcControl(RW_CRYPT_IOCTL_LOCK, Store->Volumes + Volume->Index, NULL, NULL);
    }
    else
    {
        Error = DcSecret(Store, Store->Volumes + Volume->Index, Secret, &Header, &Meta, Key);
        if (!Error && Meta.Incomplete)
            Error = ERROR_NOT_READY;
        if (!Error)
            Error = DcControl(RW_CRYPT_IOCTL_REGISTER, Store->Volumes + Volume->Index, Header, NULL);
        if (!Error)
            Error = DcControl(RW_CRYPT_IOCTL_UNLOCK, Store->Volumes + Volume->Index, Header, Key);
    }
    if (Hold != INVALID_HANDLE_VALUE) CloseHandle(Hold);
    if (Header) DcOps.Free(NULL, Header, VC_METADATA_SIZE);
    VcWipe(Key, sizeof(Key));
    return Error;
}

DWORD DcConvert(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret,
                const char *Recovery, PCWSTR Backup, BOOL Decrypt, volatile LONG *Cancel,
                DC_PROGRESS Notify, void *Context)
{
    HANDLE Hold = INVALID_HANDLE_VALUE;
    DC_FILE HeaderFile = {INVALID_HANDLE_VALUE}, JournalFile = {INVALID_HANDLE_VALUE}, DiskFile = {INVALID_HANDLE_VALUE};
    VC_IO HeaderIo, JournalIo, DiskIo;
    VC_META Meta;
    VC_PROGRESS Progress;
    BYTE Key[VC_KEY_SIZE], *Bytes = NULL;
    RW_CRYPT_DESCRIPTOR Descriptor;
    WCHAR HeaderPath[DC_PATH], JournalPath[DC_PATH], Path[DC_PATH];
    DWORD Error, Index;
    BOOL Fresh = Volume->Index < 0;
    VC_RESULT Result;
    if (Volume->Running || Volume->Boot || !_wcsicmp(Volume->Root, Store->Root) ||
        (Volume->Flags & RW_CRYPT_FLAG_SYSTEM) ||
        (Fresh && (Decrypt || Store->Count == RW_CRYPT_MAX_VOLUMES)) ||
        Volume->Partition.PartitionLength.QuadPart <= 0 || Volume->Partition.PartitionLength.QuadPart % 512)
        return ERROR_ACCESS_DENIED;
    if (Volume->System && !DcRecoveryEnvironment())
        return ERROR_NOT_SUPPORTED;
    if (Volume->System)
    {
        Error = DcPrepareBoot(Store, Volume);
        if (Error) return Error;
    }
    Error = DcExclusive(Volume, &Hold);
    if (Error)
        return Error;
    StringCchPrintfW(Path, DC_PATH, L"\\\\.\\PhysicalDrive%lu", Volume->Disk);
    DiskFile.Handle = CreateFileW(Path, GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                  FILE_FLAG_WRITE_THROUGH, NULL);
    if (DiskFile.Handle == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        goto Done;
    }
    DcFileIo(&DiskFile, DiskFile.Handle, Volume->Partition.StartingOffset.QuadPart,
              Volume->Partition.PartitionLength.QuadPart, &DiskIo);
    if (Fresh)
    {
        if (!Recovery || !*Recovery || !Backup || !*Backup)
        {
            Error = ERROR_INVALID_PARAMETER;
            goto Done;
        }
        Bytes = DcOps.Allocate(NULL, VC_METADATA_SIZE);
        if (!Bytes)
        {
            Error = ERROR_NOT_ENOUGH_MEMORY;
            goto Done;
        }
        memset(Bytes, 0, VC_METADATA_SIZE);
        DcMemoryIo(Bytes, &HeaderIo);
        Error = DcError(VcCreate(&DcOps, &HeaderIo, Secret, strlen(Secret), Recovery, strlen(Recovery), Key, &Meta));
        if (Error) goto Done;
        if (!RwCryptDescriptorCreate(&DcOps, &Descriptor, Volume->Partition.PartitionStyle,
                                     Volume->Signature, Volume->Partition.StartingOffset.QuadPart,
                                     Volume->Partition.PartitionLength.QuadPart, &Volume->DiskGuid,
                                     &Volume->Partition.Gpt.PartitionId, Meta.Uuid))
        {
            Error = ERROR_INVALID_DATA;
            goto Done;
        }
        if (Volume->System && !RwCryptDescriptorSetSystem(&DcOps, &Descriptor))
        {
            Error = ERROR_INVALID_DATA;
            goto Done;
        }
        Error = DcWriteStore(Store);
        if (Error) goto Done;
        DcHeaderPath(Store, &Descriptor, HeaderPath, FALSE);
        Error = DcExport(Store, Volume, &Descriptor, Bytes, Recovery, Backup);
        if (Error) goto Done;
        Error = DcWriteWhole(HeaderPath, Bytes, VC_METADATA_SIZE, CREATE_NEW);
        if (Error) goto Done;
        Error = DcOpenFile(HeaderPath, VC_METADATA_SIZE, OPEN_EXISTING, &HeaderFile, &HeaderIo);
        if (Error) goto Done;
    }
    else
    {
        Descriptor = Store->Volumes[Volume->Index];
        Error = DcSecret(Store, &Descriptor, Secret, &Bytes, &Meta, Key);
        if (Error) goto Done;
        DcHeaderPath(Store, &Descriptor, HeaderPath, FALSE);
        Error = DcOpenFile(HeaderPath, VC_METADATA_SIZE, OPEN_EXISTING, &HeaderFile, &HeaderIo);
        if (Error) goto Done;
        if (!Meta.Incomplete && !Decrypt)
        {
            Error = ERROR_ALREADY_EXISTS;
            goto Done;
        }
    }
    DcHeaderPath(Store, &Descriptor, JournalPath, TRUE);
    Error = DcOpenFile(JournalPath, VC_JOURNAL_SIZE,
                       (Fresh || !Meta.Incomplete) ? CREATE_ALWAYS : OPEN_EXISTING, &JournalFile, &JournalIo);
    if (Error) goto Done;
    if (Fresh || !Meta.Incomplete)
    {
        VC_META Offline = Meta;
        Offline.Incomplete = 1;
        Error = DcError(VcJournalBegin(&DcOps, &JournalIo, &Offline, Key,
                                      Descriptor.Bytes + 480, DiskIo.Size, Decrypt));
        if (Error) goto Done;
        if (!Meta.Incomplete)
        {
            Error = DcError(VcSetOffline(&DcOps, &HeaderIo, &Meta, 1));
            if (Error) goto Done;
        }
    }
    Error = DcError(VcJournalRead(&DcOps, &JournalIo, &Meta, Key,
                                 Descriptor.Bytes + 480, DiskIo.Size, &Progress));
    if (Error) goto Done;
    if (Fresh)
    {
        Index = Store->Count++;
        Store->Volumes[Index] = Descriptor;
        Error = DcWriteStore(Store);
        if (Error) goto Done;
    }
    CloseHandle(HeaderFile.Handle);
    HeaderFile.Handle = INVALID_HANDLE_VALUE;
    if (Bytes) DcOps.Free(NULL, Bytes, VC_METADATA_SIZE);
    Bytes = NULL;
    Error = DcLoadHeader(Store, &Descriptor, &Bytes, &Meta);
    if (Error) goto Done;
    Error = DcControl(RW_CRYPT_IOCTL_REGISTER, &Descriptor, Bytes, NULL);
    if (Error) goto Done;
    Error = DcOpenFile(HeaderPath, VC_METADATA_SIZE, OPEN_EXISTING, &HeaderFile, &HeaderIo);
    if (Error) goto Done;
    for (;;)
    {
        if (Notify) Notify(Context, &Progress);
        if (Progress.Completed == Progress.Total && !Progress.Pending)
            break;
        if (InterlockedCompareExchange(Cancel, 0, 0))
        {
            Error = ERROR_OPERATION_ABORTED;
            goto Done;
        }
        Result = VcConvertStep(&DcOps, &JournalIo, &DiskIo, &Meta, Key, Descriptor.Bytes + 480, &Progress);
        if (Result != VcOk && Result != VcFinished)
        {
            Error = DcError(Result);
            goto Done;
        }
    }
    if (Progress.Decrypt)
    {
        for (Index = 0; Index < Store->Count; ++Index)
            if (!memcmp(Store->Volumes + Index, &Descriptor, sizeof(Descriptor))) break;
        if (Index == Store->Count)
        {
            Error = ERROR_INVALID_DATA;
            goto Done;
        }
        --Store->Count;
        memmove(Store->Volumes + Index, Store->Volumes + Index + 1,
                 (Store->Count - Index) * sizeof(Descriptor));
        Error = DcWriteStore(Store);
        if (!Error) Error = DcControl(RW_CRYPT_IOCTL_UNREGISTER, &Descriptor, NULL, NULL);
    }
    else
    {
        Error = DcError(VcSetOffline(&DcOps, &HeaderIo, &Meta, 0));
        if (!Error)
        {
            CloseHandle(HeaderFile.Handle);
            HeaderFile.Handle = INVALID_HANDLE_VALUE;
            DcOps.Free(NULL, Bytes, VC_METADATA_SIZE);
            Bytes = NULL;
            Error = DcLoadHeader(Store, &Descriptor, &Bytes, &Meta);
            if (!Error) Error = DcControl(RW_CRYPT_IOCTL_REGISTER, &Descriptor, Bytes, NULL);
        }
    }
    if (!Error)
    {
        CloseHandle(JournalFile.Handle);
        JournalFile.Handle = INVALID_HANDLE_VALUE;
        if (Progress.Decrypt)
        {
            DeleteFileW(JournalPath);
            CloseHandle(HeaderFile.Handle);
            HeaderFile.Handle = INVALID_HANDLE_VALUE;
            DeleteFileW(HeaderPath);
        }
    }
Done:
    if (HeaderFile.Handle != INVALID_HANDLE_VALUE) CloseHandle(HeaderFile.Handle);
    if (JournalFile.Handle != INVALID_HANDLE_VALUE) CloseHandle(JournalFile.Handle);
    if (DiskFile.Handle != INVALID_HANDLE_VALUE) CloseHandle(DiskFile.Handle);
    if (Hold != INVALID_HANDLE_VALUE) CloseHandle(Hold);
    if (Bytes) DcOps.Free(NULL, Bytes, VC_METADATA_SIZE);
    VcWipe(Key, sizeof(Key));
    return Error;
}

DWORD DcChange(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret,
               const char *Replacement, BOOL Recovery, PCWSTR Backup)
{
    BYTE *Header = NULL, Key[VC_KEY_SIZE];
    VC_META Meta;
    DC_FILE File;
    VC_IO Io;
    WCHAR Path[DC_PATH];
    DWORD Error;
    if (Volume->Index < 0)
        return ERROR_NOT_FOUND;
    Error = DcSecret(Store, Store->Volumes + Volume->Index, Secret, &Header, &Meta, Key);
    if (Error) return Error;
    if (Recovery)
    {
        DcMemoryIo(Header, &Io);
        Error = DcError(VcChangeSecret(&DcOps, &Io, &Meta, Key, 1, Replacement, strlen(Replacement)));
        if (!Error) Error = DcExport(Store, Volume, Store->Volumes + Volume->Index, Header, Replacement, Backup);
        if (Error) goto Done;
        Error = DcError(VcLoad(&DcOps, &Io, &Meta));
        if (Error) goto Done;
    }
    DcHeaderPath(Store, Store->Volumes + Volume->Index, Path, FALSE);
    Error = DcOpenFile(Path, VC_METADATA_SIZE, OPEN_EXISTING, &File, &Io);
    if (!Error)
    {
        Error = DcError(VcLoad(&DcOps, &Io, &Meta));
        if (!Error) Error = DcError(VcChangeSecret(&DcOps, &Io, &Meta, Key, Recovery ? 1 : 0,
                                                   Replacement, strlen(Replacement)));
        CloseHandle(File.Handle);
    }
Done:
    DcOps.Free(NULL, Header, VC_METADATA_SIZE);
    VcWipe(Key, sizeof(Key));
    return Error;
}
