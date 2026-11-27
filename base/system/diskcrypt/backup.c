/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Verified recovery exports and authenticated detached-header restoration
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"

#define DC_BACKUP_SIZE (1024 + VC_METADATA_SIZE)

static DWORD BackupLocation(DC_STORE *Store, DC_VOLUME *Volume, PCWSTR Path)
{
    WCHAR Root[DC_PATH], Name[DC_PATH];
    if (!Path || !*Path || !GetVolumePathNameW(Path, Root, DC_PATH) ||
        !GetVolumeNameForVolumeMountPointW(Root, Name, DC_PATH))
        return ERROR_INVALID_NAME;
    if (!_wcsicmp(Name, Volume->Root) || !_wcsicmp(Name, Store->Root))
        return ERROR_ACCESS_DENIED;
    return ERROR_SUCCESS;
}

DWORD DcExport(DC_STORE *Store, DC_VOLUME *Volume, const RW_CRYPT_DESCRIPTOR *Descriptor,
                const BYTE *Header, const char *Recovery, PCWSTR Path)
{
    BYTE *Data = NULL, *Check = NULL;
    WCHAR KeyPath[DC_PATH];
    char KeyText[256] = {0};
    BOOL Created = FALSE, KeyCreated = FALSE;
    DWORD Error = BackupLocation(Store, Volume, Path);
    size_t Length = Recovery ? strlen(Recovery) : 0;
    if (Error) return Error;
    if (Length && Length != 64) return ERROR_INVALID_PARAMETER;
    Data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, DC_BACKUP_SIZE);
    Check = HeapAlloc(GetProcessHeap(), 0, DC_BACKUP_SIZE);
    if (!Data || !Check)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Done;
    }
    memcpy(Data, "RWCREC01", 8);
    Data[11] = 1;
    Data[15] = (BYTE)Length;
    if (Length) memcpy(Data + 16, Recovery, Length);
    DcOps.Hash(Header, VC_METADATA_SIZE, Data + 80);
    DcOps.Hash(Descriptor, sizeof(*Descriptor), Data + 112);
    DcOps.Hash(Data, 480, Data + 480);
    memcpy(Data + 512, Descriptor, sizeof(*Descriptor));
    memcpy(Data + 1024, Header, VC_METADATA_SIZE);
    Error = DcWriteWhole(Path, Data, DC_BACKUP_SIZE, CREATE_NEW);
    Created = !Error;
    if (!Error) Error = DcReadWhole(Path, Check, DC_BACKUP_SIZE);
    if (!Error && !VcEqual(Data, Check, DC_BACKUP_SIZE)) Error = ERROR_CRC;
    if (!Error && Length)
    {
        if (FAILED(StringCchPrintfW(KeyPath, DC_PATH, L"%s.txt", Path)) ||
            FAILED(StringCchPrintfA(KeyText, sizeof(KeyText),
                                    "ReliefWin Disk Encryption\r\nVolume: %s\r\nRecovery key: %s\r\n",
                                    Descriptor->Bytes + 72, Recovery)))
            Error = ERROR_FILENAME_EXCED_RANGE;
        else
        {
            DWORD Bytes = (DWORD)strlen(KeyText);
            Error = DcWriteWhole(KeyPath, (const BYTE *)KeyText, Bytes, CREATE_NEW);
            KeyCreated = !Error;
            if (!Error) Error = DcReadWhole(KeyPath, Check, Bytes);
            if (!Error && !VcEqual(KeyText, Check, Bytes)) Error = ERROR_CRC;
        }
    }
Done:
    if (Error && Created) DeleteFileW(Path);
    if (Error && KeyCreated) DeleteFileW(KeyPath);
    VcWipe(KeyText, sizeof(KeyText));
    if (Data) DcOps.Free(NULL, Data, DC_BACKUP_SIZE);
    if (Check) DcOps.Free(NULL, Check, DC_BACKUP_SIZE);
    return Error;
}

