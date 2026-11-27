/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Pre-boot password editing, bounds, mode changes and recovery-key validation
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include "../cryptinput.h"
#include <stdio.h>

static unsigned Checks, Failures;
#define CHECK(C) do { ++Checks; if (!(C)) {++Failures; printf("FAIL %u: %s\n", (unsigned)__LINE__, #C);} } while (0)

int main(void)
{
    struct {char Before; char Secret[129]; char After;} Guard = {'x', {0}, 'y'};
    BM_CRYPT_EDIT Edit = {0};
    BM_CRYPT_POINTER Pointer = {0};
    unsigned i;
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptInvalid);
    for (i = 0; i < 140; ++i) BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 'A', 0);
    CHECK(Edit.Length == 128 && Edit.Cursor == 128 && Guard.Secret[128] == 0);
    CHECK(Guard.Before == 'x' && Guard.After == 'y');
    CHECK(!BmCryptPointer(&Edit, &Pointer, 1, 2, 1) && Edit.Focus == 2);
    CHECK(!BmCryptPointer(&Edit, &Pointer, 0, 0, 0) && Pointer.Down == 1);
    CHECK(BmCryptPointer(&Edit, &Pointer, 1, 2, 0) && Edit.Focus == 2);
    CHECK(!BmCryptPointer(&Edit, &Pointer, 1, 2, 0));
    BmCryptPointer(&Edit, &Pointer, 1, 1, 1);
    CHECK(!BmCryptPointer(&Edit, &Pointer, 1, 3, 0));
    BmCryptPointer(&Edit, &Pointer, 1, 9, 1);
    CHECK(!BmCryptPointer(&Edit, &Pointer, 1, 1, 0));
    BmCryptPointer(&Edit, &Pointer, 1, 0, 1);
    CHECK(!BmCryptPointer(&Edit, &Pointer, 1, 0, 0) && !Edit.Focus);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptSubmit);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x47, 1);
    CHECK(Edit.Cursor == 0);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x53, 1);
    CHECK(Edit.Length == 127 && Edit.Cursor == 0);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 'B', 0);
    CHECK(Edit.Length == 128 && Edit.Cursor == 1 && Guard.Secret[0] == 'B' && Guard.Secret[1] == 'A');
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\b', 0);
    CHECK(Edit.Length == 127 && Edit.Cursor == 0 && Guard.Secret[0] == 'A');
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x4f, 1);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x4b, 1);
    CHECK(Edit.Cursor == 126);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x4d, 1);
    CHECK(Edit.Cursor == 127);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x15, 0);
    CHECK(Edit.Length == 0 && Edit.Cursor == 0);
    for (i = 0; i < sizeof(Guard.Secret); ++i) CHECK(Guard.Secret[i] == 0);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\t', 0);
    CHECK(Edit.Focus == 1);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\t', 0);
    CHECK(Edit.Focus == 2);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptContinue);
    CHECK(Edit.Recovery && !Edit.Length && !Edit.Focus);
    for (i = 0; i < 70; ++i) BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 'F', 0);
    CHECK(Edit.Length == 64 && Guard.Secret[64] == 0);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptSubmit);
    CHECK(Guard.Secret[0] == 'f' && Guard.Secret[63] == 'f');
    Guard.Secret[20] = 'g';
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptInvalid);
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\b', 0);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptInvalid);
    Edit.Focus = 2;
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0);
    CHECK(!Edit.Recovery && !Edit.Length && !Edit.Focus);
    for (i = 0; i < 12; ++i) BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), ' ', 0);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptSubmit);
    Edit.Focus = 4;
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptLanguage);
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x3c, 1) == BmCryptLanguage);
    Edit.Focus = 3;
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), '\r', 0) == BmCryptCancel);
    Edit.Focus = 0;
    CHECK(BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x1b, 0) == BmCryptCancel);
    BmCryptClear(&Edit, Guard.Secret, sizeof(Guard.Secret));
    BmCryptKey(&Edit, Guard.Secret, sizeof(Guard.Secret), 0x80, 0);
    CHECK(!Edit.Length);
    CHECK(BmCryptKey(&Edit, Guard.Secret, 0, 'A', 0) == BmCryptInvalid);
    CHECK(Guard.Before == 'x' && Guard.After == 'y');
    printf("%u pre-boot input checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
