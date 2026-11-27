/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Persist boot language on the firmware boot volume
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bl.h>
#include <bootlang.h>

static BOOLEAN UiLanguageLoaded;

void UiLoadLanguage(void)
{
    ULONG Size;
    PVOID Data;
    UI_LANGUAGE Language;
    if (UiLanguageLoaded) return;
    UiLanguageLoaded = TRUE;
    Data = FwReadBootFile(L"\\ReliefWin.bootlang", &Size);
    if (!Data) return;
    if (UiParseLanguage(Data, Size, &Language)) UiSelectLanguage(Language);
    FwFreePages(Data, Size);
}

int UiSaveLanguage(UI_LANGUAGE Language)
{
    const char *Tag;
    PVOID Data;
    ULONG Size;
    UI_LANGUAGE Saved;
    int Result;
    if (!UiSelectLanguage(Language)) return FALSE;
    Tag = UiLanguageTag(Language);
    if (!FwWriteBootFile(L"\\ReliefWin.bootlang", Tag, 5)) return FALSE;
    Data = FwReadBootFile(L"\\ReliefWin.bootlang", &Size);
    if (!Data) return FALSE;
    Result = UiParseLanguage(Data, Size, &Saved) && Saved == Language;
    FwFreePages(Data, Size);
    return Result;
}
