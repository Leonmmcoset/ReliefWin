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
    _In_ ULONG Dpi,
    _Out_ PRELIEF_BOOT_LOGO_RECT Rect)
{
    ULONG Width, Height, MaxWidth, SafeBottom, Top;

    if (!Rect || !ScreenWidth || !ScreenHeight || !SourceWidth || !SourceHeight)
        return FALSE;

    if (Dpi < LOADER_PARAMETER_FRAMEBUFFER_DPI_MIN || Dpi > LOADER_PARAMETER_FRAMEBUFFER_DPI_MAX)
        Dpi = LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT;

    Width = (ULONG)(((ULONGLONG)SourceWidth * 2 + 2) / 5);
    Height = (ULONG)(((ULONGLONG)SourceHeight * 2 + 2) / 5);
    Width = max((ULONG)(((ULONGLONG)Width * Dpi + LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT / 2) /
                        LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT), 1UL);
    Height = max((ULONG)(((ULONGLONG)Height * Dpi + LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT / 2) /
                         LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT), 1UL);
    MaxWidth = max((ULONG)(((ULONGLONG)ScreenWidth * 9) / 10), 1UL);
    SafeBottom = max((ULONG)(((ULONGLONG)ScreenHeight * 35) / 100), 1UL);
    if (Width > MaxWidth)
    {
        Height = max((ULONG)(((ULONGLONG)Height * MaxWidth) / Width), 1UL);
        Width = MaxWidth;
    }
    if (Height > SafeBottom)
    {
        Width = max((ULONG)(((ULONGLONG)Width * SafeBottom) / Height), 1UL);
        Height = SafeBottom;
    }

    Top = (ULONG)(((ULONGLONG)ScreenHeight * 8 + 50) / 100);
    if (Top + Height > SafeBottom)
        Top = SafeBottom - Height;
    Rect->X = (ScreenWidth - Width) / 2;
    Rect->Y = Top;
    Rect->Width = Width;
    Rect->Height = Height;
    return TRUE;
}
