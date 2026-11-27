/*
 * PROJECT:     ReliefWin API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Boot translations, bounded Unicode conversion and saved preferences
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#include <bootlang.h>
#include "../utf8.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned Checks, Failures;
#define CHECK(C) do {++Checks; if (!(C)) {++Failures; printf("FAIL %d: %s\n", __LINE__, #C);}} while (0)

static void Formats(const char *Text, char *Result)
{
    while (*Text)
        if (*Text++ == '%')
        {
            while (*Text == 'l') *Result++ = *Text++;
            *Result++ = *Text++;
        }
    *Result = 0;
}

static void TestResources(void)
{
    unsigned Id, Language;
    size_t Length, Offset, Used;
    uint32_t Character;
    char Format[64], Reference[64];
    for (Id = 0; Id < UiTextCount; ++Id)
    {
        Formats(UiTextInLanguage(Id, UiLanguageEnglish), Reference);
        for (Language = 0; Language < UiLanguageCount; ++Language)
        {
            const char *Text = UiTextInLanguage(Id, Language);
            CHECK(Text[0]);
            Formats(Text, Format);
            CHECK(!strcmp(Format, Reference));
            Length = strlen(Text);
            for (Offset = 0; Offset < Length; Offset += Used)
            {
                Character = UiUtf8Next(Text + Offset, Length - Offset, &Used);
                CHECK(Used && Used <= Length - Offset && Character != 0xfffd);
            }
        }
    }
    CHECK(!UiTextInLanguage(UiTextCount, UiLanguageEnglish)[0]);
    CHECK(!UiTextInLanguage(UiTextSettings, UiLanguageCount)[0]);
}

static void TestUnicode(void)
{
    static const uint16_t Name[] = {0x4e2d, 0x65e5, 0xd83d, 0xde00, 0};
    static const uint16_t Invalid[] = {0xd800, 'A', 0xdc00, 0};
    char Buffer[32];
    size_t Used, Capacity, Length;
    CHECK(UiUtf8Next("中", 3, &Used) == 0x4e2d && Used == 3);
    CHECK(UiUtf8Next("\xf0\x9f\x98\x80", 4, &Used) == 0x1f600 && Used == 4);
    CHECK(UiUtf8Next("\xe0\x80\x80", 3, &Used) == 0xfffd && Used == 1);
    CHECK(UiUtf8Next("\xed\xa0\x80", 3, &Used) == 0xfffd && Used == 1);
    CHECK(UiUtf8Next("\xf4\x90\x80\x80", 4, &Used) == 0xfffd && Used == 1);
    CHECK(UiUtf8Next("\x80", 1, &Used) == 0xfffd && Used == 1);
    CHECK(UiUtf8Next("\xe4\xb8", 2, &Used) == 0xfffd && Used == 1);
    CHECK(UiUtf8Next("\xe4", 1, &Used) == 0xfffd && Used == 1);
    CHECK(!UiUtf8Next("", 1, &Used) && !Used);
    CHECK(!UiUtf8Next(NULL, 0, &Used) && !Used);
    CHECK(UiUtf16ToUtf8(Name, Buffer, sizeof(Buffer)) == 10);
    CHECK(!strcmp(Buffer, "中日\xf0\x9f\x98\x80"));
    CHECK(UiUtf16ToUtf8(Invalid, Buffer, sizeof(Buffer)) == 7);
    CHECK(!strcmp(Buffer, "\xef\xbf\xbd" "A" "\xef\xbf\xbd"));
    for (Capacity = 0; Capacity <= 12; ++Capacity)
    {
        memset(Buffer, 'x', sizeof(Buffer));
        Length = UiUtf16ToUtf8(Name, Buffer, Capacity);
        CHECK(Buffer[Capacity] == 'x');
        CHECK(!Capacity ? !Length : !Buffer[Length] && Length < Capacity);
        CHECK(!Length || Length == 3 || Length == 6 || Length == 10);
    }
}

int main(void)
{
    unsigned Index;
    UI_LANGUAGE Parsed = UiLanguageJapanese;
    CHECK(UiGetLanguage() == UiLanguageChinese);
    CHECK(!UiSelectLanguage(UiLanguageCount) && UiGetLanguage() == UiLanguageChinese);
    CHECK(!UiParseLanguage(NULL, 5, &Parsed));
    CHECK(!UiParseLanguage("ja-JP", 4, &Parsed));
    CHECK(!UiParseLanguage("ja-JP\n", 6, &Parsed));
    CHECK(!UiParseLanguage("xx-XX", 5, &Parsed));
    CHECK(!UiParseLanguage("ja-JP", 5, NULL));
    for (Index = 0; Index < UiLanguageCount; ++Index)
    {
        CHECK(UiParseLanguage(UiLanguageTag(Index), 5, &Parsed) && Parsed == Index);
        CHECK(UiSelectLanguage(Parsed) && UiGetLanguage() == Parsed);
        CHECK(!strcmp(UiText(UiTextSettings), UiTextInLanguage(UiTextSettings, Parsed)));
    }
    TestResources();
    TestUnicode();
    printf("%u boot language checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
