/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Read and persist files on the BIOS boot volume
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bl.h>
#include <ntfsbl.h>

static BOOLEAN
BiosFilePath(PCWSTR Path, PCHAR Name)
{
    ULONG i;

    if (!Path)
        return FALSE;
    while (*Path == L'\\' || *Path == L'/')
        Path++;
    for (i = 0; Path[i]; i++)
    {
        if (i >= BM_PATH_CHARS - 1 || Path[i] > 0x7f)
            return FALSE;
        Name[i] = (CHAR)Path[i];
    }
    Name[i] = 0;
    return i != 0;
}

PVOID
FwReadBootFile(
    _In_ PCWSTR Path,
    _Out_ PULONG Size)
{
    DK_DEVICE Device;
    CHAR Name[BM_PATH_CHARS];
    PVOID File = NULL, Data = NULL;
    ULONGLONG Length;
    ULONG Read;
    BOOLEAN Directory;
    NTSTATUS Status;

    *Size = 0;
    if (!BiosFilePath(Path, Name) || !FwGetBootDevice(&Device))
        return NULL;
    if (!NT_SUCCESS(NtfsBlMount(&Device)))
        return FsReadFile(&Device, Path, Size);

    Status = NtfsBlOpenFile(Name, &File, &Length, &Directory);
    if (NT_SUCCESS(Status) && !Directory && Length && Length <= MAXULONG)
    {
        Data = FwAllocatePages((SIZE_T)Length);
        if (Data)
        {
            Status = NtfsBlReadFileAt(File, 0, Data, (ULONG)Length, &Read);
            if (!NT_SUCCESS(Status) || Read != Length)
            {
                FwFreePages(Data, (SIZE_T)Length);
                Data = NULL;
            }
        }
    }
    NtfsBlCloseFile(File);
    Status = NtfsBlUnmount();
    if (Data && !NT_SUCCESS(Status))
    {
        FwFreePages(Data, (SIZE_T)Length);
        Data = NULL;
    }
    if (Data)
        *Size = (ULONG)Length;
    return Data;
}

BOOLEAN
FwWriteBootFile(
    _In_ PCWSTR Path,
    _In_reads_bytes_(Size) const VOID* Data,
    _In_ ULONG Size)
{
    DK_DEVICE Device;
    CHAR Name[BM_PATH_CHARS];
    NTSTATUS Status, UnmountStatus;

    if ((!Data && Size) || !BiosFilePath(Path, Name) || !FwGetBootDevice(&Device))
        return FALSE;
    if (FsWriteFatFile(&Device, Name, Data, Size))
        return TRUE;

    Status = NtfsBlMount(&Device);
    if (!NT_SUCCESS(Status))
        return FALSE;
    Status = NtfsBlWriteFile(Name, Data, Size);
    UnmountStatus = NtfsBlUnmount();
    return NT_SUCCESS(Status) && NT_SUCCESS(UnmountStatus);
}
