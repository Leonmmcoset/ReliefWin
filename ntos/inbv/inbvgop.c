/*
 * PROJECT:     ReliefWin Kernel
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     UEFI GOP-specific boot UI helpers
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF (arif.ing@outlook.com)
 */

#include <ntoskrnl.h>
#define NDEBUG
#include <debug.h>
#include <drivers/bootvid/bootvid.h>
#include <reliefwin/bootlogo.h>
#include "logo.h"
#include "resource.h"
#include "inbvgop.h"


#ifndef BI_RGB
#define BI_RGB 0
#endif
#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif
#define INBV_WORDMARK_TOP_VPOS      0.08f

#define INBV_LOGO_POOL_TAG          'oLbI'
#define INBV_BOOT_PROGRESS_MAX      10000
#define INBV_BOOT_PROGRESS_START    5000

typedef RELIEF_BOOT_LOGO_RECT INBV_GOP_RECT, *PINBV_GOP_RECT;

typedef struct _INBV_STATUS_LINE
{
    ULONG Top;
    ULONG Height;
    ULONG Width;
    ULONG CharWidth;
    ULONG PixelTop;
    ULONG PixelHeight;
    ULONG PixelWidth;
    PUCHAR Coverage;
    PULONG Row;
    ULONG Lut[256];
    BOOLEAN Ready;
} INBV_STATUS_LINE;

static INBV_STATUS_LINE g_Status;
static ULONG InbvBootProgressValue = INBV_BOOT_PROGRESS_START;

static VOID NTAPI InbvGopDisplayFilter(_Inout_ PCSTR *String);
static VOID InbvDisplayStatusLocked(_In_ PCSTR String);

static VOID
InbvGopFillStatusBand(VOID)
{
    VidSolidColorFill(0, g_Status.Top, g_Status.Width - 1, g_Status.Top + g_Status.Height - 1, BV_COLOR_BLACK);
}

static VOID
InbvGopStatusFree(
    _In_opt_ PUCHAR Coverage,
    _In_opt_ PULONG Row)
{
    if (Coverage)
        ExFreePoolWithTag(Coverage, INBV_LOGO_POOL_TAG);
    if (Row)
        ExFreePoolWithTag(Row, INBV_LOGO_POOL_TAG);
}

extern VOID NTAPI InbvAcquireLock(VOID);
extern VOID NTAPI InbvReleaseLock(VOID);

CODE_SEG("INIT")
static
BOOLEAN
InbvGopQueryInfo(
    _Out_ LOADER_PARAMETER_FRAMEBUFFER *FbInfo)
{
    if (!FbInfo)
        return FALSE;

    RtlZeroMemory(FbInfo, sizeof(*FbInfo));

    if (!InbvGetGopFrameBufferInfo(FbInfo))
        return FALSE;

    if (FbInfo->FrameBufferSize == 0 ||
        FbInfo->HorizontalResolution == 0 ||
        FbInfo->VerticalResolution == 0)
    {
        return FALSE;
    }

    return TRUE;
}

static ULONG InbvMaskShift(ULONG Mask)
{
    ULONG Shift = 0;
    if (!Mask) return 0;
    while ((Mask & 1) == 0) { Shift++; Mask >>= 1; }
    return Shift;
}

static ULONG InbvMaskMax(ULONG Mask)
{
    ULONG Value = 0;
    while (Mask) { Value = (Value << 1) | 1; Mask >>= 1; }
    return Value;
}

static ULONG InbvPackColor(ULONG RedMask, ULONG GreenMask, ULONG BlueMask,
                           ULONG RedShift, ULONG GreenShift, ULONG BlueShift,
                           ULONG RedMax, ULONG GreenMax, ULONG BlueMax,
                           UCHAR r, UCHAR g, UCHAR b)
{
    ULONG R = RedMask   ? ((ULONG)r * RedMax   + 127) / 255 : 0;
    ULONG G = GreenMask ? ((ULONG)g * GreenMax + 127) / 255 : 0;
    ULONG B = BlueMask  ? ((ULONG)b * BlueMax  + 127) / 255 : 0;
    return (RedMask   ? ((R << RedShift)   & RedMask)   : 0) |
           (GreenMask ? ((G << GreenShift) & GreenMask) : 0) |
           (BlueMask  ? ((B << BlueShift)  & BlueMask)  : 0);
}

