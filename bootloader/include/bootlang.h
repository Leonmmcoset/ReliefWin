/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Boot menu language selection and resource access
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

#include <stddef.h>

typedef enum UI_LANGUAGE
{
    UiLanguageChinese,
    UiLanguageEnglish,
    UiLanguageJapanese,
    UiLanguageCount
} UI_LANGUAGE;

typedef enum UI_TEXT
{
#define BOOT_TEXT(Id, En, Zh, Ja) UiText##Id,
#include "bootstrings.inc"
#undef BOOT_TEXT
    UiTextCount
} UI_TEXT;

const char *UiText(UI_TEXT Id);
const char *UiTextInLanguage(UI_TEXT Id, UI_LANGUAGE Language);
UI_LANGUAGE UiGetLanguage(void);
int UiSelectLanguage(UI_LANGUAGE Language);
const char *UiLanguageTag(UI_LANGUAGE Language);
int UiParseLanguage(const void *Data, size_t Size, UI_LANGUAGE *Language);
void UiLoadLanguage(void);
int UiSaveLanguage(UI_LANGUAGE Language);
