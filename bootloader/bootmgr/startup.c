/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Startup settings chosen from the boot menu
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 */

#include <bl.h>
#include <gui.h>

#define BM_PROFILE_DEBUGGING          1
#ifdef REMOTE_ASSISTANCE
#define BM_PROFILE_REMOTE_DEBUGGING   2
#endif
#define BM_PROFILE_ISOLATED_DEBUGGING 3

static const UI_TEXT BmStartupNames[] =
{
    UiTextDebugging,
    UiTextIsolatedDebugging,
#ifdef REMOTE_ASSISTANCE
    UiTextRemoteDebugging
#endif
};

static const UI_TEXT BmStartupDescriptions[] =
{
    UiTextDebuggingDescription,
    UiTextIsolatedDescription,
#ifdef REMOTE_ASSISTANCE
    UiTextRemoteDescription
#endif
};

static const ULONG BmStartupProfiles[] =
{
    BM_PROFILE_DEBUGGING,
    BM_PROFILE_ISOLATED_DEBUGGING,
#ifdef REMOTE_ASSISTANCE
    BM_PROFILE_REMOTE_DEBUGGING
#endif
};

C_ASSERT(RTL_NUMBER_OF(BmStartupNames) == RTL_NUMBER_OF(BmStartupProfiles));
C_ASSERT(RTL_NUMBER_OF(BmStartupDescriptions) == RTL_NUMBER_OF(BmStartupProfiles));

BOOLEAN
BmChooseStartupSettings(
    _In_ BOOLEAN Graphical,
    _In_reads_(ToolCount) PCSTR* ToolNames,
    _In_ ULONG ToolCount,
    _Out_ PBM_STARTUP Startup,
    _Out_ PULONG Tool)
{
    PCSTR Names[RTL_NUMBER_OF(BmStartupProfiles) + BM_MAX_TOOLS + 1];
    PCSTR Descriptions[RTL_NUMBER_OF(BmStartupProfiles) + BM_MAX_TOOLS + 1] = { NULL };
    ULONG Choice = 0, Count = 0, i;
    BOOLEAN Chosen;

    RtlZeroMemory(Startup, sizeof(*Startup));
    *Tool = MAXULONG;

    for (i = 0; i < RTL_NUMBER_OF(BmStartupProfiles); i++)
    {
        Descriptions[Count] = UiTextInLanguage(BmStartupDescriptions[i], Graphical ? UiGetLanguage() : UiLanguageEnglish);
        Names[Count++] = UiTextInLanguage(BmStartupNames[i], Graphical ? UiGetLanguage() : UiLanguageEnglish);
    }

    for (i = 0; i < ToolCount && i < BM_MAX_TOOLS; i++)
        Names[Count++] = ToolNames[i];

    Names[Count] = UiText(UiTextBack);

    if (Graphical)
    {
        UiMenuDescriptions = Descriptions;
        Chosen = UiDisplayMenu(UiText(UiTextStartup), NULL, Names, Count + 1,
                               0, -1, &Choice, TRUE, NULL, NULL);
        UiMenuDescriptions = NULL;
        if (!Chosen)
            return FALSE;
    }
    else if (!BmUiStartupSettings(Names, Descriptions, Count, &Choice))
    {
        return FALSE;
    }

    if (Choice < RTL_NUMBER_OF(BmStartupProfiles))
    {
        Startup->Profile = BmStartupProfiles[Choice];
        return TRUE;
    }

    if (Choice >= Count)
        return FALSE;

    *Tool = Choice - RTL_NUMBER_OF(BmStartupProfiles);
    return TRUE;
}