static BOOLEAN
InbvGopValidateWordmarkHeader(_In_ const BITMAPINFOHEADER *Header)
{
    if (!Header ||
        Header->biSize < sizeof(BITMAPINFOHEADER) ||
        Header->biPlanes != 1 ||
        Header->biWidth <= 0 ||
        Header->biHeight == 0 ||
        Header->biHeight == (LONG)MINLONG)
    {
        return FALSE;
    }

    if (Header->biBitCount == 4)
        return Header->biCompression == BI_RGB;

    if (Header->biBitCount == 32)
    {
        return Header->biCompression == BI_RGB || Header->biCompression == BI_BITFIELDS;
    }

    return FALSE;
}

static
BOOLEAN
InbvGopComputeWordmarkRect(
    _In_ ULONG ScreenWidth,
    _In_ ULONG ScreenHeight,
    _In_ ULONG Dpi,
    _Out_ PINBV_GOP_RECT Rect)
{
    PBITMAPINFOHEADER Header;
    ULONG SrcWidth, SrcHeight;

    if (!Rect)
        return FALSE;

    Header = (PBITMAPINFOHEADER)InbvGetResourceAddress(IDB_GOP_LOGO);
    if (!InbvGopValidateWordmarkHeader(Header))
        return FALSE;

    SrcWidth = (ULONG)Header->biWidth;
    SrcHeight = (Header->biHeight < 0) ? (ULONG)(-Header->biHeight) : (ULONG)Header->biHeight;
    if (!SrcWidth || !SrcHeight)
        return FALSE;

    return ReliefBootLogoGetRect(ScreenWidth, ScreenHeight, SrcWidth, SrcHeight, Dpi, Rect);
}

static VOID
InbvDrawBootProgressLocked(VOID)
{
    ULONG Height, Filled;

    ASSERT(g_Status.Ready);
    ASSERT(g_Status.Width != 0);
    ASSERT(InbvBootProgressValue <= INBV_BOOT_PROGRESS_MAX);
    Height = max(1UL, g_Status.Height / 4);
    Filled = (ULONG)(((ULONGLONG)g_Status.Width * InbvBootProgressValue) /
                     INBV_BOOT_PROGRESS_MAX);
    VidSolidColorFill(0, 0, g_Status.Width - 1, Height - 1, BV_COLOR_DARK_GRAY);
    if (Filled)
        VidSolidColorFill(0, 0, Filled - 1, Height - 1, BV_COLOR_WHITE);
}

