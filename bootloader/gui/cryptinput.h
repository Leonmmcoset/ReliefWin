/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Bounded password editing for the graphical pre-boot unlock screen
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stddef.h>
#include <string.h>

typedef struct BM_CRYPT_EDIT
{
    unsigned Length;
    unsigned Cursor;
    unsigned Focus;
    unsigned Recovery;
} BM_CRYPT_EDIT;

enum { BmCryptContinue, BmCryptSubmit, BmCryptCancel, BmCryptLanguage, BmCryptInvalid };

typedef struct BM_CRYPT_POINTER
{
    unsigned Pressed;
    unsigned Down;
} BM_CRYPT_POINTER;

static int BmCryptPointer(BM_CRYPT_EDIT *Edit, BM_CRYPT_POINTER *Pointer,
                         int Valid, unsigned Hit, unsigned Down)
{
    int Activate = 0;
    if (!Valid) return 0;
    if (Down && !Pointer->Down)
    {
        Pointer->Pressed = Hit;
        if (Hit <= 4) Edit->Focus = Hit;
    }
    if (!Down && Pointer->Down && Hit == Pointer->Pressed && Hit > 0 && Hit <= 4)
    {
        Edit->Focus = Hit;
        Activate = 1;
    }
    Pointer->Down = Down;
    return Activate;
}

static void BmCryptClear(BM_CRYPT_EDIT *Edit, char *Secret, unsigned Capacity)
{
    volatile char *Bytes = Secret;
    unsigned i;
    for (i = 0; i < Capacity; ++i) Bytes[i] = 0;
    Edit->Length = Edit->Cursor = 0;
}

static int BmCryptKey(BM_CRYPT_EDIT *Edit, char *Secret, unsigned Capacity, int Key, int Extended)
{
    unsigned i;
    if (!Capacity || Edit->Length >= Capacity || Edit->Cursor > Edit->Length ||
        Edit->Length > 128 || Edit->Focus > 4)
        return BmCryptInvalid;
    if (!Extended && Key == 0x1b) return BmCryptCancel;
    if (Extended && Key == 0x3c) return BmCryptLanguage;
    if (!Extended && Key == '\t')
    {
        Edit->Focus = (Edit->Focus + 1) % 5;
        return BmCryptContinue;
    }
    if (!Extended && (Key == '\r' || (Key == ' ' && Edit->Focus)))
    {
        if (Edit->Focus == 2)
        {
            BmCryptClear(Edit, Secret, Capacity);
            Edit->Recovery = !Edit->Recovery;
            Edit->Focus = 0;
            return BmCryptContinue;
        }
        if (Edit->Focus == 3) return BmCryptCancel;
        if (Edit->Focus == 4) return BmCryptLanguage;
        if (Edit->Recovery)
        {
            if (Edit->Length != 64) return BmCryptInvalid;
            for (i = 0; i < Edit->Length; ++i)
            {
                if (Secret[i] >= 'A' && Secret[i] <= 'F') Secret[i] += 'a' - 'A';
                if (!((Secret[i] >= '0' && Secret[i] <= '9') || (Secret[i] >= 'a' && Secret[i] <= 'f')))
                    return BmCryptInvalid;
            }
        }
        else if (Edit->Length < 12) return BmCryptInvalid;
        return BmCryptSubmit;
    }
    if (Edit->Focus) return BmCryptContinue;
    if (Extended)
    {
        if (Key == 0x4b && Edit->Cursor) --Edit->Cursor;
        else if (Key == 0x4d && Edit->Cursor < Edit->Length) ++Edit->Cursor;
        else if (Key == 0x47) Edit->Cursor = 0;
        else if (Key == 0x4f) Edit->Cursor = Edit->Length;
        else if (Key == 0x53 && Edit->Cursor < Edit->Length)
        {
            memmove(Secret + Edit->Cursor, Secret + Edit->Cursor + 1, Edit->Length - Edit->Cursor);
            Secret[--Edit->Length] = 0;
        }
    }
    else if (Key == '\b' && Edit->Cursor)
    {
        --Edit->Cursor;
        memmove(Secret + Edit->Cursor, Secret + Edit->Cursor + 1, Edit->Length - Edit->Cursor);
        Secret[--Edit->Length] = 0;
    }
    else if (Key == 0x15) BmCryptClear(Edit, Secret, Capacity);
    else if (Key >= 0x20 && Key <= 0x7e && Edit->Length < (Edit->Recovery ? 64 : 128) &&
             Edit->Length + 1 < Capacity)
    {
        memmove(Secret + Edit->Cursor + 1, Secret + Edit->Cursor, Edit->Length - Edit->Cursor + 1);
        Secret[Edit->Cursor++] = (char)Key;
        ++Edit->Length;
    }
    return BmCryptContinue;
}