DWORD DcImport(DC_STORE *Store, DC_VOLUME *Volume, const char *Secret, PCWSTR Path)
{
    BYTE *Data = HeapAlloc(GetProcessHeap(), 0, DC_BACKUP_SIZE), Hash[32], Key[VC_KEY_SIZE];
    BYTE *Current = NULL;
    HANDLE Hold = INVALID_HANDLE_VALUE;
    RW_CRYPT_DESCRIPTOR *Descriptor;
    VC_IO HeaderIo, JournalIo;
    DC_FILE Journal = {INVALID_HANDLE_VALUE};
    VC_META Meta;
    VC_PROGRESS Progress;
    WCHAR HeaderPath[DC_PATH], JournalPath[DC_PATH], Previous[DC_PATH];
    char Saved[65];
    DWORD Error, i;
    if (!Data) return ERROR_NOT_ENOUGH_MEMORY;
    if (Volume->Running || Volume->Boot || (Volume->Flags & (RW_CRYPT_FLAG_UNLOCKED | RW_CRYPT_FLAG_SYSTEM)))
    {
        Error = ERROR_ACCESS_DENIED;
        goto Done;
    }
    Error = DcReadWhole(Path, Data, DC_BACKUP_SIZE);
    if (Error) goto Done;
    DcOps.Hash(Data, 480, Hash);
    if (memcmp(Data, "RWCREC01", 8) || RwCryptGet32(Data + 8) != 1 ||
        (RwCryptGet32(Data + 12) != 0 && RwCryptGet32(Data + 12) != 64) ||
        !VcEqual(Hash, Data + 480, 32))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    for (i = 144; i < 480; ++i)
        if (Data[i])
        {
            Error = ERROR_INVALID_DATA;
            goto Done;
        }
    Descriptor = (RW_CRYPT_DESCRIPTOR *)(Data + 512);
    DcOps.Hash(Descriptor, sizeof(*Descriptor), Hash);
    if (!VcEqual(Hash, Data + 112, 32) || !RwCryptDescriptorValid(&DcOps, Descriptor) ||
        !RwCryptDescriptorMatch(Descriptor, Volume->Partition.PartitionStyle,
                                Volume->Signature, Volume->Partition.StartingOffset.QuadPart,
                                Volume->Partition.PartitionLength.QuadPart, &Volume->DiskGuid,
                                &Volume->Partition.Gpt.PartitionId))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    DcOps.Hash(Data + 1024, VC_METADATA_SIZE, Hash);
    if (!VcEqual(Hash, Data + 80, 32) ||
        (Volume->Index >= 0 && memcmp(Store->Volumes + Volume->Index, Descriptor, sizeof(*Descriptor))))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    DcMemoryIo(Data + 1024, &HeaderIo);
    Error = DcError(VcLoad(&DcOps, &HeaderIo, &Meta));
    if (Error) goto Done;
    if (strcmp(Meta.Uuid, (char *)Descriptor->Bytes + 72) || Meta.PayloadOffset ||
        (Meta.PayloadSize && Meta.PayloadSize != (ULONGLONG)Volume->Partition.PartitionLength.QuadPart))
    {
        Error = ERROR_INVALID_DATA;
        goto Done;
    }
    memcpy(Saved, Data + 16, 64);
    Saved[RwCryptGet32(Data + 12)] = 0;
    if (!Secret || !*Secret) Secret = Saved;
    Error = DcError(VcUnlock(&DcOps, &HeaderIo, &Meta, Secret, strlen(Secret), Key));
    if (Error) goto Done;
    DcHeaderPath(Store, Descriptor, JournalPath, TRUE);
    Error = DcOpenFile(JournalPath, VC_JOURNAL_SIZE, OPEN_EXISTING, &Journal, &JournalIo);
    if (!Error)
    {
        VC_META Offline = Meta;
        Offline.Incomplete = 1;
        Error = DcError(VcJournalRead(&DcOps, &JournalIo, &Offline, Key, Descriptor->Bytes + 480,
                                     Volume->Partition.PartitionLength.QuadPart, &Progress));
        if (!Error) Error = DcError(VcSetOffline(&DcOps, &HeaderIo, &Meta, 1));
    }
    else if (Error == ERROR_FILE_NOT_FOUND || Error == ERROR_PATH_NOT_FOUND)
        Error = Meta.Incomplete ? ERROR_NOT_READY : ERROR_SUCCESS;
    if (Error) goto Done;
    Error = DcExclusive(Volume, &Hold);
    if (Error) goto Done;
    DcHeaderPath(Store, Descriptor, HeaderPath, FALSE);
    StringCchPrintfW(Previous, DC_PATH, L"%s.before-restore", HeaderPath);
    if (GetFileAttributesW(HeaderPath) != INVALID_FILE_ATTRIBUTES && !CopyFileW(HeaderPath, Previous, TRUE))
    {
        Error = GetLastError();
        goto Done;
    }
    Error = DcWriteWhole(HeaderPath, Data + 1024, VC_METADATA_SIZE, CREATE_ALWAYS);
    if (Error) goto Done;
    Current = DcOps.Allocate(NULL, VC_METADATA_SIZE);
    if (!Current)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto Done;
    }
    Error = DcReadWhole(HeaderPath, Current, VC_METADATA_SIZE);
    if (!Error && !VcEqual(Current, Data + 1024, VC_METADATA_SIZE)) Error = ERROR_CRC;
    if (Error) goto Done;
    if (Volume->Index < 0)
    {
        if (Store->Count == RW_CRYPT_MAX_VOLUMES)
        {
            Error = ERROR_NOT_ENOUGH_MEMORY;
            goto Done;
        }
        Store->Volumes[Store->Count++] = *Descriptor;
        Error = DcWriteStore(Store);
        if (Error) goto Done;
    }
    Error = DcControl(RW_CRYPT_IOCTL_REGISTER, Descriptor, Current, NULL);
Done:
    if (Hold != INVALID_HANDLE_VALUE) CloseHandle(Hold);
    if (Journal.Handle != INVALID_HANDLE_VALUE) CloseHandle(Journal.Handle);
    if (Current) DcOps.Free(NULL, Current, VC_METADATA_SIZE);
    VcWipe(Saved, sizeof(Saved));
    VcWipe(Key, sizeof(Key));
    DcOps.Free(NULL, Data, DC_BACKUP_SIZE);
    return Error;
}