CODE_SEG("INIT")
VOID
NTAPI
InbvInitializeBootStatus(VOID)
{
    LOADER_PARAMETER_FRAMEBUFFER FbInfo;
    VID_DISPLAY_INFO DisplayInfo;
    ULONG RedShift, GreenShift, BlueShift;
    ULONG RedMax, GreenMax, BlueMax;
    ULONG i, Root, Center;

    if (g_Status.Ready)
        return;

    if (!InbvQueryDisplayInfo(&DisplayInfo) || !DisplayInfo.Width ||
        !DisplayInfo.CharacterWidth || !DisplayInfo.CharacterHeight ||
        DisplayInfo.CharacterHeight >= DisplayInfo.Height)
        return;

    RtlZeroMemory(&g_Status, sizeof(g_Status));
    g_Status.Width = DisplayInfo.Width;
    g_Status.CharWidth = DisplayInfo.CharacterWidth;
    g_Status.Height = DisplayInfo.CharacterHeight;
    Center = (ULONG)(((ULONGLONG)DisplayInfo.Height * 13) / 16);
    g_Status.Top = (Center > 20 + g_Status.Height / 2) ?
                   Center - 20 - g_Status.Height / 2 : 0;
    g_Status.Top = min(g_Status.Top, DisplayInfo.Height - g_Status.Height);

    if (InbvGopQueryInfo(&FbInfo) && DisplayInfo.PhysicalWidth &&
        DisplayInfo.PhysicalHeight)
    {
        g_Status.PixelTop = (ULONG)(((ULONGLONG)g_Status.Top * DisplayInfo.PhysicalHeight) /
                                    DisplayInfo.Height);
        g_Status.PixelHeight = (ULONG)(((ULONGLONG)g_Status.Height * DisplayInfo.PhysicalHeight) /
                                       DisplayInfo.Height);
        g_Status.PixelWidth = DisplayInfo.PhysicalWidth;
        if (g_Status.PixelHeight &&
            g_Status.PixelWidth <= MAXULONG / sizeof(ULONG) &&
            g_Status.PixelWidth <= MAXULONG_PTR / g_Status.PixelHeight)
        {
            g_Status.Coverage = ExAllocatePoolWithTag(NonPagedPool,
                                                      (SIZE_T)g_Status.PixelWidth * g_Status.PixelHeight,
                                                      INBV_LOGO_POOL_TAG);
            g_Status.Row = ExAllocatePoolWithTag(NonPagedPool,
                                                 (SIZE_T)g_Status.PixelWidth * sizeof(ULONG),
                                                 INBV_LOGO_POOL_TAG);
            if (!g_Status.Coverage || !g_Status.Row)
            {
                InbvGopStatusFree(g_Status.Coverage, g_Status.Row);
                g_Status.Coverage = NULL;
                g_Status.Row = NULL;
            }
        }

        RedShift = InbvMaskShift(FbInfo.RedMask);
        GreenShift = InbvMaskShift(FbInfo.GreenMask);
        BlueShift = InbvMaskShift(FbInfo.BlueMask);
        RedMax = InbvMaskMax(FbInfo.RedMask >> RedShift);
        GreenMax = InbvMaskMax(FbInfo.GreenMask >> GreenShift);
        BlueMax = InbvMaskMax(FbInfo.BlueMask >> BlueShift);
        for (i = 0; i < RTL_NUMBER_OF(g_Status.Lut); ++i)
        {
            for (Root = 0; (Root + 1) * (Root + 1) <= i * 255; Root++);
            if (i * 255 - Root * Root > Root)
                Root++;
            g_Status.Lut[i] = InbvPackColor(FbInfo.RedMask, FbInfo.GreenMask, FbInfo.BlueMask,
                                            RedShift, GreenShift, BlueShift,
                                            RedMax, GreenMax, BlueMax,
                                            (UCHAR)Root, (UCHAR)Root, (UCHAR)Root);
        }
    }

    InbvAcquireLock();
    g_Status.Ready = TRUE;
    InbvBootProgressValue = INBV_BOOT_PROGRESS_START;
    if (InbvIsBootDriverInstalled() && InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
        InbvDrawBootProgressLocked();
    InbvReleaseLock();
    InbvInstallDisplayStringFilter(InbvGopDisplayFilter);
    InbvBootProgress("ntoskrnl.exe", INBV_BOOT_PROGRESS_START);
}

VOID
NTAPI
InbvGopLogoComplete(VOID)
{
    InbvBootProgress("smss.exe", INBV_BOOT_PROGRESS_MAX);
}

VOID
NTAPI
InbvGopLogoStop(VOID)
{
    InbvGopClearStatus();
}

static ULONG
InbvFormatBootComponent(
    _In_ PCSTR Component,
    _In_ ULONG MaxChars,
    _Out_writes_(128) PCHAR Line)
{
    ULONG Length = 0;

    MaxChars = min(MaxChars, 127UL);
    Line[0] = ANSI_NULL;
    if (!Component || !Component[0] || MaxChars < 10)
        return 0;
    RtlCopyMemory(Line, "Loading ", 8);
    while (Length < MaxChars - 9 && Component[Length])
    {
        Line[8 + Length] = Component[Length];
        Length++;
    }
    Line[8 + Length] = (CHAR)0x85;
    Line[9 + Length] = ANSI_NULL;
    return Length + 9;
}

VOID
NTAPI
InbvBootProgress(
    _In_opt_ PCSTR Component,
    _In_ ULONG Progress)
{
    CHAR Line[128];
    ULONG MaxChars;

    if (!g_Status.Ready)
        return;

    InbvAcquireLock();
    if (g_Status.Ready && InbvIsBootDriverInstalled() &&
        InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
    {
        if (Progress != MAXULONG)
            InbvBootProgressValue = max(InbvBootProgressValue, min(Progress, INBV_BOOT_PROGRESS_MAX));
        InbvDrawBootProgressLocked();
        MaxChars = g_Status.Width / g_Status.CharWidth;
        MaxChars = MaxChars > 2 ? MaxChars - 2 : 0;
        if (InbvFormatBootComponent(Component, MaxChars, Line))
            InbvDisplayStatusLocked(Line);
    }
    InbvReleaseLock();
}

VOID
NTAPI
InbvBootDriver(
    _In_ PCUNICODE_STRING Name)
{
    CHAR Component[113];

    if (!g_Status.Ready || !Name || !Name->Buffer || !Name->Length)
        return;
    RtlStringCbPrintfA(Component, sizeof(Component), "%wZ", Name);
    InbvBootProgress(Component, MAXULONG);
}

static VOID
InbvDisplayStatusLocked(
    _In_ PCSTR String)
{
    CHAR Line[128];
    SIZE_T Start, End, Length, MaxChars;
    ULONG Left, x, y;

    ASSERT(g_Status.Ready);
    ASSERT(g_Status.CharWidth != 0);

    End = strlen(String);
    while (End != 0 && (String[End - 1] == '\r' || String[End - 1] == '\n' || String[End - 1] == ' '))
        End--;
    Start = End;
    while (Start != 0 && String[Start - 1] != '\r' && String[Start - 1] != '\n')
        Start--;
    while (Start < End && String[Start] == ' ')
        Start++;

    MaxChars = g_Status.Width / g_Status.CharWidth;
    MaxChars = (MaxChars > 2) ? (MaxChars - 2) : 0;
    if (MaxChars >= sizeof(Line))
        MaxChars = sizeof(Line) - 1;
    Length = End - Start;
    if (Length > MaxChars)
        Length = MaxChars;
    if (Length == 0)
        return;
    RtlCopyMemory(Line, String + Start, Length);
    Line[Length] = '\0';

    if (g_Status.Coverage &&
        VidRenderStatusText(Line, g_Status.Coverage, g_Status.PixelWidth, g_Status.PixelHeight))
    {
        for (y = 0; y < g_Status.PixelHeight; y++)
        {
            const UCHAR *Source = g_Status.Coverage + (SIZE_T)y * g_Status.PixelWidth;

            for (x = 0; x < g_Status.PixelWidth; x++)
                g_Status.Row[x] = g_Status.Lut[Source[x]];
            VidBufferToScreenBlt((PUCHAR)g_Status.Row,
                                 0,
                                 g_Status.PixelTop + y,
                                 g_Status.PixelWidth,
                                 1,
                                 g_Status.PixelWidth * sizeof(ULONG));
        }
    }
    else
    {
        InbvGopFillStatusBand();
        Left = (g_Status.Width - (ULONG)Length * g_Status.CharWidth + g_Status.CharWidth) /
               (2 * g_Status.CharWidth) * g_Status.CharWidth;
        VidSetScrollRegion(Left,
                           g_Status.Top,
                           min(Left + ((ULONG)Length + 1) * g_Status.CharWidth, g_Status.Width) - 1,
                           g_Status.Top + g_Status.Height - 1);
        for (x = 0; x < Length; x++)
        {
            if ((UCHAR)Line[x] == 0x85)
                Line[x] = '.';
        }
        VidDisplayString(Line);
    }
}

BOOLEAN
NTAPI
InbvGopDisplayStatus(
    _In_ PCSTR String)
{
    BOOLEAN Displayed = FALSE;

    InbvAcquireLock();
    if (g_Status.Ready && InbvIsBootDriverInstalled() &&
        InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
    {
        InbvDisplayStatusLocked(String);
        Displayed = TRUE;
    }
    InbvReleaseLock();
    return Displayed;
}

VOID
NTAPI
InbvGopEraseStatus(VOID)
{
    if (!g_Status.Ready)
        return;

    InbvAcquireLock();
    if (g_Status.Ready &&
        InbvIsBootDriverInstalled() &&
        InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
    {
        InbvGopFillStatusBand();
    }
    InbvReleaseLock();
}

VOID
NTAPI
InbvGopClearStatus(VOID)
{
    PUCHAR Coverage;
    PULONG Row;

    if (!g_Status.Ready)
        return;

    InbvAcquireLock();
    if (!g_Status.Ready)
    {
        InbvReleaseLock();
        return;
    }
    if (InbvIsBootDriverInstalled() && InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
        InbvGopFillStatusBand();
    g_Status.Ready = FALSE;
    Coverage = g_Status.Coverage;
    Row = g_Status.Row;
    g_Status.Coverage = NULL;
    g_Status.Row = NULL;
    InbvReleaseLock();

    InbvGopStatusFree(Coverage, Row);
}

static VOID
NTAPI
InbvGopDisplayFilter(
    _Inout_ PCSTR *String)
{
    InbvGopDisplayStatus(*String);
    *String = "";
}

typedef struct _INBV_GOP_RGB
{
    ULONG Red;
    ULONG Green;
    ULONG Blue;
} INBV_GOP_RGB, *PINBV_GOP_RGB;

static UCHAR
InbvGopExtractChannel(ULONG Pixel, ULONG Mask)
{
    ULONG Shift;
    ULONG MaxValue;
    ULONG Value;

    if (!Mask)
        return 0;

    Shift = InbvMaskShift(Mask);
    MaxValue = InbvMaskMax(Mask >> Shift);
    if (!MaxValue)
        return 0;

    Value = (Pixel & Mask) >> Shift;
    return (UCHAR)(((ULONGLONG)Value * 255 + MaxValue / 2) / MaxValue);
}

static PULONG InbvBootScreen;
static ULONG InbvBootScreenWidth;
static ULONG InbvBootScreenHeight;
static EX_PUSH_LOCK InbvBootScreenLock;

static
PULONG
InbvTakeBootScreen(VOID)
{
    PULONG Pixels;

    KeEnterCriticalRegion();
    ExAcquirePushLockExclusive(&InbvBootScreenLock);
    Pixels = InterlockedExchangePointer((PVOID *)&InbvBootScreen, NULL);
    ExReleasePushLockExclusive(&InbvBootScreenLock);
    KeLeaveCriticalRegion();
    return Pixels;
}

BOOLEAN
NTAPI
InbvGopCaptureScreen(VOID)
{
    LOADER_PARAMETER_FRAMEBUFFER FbInfo;
    VID_DISPLAY_INFO DisplayInfo;
    PULONG Pixels;
    SIZE_T Count, i;

    if (InbvBootScreen)
        return TRUE;
    if (KeGetCurrentIrql() != PASSIVE_LEVEL ||
        !InbvIsBootDriverInstalled() ||
        InbvGetDisplayState() != INBV_DISPLAY_STATE_OWNED ||
        !InbvGetGopFrameBufferInfo(&FbInfo) ||
        !FbInfo.RedMask || !FbInfo.GreenMask || !FbInfo.BlueMask)
    {
        return FALSE;
    }

    InbvQueryDisplayInfo(&DisplayInfo);
    if (!DisplayInfo.PhysicalWidth || !DisplayInfo.PhysicalHeight ||
        DisplayInfo.PhysicalWidth > MAXULONG / sizeof(ULONG) / DisplayInfo.PhysicalHeight)
    {
        return FALSE;
    }

    Count = (SIZE_T)DisplayInfo.PhysicalWidth * DisplayInfo.PhysicalHeight;
    Pixels = ExAllocatePoolWithTag(PagedPool, Count * sizeof(ULONG), INBV_LOGO_POOL_TAG);
    if (!Pixels)
        return FALSE;

    VidScreenToBufferBlt((PUCHAR)Pixels,
                         0,
                         0,
                         DisplayInfo.PhysicalWidth,
                         DisplayInfo.PhysicalHeight,
                         DisplayInfo.PhysicalWidth * sizeof(ULONG));

    if (FbInfo.RedMask == 0x00FF0000 && FbInfo.GreenMask == 0x0000FF00 && FbInfo.BlueMask == 0x000000FF)
    {
        for (i = 0; i < Count; i++)
            Pixels[i] &= 0x00FFFFFF;
    }
    else
    {
        for (i = 0; i < Count; i++)
        {
            ULONG Pixel = Pixels[i];

            Pixels[i] = ((ULONG)InbvGopExtractChannel(Pixel, FbInfo.RedMask) << 16) |
                        ((ULONG)InbvGopExtractChannel(Pixel, FbInfo.GreenMask) << 8) |
                        (ULONG)InbvGopExtractChannel(Pixel, FbInfo.BlueMask);
        }
    }

    InbvBootScreenWidth = DisplayInfo.PhysicalWidth;
    InbvBootScreenHeight = DisplayInfo.PhysicalHeight;
    KeEnterCriticalRegion();
    ExAcquirePushLockExclusive(&InbvBootScreenLock);
    InterlockedExchangePointer((PVOID *)&InbvBootScreen, Pixels);
    ExReleasePushLockExclusive(&InbvBootScreenLock);
    KeLeaveCriticalRegion();
    return TRUE;
}

NTSTATUS
NTAPI
InbvQueryBootScreen(
    _Out_writes_bytes_opt_(*Height * Stride) PVOID Bits,
    _In_ LONG Stride,
    _Inout_ PULONG Width,
    _Inout_ PULONG Height)
{
    PULONG Pixels;
    ULONG y;

    if (!Width || !Height)
        return STATUS_INVALID_PARAMETER;

    if (!InbvGopCaptureScreen())
        return STATUS_NOT_FOUND;

    if (!Bits)
    {
        *Width = InbvBootScreenWidth;
        *Height = InbvBootScreenHeight;
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (*Width != InbvBootScreenWidth ||
        *Height != InbvBootScreenHeight ||
        (ULONG)((Stride < 0) ? -Stride : Stride) < InbvBootScreenWidth * sizeof(ULONG))
    {
        return STATUS_INVALID_PARAMETER;
    }

    Pixels = InbvTakeBootScreen();
    if (!Pixels)
        return STATUS_NOT_FOUND;

    for (y = 0; y < InbvBootScreenHeight; y++)
    {
        RtlCopyMemory((PUCHAR)Bits + (LONG_PTR)y * Stride,
                      Pixels + (SIZE_T)y * InbvBootScreenWidth,
                      InbvBootScreenWidth * sizeof(ULONG));
    }
    ExFreePoolWithTag(Pixels, INBV_LOGO_POOL_TAG);
    return STATUS_SUCCESS;
}

VOID
NTAPI
InbvDiscardBootScreen(VOID)
{
    PVOID Pixels = InbvTakeBootScreen();

    if (Pixels)
        ExFreePoolWithTag(Pixels, INBV_LOGO_POOL_TAG);
}

NTSTATUS
NTAPI
InbvCopyBootScreen(
    _Out_writes_bytes_(Height * Stride) PVOID Bits,
    _In_ LONG Stride,
    _In_ ULONG Width,
    _In_ ULONG Height)
{
    NTSTATUS Status = STATUS_NOT_FOUND;
    ULONG y;

    if (!Bits)
        return STATUS_INVALID_PARAMETER;

    KeEnterCriticalRegion();
    ExAcquirePushLockShared(&InbvBootScreenLock);
    if (InbvBootScreen)
    {
        if (Width != InbvBootScreenWidth ||
            Height != InbvBootScreenHeight ||
            (ULONG)((Stride < 0) ? -Stride : Stride) < InbvBootScreenWidth * sizeof(ULONG))
        {
            Status = STATUS_INVALID_PARAMETER;
        }
        else
        {
            for (y = 0; y < InbvBootScreenHeight; y++)
            {
                RtlCopyMemory((PUCHAR)Bits + (LONG_PTR)y * Stride,
                              InbvBootScreen + (SIZE_T)y * InbvBootScreenWidth,
                              InbvBootScreenWidth * sizeof(ULONG));
            }
            Status = STATUS_SUCCESS;
        }
    }
    ExReleasePushLockShared(&InbvBootScreenLock);
    KeLeaveCriticalRegion();
    return Status;
}

static VOID
InbvGopReadWordmarkPixel(
    _In_reads_bytes_(Stride * SrcHeight) const UCHAR *Bits,
    ULONG Stride,
    ULONG BitCount,
    BOOLEAN TopDown,
    ULONG SrcHeight,
    ULONG X,
    ULONG Y,
    _In_reads_opt_(PaletteCount) const RGBQUAD *Palette,
    ULONG PaletteCount,
    ULONG RedMask,
    ULONG GreenMask,
    ULONG BlueMask,
    ULONG AlphaMask,
    _Out_ PINBV_GOP_RGB Color)
{
    const UCHAR *Row;

    Row = Bits + (SIZE_T)(TopDown ? Y : (SrcHeight - 1 - Y)) * Stride;
    if (BitCount == 4)
    {
        UCHAR Value = Row[X / 2];
        ULONG Index = (X & 1) ? (Value & 0x0f) : (Value >> 4);

        if (Index >= PaletteCount)
            Index = 0;

        Color->Red = Palette[Index].rgbRed;
        Color->Green = Palette[Index].rgbGreen;
        Color->Blue = Palette[Index].rgbBlue;
    }
    else
    {
        ULONG Pixel;
        ULONG Alpha = 255;

        RtlCopyMemory(&Pixel, Row + (SIZE_T)X * sizeof(Pixel), sizeof(Pixel));
        Color->Red = InbvGopExtractChannel(Pixel, RedMask);
        Color->Green = InbvGopExtractChannel(Pixel, GreenMask);
        Color->Blue = InbvGopExtractChannel(Pixel, BlueMask);

        if (AlphaMask)
            Alpha = InbvGopExtractChannel(Pixel, AlphaMask);

        /* The boot background is black, so premultiply transparent pixels. */
        Color->Red = (Color->Red * Alpha + 127) / 255;
        Color->Green = (Color->Green * Alpha + 127) / 255;
        Color->Blue = (Color->Blue * Alpha + 127) / 255;
    }
}

static
BOOLEAN
InbvGopDrawWordmark(
    _In_ ULONG ScreenWidth,
    _In_ ULONG ScreenHeight,
    _In_opt_ const INBV_GOP_RECT *WordmarkRectOverride)
{
    PUCHAR Bitmap;
    PBITMAPINFOHEADER Header;
    ULONG SrcWidth, SrcHeight, SrcDelta;
    INBV_GOP_RECT WordmarkRect;
    ULONG DstWidth, DstHeight, DestX, DestY;
    BOOLEAN TopDown = FALSE;
    PUCHAR Bits;
    LPRGBQUAD Palette = NULL;
    ULONG PaletteCount = 0;
    ULONG SourceRedMask = 0x00ff0000;
    ULONG SourceGreenMask = 0x0000ff00;
    ULONG SourceBlueMask = 0x000000ff;
    ULONG SourceAlphaMask = 0;

    LOADER_PARAMETER_FRAMEBUFFER FbInfo;
    ULONG RedMask = 0, GreenMask = 0, BlueMask = 0;
    ULONG RedShift = 0, GreenShift = 0, BlueShift = 0;
    ULONG RedMax = 0, GreenMax = 0, BlueMax = 0;

    Bitmap = InbvGetResourceAddress(IDB_GOP_LOGO);
    Header = (PBITMAPINFOHEADER)Bitmap;
    if (!InbvGopValidateWordmarkHeader(Header))
        return FALSE;

    SrcWidth = (ULONG)Header->biWidth;
    SrcHeight = (Header->biHeight < 0) ? (ULONG)(-Header->biHeight) : (ULONG)Header->biHeight;
    TopDown = (Header->biHeight < 0);
    if (!SrcWidth || !SrcHeight)
        return FALSE;

    if (!InbvGopQueryInfo(&FbInfo))
        return FALSE;

    if (WordmarkRectOverride)
    {
        WordmarkRect = *WordmarkRectOverride;
    }
    else
    {
        if (!InbvGopComputeWordmarkRect(ScreenWidth, ScreenHeight, FbInfo.Dpi, &WordmarkRect))
            return FALSE;
    }

    DstWidth = WordmarkRect.Width;
    DstHeight = WordmarkRect.Height;
    DestX = WordmarkRect.X;
    DestY = WordmarkRect.Y;

    if (!DstWidth || !DstHeight || DestX >= ScreenWidth || DestY >= ScreenHeight)
    {
        return FALSE;
    }

    DstWidth = min(DstWidth, ScreenWidth - DestX);
    DstHeight = min(DstHeight, ScreenHeight - DestY);

    RedMask = FbInfo.RedMask; GreenMask = FbInfo.GreenMask; BlueMask = FbInfo.BlueMask;
    RedShift = InbvMaskShift(RedMask); GreenShift = InbvMaskShift(GreenMask); BlueShift = InbvMaskShift(BlueMask);
    RedMax = InbvMaskMax(RedMask >> RedShift); GreenMax = InbvMaskMax(GreenMask >> GreenShift); BlueMax = InbvMaskMax(BlueMask >> BlueShift);

    Bits = Bitmap + Header->biSize;
    if (Header->biBitCount == 4)
    {
        PaletteCount = Header->biClrUsed ? min(Header->biClrUsed, 16UL) : 16;
        Palette = (LPRGBQUAD)Bits;
        Bits += PaletteCount * sizeof(*Palette);
    }
    else if (Header->biCompression == BI_BITFIELDS)
    {
        const ULONG *Masks = (const ULONG *)(Bitmap + sizeof(BITMAPINFOHEADER));

        SourceRedMask = Masks[0];
        SourceGreenMask = Masks[1];
        SourceBlueMask = Masks[2];

        /*
         * V4/V5 headers embed four masks after the common 40-byte header.
         * A 40-byte BITMAPINFOHEADER stores three masks immediately before
         * the pixel array instead.
         */
        if (Header->biSize >= sizeof(BITMAPINFOHEADER) + 4 * sizeof(ULONG))
            SourceAlphaMask = Masks[3];
        else
            Bits += 3 * sizeof(*Masks);

        if (!SourceRedMask || !SourceGreenMask || !SourceBlueMask)
            return FALSE;
    }

    SrcDelta = (ULONG)((((ULONGLONG)SrcWidth * Header->biBitCount + 31) / 32) * 4);
    if (!SrcDelta)
        return FALSE;

    for (ULONG dy = 0; dy < DstHeight; dy++)
    {
        ULONG sy = (ULONG)min((ULONGLONG)SrcHeight - 1,
                              ((ULONGLONG)dy * SrcHeight) / DstHeight);
        ULONG sy1 = (DstHeight < SrcHeight) ? min(sy + 1, SrcHeight - 1) : sy;

        static ULONG LineBuf[1024];
        ULONG produced = 0;
        while (produced < DstWidth)
        {
            ULONG toDo = min((ULONG)1024, DstWidth - produced);
            for (ULONG dx = 0; dx < toDo; dx++)
            {
                ULONG sx0 = (ULONG)min((ULONGLONG)SrcWidth - 1,
                                       ((ULONGLONG)(produced + dx) * SrcWidth) /
                                       DstWidth);
                ULONG sx1 = (DstWidth < SrcWidth) ? min(sx0 + 1, SrcWidth - 1) : sx0;
                INBV_GOP_RGB C00, C10, C01, C11;
                ULONG r, g, b;

                InbvGopReadWordmarkPixel(Bits, SrcDelta, Header->biBitCount,
                                         TopDown, SrcHeight, sx0, sy,
                                         Palette, PaletteCount,
                                         SourceRedMask, SourceGreenMask,
                                         SourceBlueMask, SourceAlphaMask, &C00);
                InbvGopReadWordmarkPixel(Bits, SrcDelta, Header->biBitCount,
                                         TopDown, SrcHeight, sx1, sy,
                                         Palette, PaletteCount,
                                         SourceRedMask, SourceGreenMask,
                                         SourceBlueMask, SourceAlphaMask, &C10);
                InbvGopReadWordmarkPixel(Bits, SrcDelta, Header->biBitCount,
                                         TopDown, SrcHeight, sx0, sy1,
                                         Palette, PaletteCount,
                                         SourceRedMask, SourceGreenMask,
                                         SourceBlueMask, SourceAlphaMask, &C01);
                InbvGopReadWordmarkPixel(Bits, SrcDelta, Header->biBitCount,
                                         TopDown, SrcHeight, sx1, sy1,
                                         Palette, PaletteCount,
                                         SourceRedMask, SourceGreenMask,
                                         SourceBlueMask, SourceAlphaMask, &C11);

                r = (C00.Red + C10.Red + C01.Red + C11.Red) / 4;
                g = (C00.Green + C10.Green + C01.Green + C11.Green) / 4;
                b = (C00.Blue + C10.Blue + C01.Blue + C11.Blue) / 4;

                LineBuf[dx] = InbvPackColor(RedMask,GreenMask,BlueMask,
                                             RedShift,GreenShift,BlueShift,
                                             RedMax,GreenMax,BlueMax,
                                             (UCHAR)r,(UCHAR)g,(UCHAR)b);
            }
            VidBufferToScreenBlt((PUCHAR)LineBuf,
                                 DestX + produced,
                                 DestY + dy,
                                 toDo,
                                 1,
                                 toDo * sizeof(ULONG));
            produced += toDo;
        }
    }

    return TRUE;
}

CODE_SEG("INIT")
BOOLEAN
NTAPI
InbvGopHandleBootBitmap(VOID)
{
    LOADER_PARAMETER_FRAMEBUFFER FbInfo;
    VID_DISPLAY_INFO DisplayInfo;
    ULONG Width, Height;
    BOOLEAN WordmarkDrawn;

    if (!InbvGopQueryInfo(&FbInfo))
        return FALSE;

    /* Native 32-bit blits use physical dimensions after rotation. */
    if (!InbvQueryDisplayInfo(&DisplayInfo))
        return FALSE;
    Width = DisplayInfo.PhysicalWidth;
    Height = DisplayInfo.PhysicalHeight;
    if (!Width || !Height || !DisplayInfo.Width || !DisplayInfo.Height)
        return FALSE;

    InbvSetTextColor(BV_COLOR_WHITE);
    InbvAcquireLock();
    if (!InbvIsBootDriverInstalled() || InbvGetDisplayState() != INBV_DISPLAY_STATE_OWNED)
    {
        InbvReleaseLock();
        return FALSE;
    }
    ASSERT(DisplayInfo.Width && DisplayInfo.Height);
    VidSolidColorFill(0, 0, DisplayInfo.Width - 1, DisplayInfo.Height - 1, BV_COLOR_BLACK);
    WordmarkDrawn = InbvGopDrawWordmark(Width, Height, NULL);
    InbvReleaseLock();
    InbvInitializeBootStatus();

    if (!WordmarkDrawn)
    {
        static const CHAR LoadingMsg[] = "ReliefWin";
        ULONG CharW;
        ULONG msgPx;
        ULONG x;
        ULONG y;

        CharW = DisplayInfo.CharacterWidth ? DisplayInfo.CharacterWidth : 8;
        msgPx = (ULONG)(sizeof(LoadingMsg) - 1) * CharW;
        x = (DisplayInfo.Width > msgPx) ? ((DisplayInfo.Width - msgPx) / 2) : 0;
        y = (ULONG)(DisplayInfo.Height * INBV_WORDMARK_TOP_VPOS + 0.5f);
        VidDisplayStringXY(LoadingMsg, x, y, TRUE);
    }

    InbvEnableDisplayString(FALSE);
    return TRUE;
}
