/*
 * PROJECT:     ReliefWin Boot Graphics
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Shared loader and kernel boot-logo layout
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

typedef struct _RELIEF_BOOT_LOGO_RECT
{
    ULONG X;
    ULONG Y;
    ULONG Width;
    ULONG Height;
} RELIEF_BOOT_LOGO_RECT, *PRELIEF_BOOT_LOGO_RECT;

static __inline
BOOLEAN
ReliefBootLogoGetRect(
    _In_ ULONG ScreenWidth,
    _In_ ULONG ScreenHeight,
    _In_ ULONG SourceWidth,
    _In_ ULONG SourceHeight,
    _Out_ PRELIEF_BOOT_LOGO_RECT Rect)
{
    ULONG Width, Height, MaxWidth;

    if (!Rect || !ScreenWidth || !ScreenHeight || !SourceWidth || !SourceHeight)
        return FALSE;

    Height = max((ULONG)(((ULONGLONG)ScreenHeight + 2) / 4), 1UL);
    Width = max((ULONG)(((ULONGLONG)SourceWidth * Height + SourceHeight / 2) / SourceHeight), 1UL);
    MaxWidth = max((ULONG)(((ULONGLONG)ScreenWidth * 9) / 10), 1UL);
    if (Width > MaxWidth)
    {
        Width = MaxWidth;
        Height = max((ULONG)(((ULONGLONG)SourceHeight * Width + SourceWidth / 2) / SourceWidth), 1UL);
    }

    Rect->X = (ScreenWidth - Width) / 2;
    Rect->Y = (ScreenHeight - Height) / 2;
    Rect->Width = Width;
    Rect->Height = Height;
    return TRUE;
}
