/*
 * PROJECT:     ReliefWin Disk Encryption
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Stable partition enumeration and encryption state discovery
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "diskcrypt.h"

BOOL DcRecoveryEnvironment(void)
{
    WCHAR Windows[MAX_PATH], Device[64], Path[] = L"C:";
    HANDLE Volume;
    STORAGE_DEVICE_NUMBER Number;
    DWORD Done;
    GetWindowsDirectoryW(Windows, MAX_PATH);
    Path[0] = Windows[0];
    if (!QueryDosDeviceW(Path, Device, _countof(Device)))
        return FALSE;
    if (wcsstr(Device, L"Ramdisk") || wcsstr(Device, L"RamDisk"))
        return TRUE;
    StringCchPrintfW(Device, _countof(Device), L"\\\\.\\%c:", Windows[0]);
    Volume = CreateFileW(Device, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (Volume == INVALID_HANDLE_VALUE)
        return FALSE;
    Done = sizeof(Number);
    if (!DeviceIoControl(Volume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &Number, sizeof(Number), &Done, NULL))
    {
        CloseHandle(Volume);
        return FALSE;
    }
    CloseHandle(Volume);
    return Number.DeviceType == FILE_DEVICE_VIRTUAL_DISK;
}

DWORD DcEnumerate(DC_VOLUME *Volumes, DWORD *Count)
{
    WCHAR Root[DC_PATH], RunningRoot[DC_PATH], Windows[MAX_PATH], Path[DC_PATH];
    HANDLE Find, File;
    BYTE *LayoutBuffer = HeapAlloc(GetProcessHeap(), 0, 65536);
    PDRIVE_LAYOUT_INFORMATION_EX Layout = (PDRIVE_LAYOUT_INFORMATION_EX)LayoutBuffer;
    STORAGE_DEVICE_NUMBER Number;
    DWORD Done;
    DC_VOLUME *Volume;
    *Count = 0;
    if (!LayoutBuffer)
        return ERROR_NOT_ENOUGH_MEMORY;
    GetWindowsDirectoryW(Windows, MAX_PATH);
    Windows[3] = 0;
    RunningRoot[0] = 0;
    GetVolumeNameForVolumeMountPointW(Windows, RunningRoot, DC_PATH);
    Find = FindFirstVolumeW(Root, DC_PATH);
    if (Find == INVALID_HANDLE_VALUE)
    {
        HeapFree(GetProcessHeap(), 0, LayoutBuffer);
        return GetLastError();
    }
    do
    {
        if (*Count == DC_MAX_VOLUMES)
            break;
        Volume = &Volumes[*Count];
        memset(Volume, 0, sizeof(*Volume));
        Volume->Index = -1;
        StringCchCopyW(Volume->Root, DC_PATH, Root);
        StringCchCopyW(Volume->Device, DC_PATH, Root);
        Volume->Device[wcslen(Volume->Device) - 1] = 0;
        File = CreateFileW(Volume->Device, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
        if (File == INVALID_HANDLE_VALUE)
            continue;
        if (!DeviceIoControl(File, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0,
                              &Volume->Partition, sizeof(Volume->Partition), &Done, NULL) ||
            !DeviceIoControl(File, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0,
                              Layout, 65536, &Done, NULL) ||
            !DeviceIoControl(File, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0,
                              &Number, sizeof(Number), &Done, NULL) ||
            Volume->Partition.PartitionLength.QuadPart <= 0 ||
            (Layout->PartitionStyle != PARTITION_STYLE_MBR && Layout->PartitionStyle != PARTITION_STYLE_GPT))
        {
            CloseHandle(File);
            continue;
        }
        Volume->Disk = Number.DeviceNumber;
        if (Layout->PartitionStyle == PARTITION_STYLE_MBR)
            Volume->Signature = Layout->Mbr.Signature;
        else
            Volume->DiskGuid = Layout->Gpt.DiskId;
        CloseHandle(File);
        GetVolumeInformationW(Root, Volume->Label, _countof(Volume->Label), NULL, NULL, NULL, NULL, 0);
        Volume->Running = _wcsicmp(RunningRoot, Root) == 0;
        StringCchPrintfW(Path, DC_PATH, L"%sWindows\\system32\\ntoskrnl.exe", Root);
        Volume->System = GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES;
        StringCchPrintfW(Path, DC_PATH, L"%sEFI\\ReliefWin\\Boot\\BCD", Root);
        Volume->Boot = GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES;
        if (!Volume->Boot)
        {
            StringCchPrintfW(Path, DC_PATH, L"%sBoot\\BCD", Root);
            Volume->Boot = GetFileAttributesW(Path) != INVALID_FILE_ATTRIBUTES;
        }
        ++*Count;
    } while (FindNextVolumeW(Find, Root, DC_PATH));
    FindVolumeClose(Find);
    HeapFree(GetProcessHeap(), 0, LayoutBuffer);
    return ERROR_SUCCESS;
}

DWORD DcBindVolumes(DC_STORE *Store, DC_VOLUME *Volumes, DWORD Count)
{
    RW_CRYPT_STATUS *Status = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*Status));
    HANDLE Control;
    DWORD i, j, Bytes, Error;
    BYTE *Header;
    VC_META Meta;
    if (!Status)
        return ERROR_NOT_ENOUGH_MEMORY;
    Control = CreateFileW(L"\\\\.\\ReliefWinEncryption", GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    Error = Control == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS;
    if (!Error)
    {
        if (!DeviceIoControl(Control, RW_CRYPT_IOCTL_STATUS, NULL, 0, Status, sizeof(*Status), &Bytes, NULL))
            Error = GetLastError();
        else if (Bytes != sizeof(*Status) || Status->Count > RW_CRYPT_MAX_VOLUMES ||
                 Status->Version != RW_CRYPT_VERSION)
            Error = ERROR_INVALID_DATA;
        CloseHandle(Control);
    }
    if (Error)
    {
        HeapFree(GetProcessHeap(), 0, Status);
        return Error;
    }
    for (i = 0; i < Count; ++i)
    {
        DC_VOLUME *Volume = Volumes + i;
        Volume->Index = -1;
        Volume->Flags = 0;
        for (j = 0; j < Store->Count; ++j)
        {
            const RW_CRYPT_DESCRIPTOR *Descriptor = Store->Volumes + j;
            if (!RwCryptDescriptorMatch(Descriptor, Volume->Partition.PartitionStyle,
                                        Volume->Signature, Volume->Partition.StartingOffset.QuadPart,
                                        Volume->Partition.PartitionLength.QuadPart, &Volume->DiskGuid,
                                        &Volume->Partition.Gpt.PartitionId))
                continue;
            Volume->Index = j;
            Volume->System |= RwCryptDescriptorSystem(Descriptor);
            Volume->Flags = RW_CRYPT_FLAG_CONFIGURED;
            Error = DcLoadHeader(Store, Descriptor, &Header, &Meta);
            if (Error)
                Volume->Flags |= RW_CRYPT_FLAG_OFFLINE;
            else
            {
                if (Meta.Incomplete) Volume->Flags |= RW_CRYPT_FLAG_OFFLINE;
                DcOps.Free(NULL, Header, VC_METADATA_SIZE);
            }
            break;
        }
        for (j = 0; j < Status->Count; ++j)
        {
            const RW_CRYPT_DESCRIPTOR *Descriptor = &Status->Volumes[j].Descriptor;
            if (RwCryptDescriptorMatch(Descriptor, Volume->Partition.PartitionStyle,
                                       Volume->Signature, Volume->Partition.StartingOffset.QuadPart,
                                       Volume->Partition.PartitionLength.QuadPart, &Volume->DiskGuid,
                                       &Volume->Partition.Gpt.PartitionId))
            {
                if (Volume->Index < 0 || memcmp(Descriptor, Store->Volumes + Volume->Index, sizeof(*Descriptor)))
                {
                    HeapFree(GetProcessHeap(), 0, Status);
                    return ERROR_REVISION_MISMATCH;
                }
                Volume->Flags |= Status->Volumes[j].Flags;
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, Status);
    return ERROR_SUCCESS;
}
