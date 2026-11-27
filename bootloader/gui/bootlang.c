/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Validated boot language tags and localized UTF-8 resources
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bootlang.h>
#include <string.h>

static UI_LANGUAGE CurrentLanguage = UiLanguageChinese;
static const char *const Tags[] = {"zh-CN", "en-US", "ja-JP"};
static const char *const Text[][UiLanguageCount] =
{
#define BOOT_TEXT(Id, En, Zh, Ja) {Zh, En, Ja},
#include <bootstrings.inc>
#undef BOOT_TEXT
};

const char *UiTextInLanguage(UI_TEXT Id, UI_LANGUAGE Language)
{
    if ((unsigned)Id >= UiTextCount || (unsigned)Language >= UiLanguageCount) return "";
    return Text[Id][Language];
}

const char *UiText(UI_TEXT Id)
{
    return UiTextInLanguage(Id, CurrentLanguage);
}

UI_LANGUAGE UiGetLanguage(void)
{
    return CurrentLanguage;
}

int UiSelectLanguage(UI_LANGUAGE Language)
{
    if ((unsigned)Language >= UiLanguageCount) return 0;
    CurrentLanguage = Language;
    return 1;
}

const char *UiLanguageTag(UI_LANGUAGE Language)
{
    return (unsigned)Language < UiLanguageCount ? Tags[Language] : "";
}

int UiParseLanguage(const void *Data, size_t Size, UI_LANGUAGE *Language)
{
    unsigned Index;
    if (!Data || !Language || Size != 5) return 0;
    for (Index = 0; Index < UiLanguageCount; ++Index)
        if (!memcmp(Data, Tags[Index], Size))
        {
            *Language = (UI_LANGUAGE)Index;
            return 1;
        }
    return 0;
}
