/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Boot manager screens: operating system menu and startup settings
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 */

#include <bl.h>
#include <bmui.h>
#include <gui.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "bootui_mono.h"
#include "bootui_mark.h"
#include "bootui_unlock.h"
#include "bootui_icons.h"
#include "cryptinput.h"
#include "cryptstrings.h"
#include <reliefwin/bootlogo.h>

#define BMUI_GRAPHICS_COLUMNS   100
#define BMUI_TEXT_COLUMNS       80
#define BMUI_ROWS               25
#define BMUI_MONO_EM_NUMERATOR  2
#define BMUI_MONO_EM_DENOMINATOR 3
#define BMUI_BLACK              0x000000
#define BMUI_GRAY               0xBFBFBF
#define BMUI_WHITE              0xFFFFFF
#define BMUI_POLL_MICROSECONDS  10000
#define BMUI_POLLS_PER_SECOND   100
#define BMUI_TEXT_CHARS         128

#define BMUI_ROW_TITLE          0
#define BMUI_ROW_HEADING        2
#define BMUI_ROW_HINT           3
#define BMUI_ROW_ENTRIES        5
#define BMUI_ROW_ADVANCED       16
#define BMUI_ROW_COUNTDOWN      17
#define BMUI_ROW_DESCRIPTION    18
#define BMUI_DESCRIPTION_ROWS   5
#define BMUI_ROW_FOOTER         24
#define BMUI_COLUMN_ENTRY       6
#define BMUI_ENTRY_RIGHT_MARGIN 10
#define BMUI_MAX_ENTRIES        (BMUI_ROW_ADVANCED - BMUI_ROW_ENTRIES - 1)

static ULONG BmUiWidth;
static ULONG BmUiHeight;
static ULONG BmUiColumns;
static ULONG BmUiCellWidth;
static ULONG BmUiCellHeight;
static PULONG BmUiPixels;
static SIZE_T BmUiPixelBytes;
static FT_Library BmUiLibrary;
static FT_Face BmUiMono;
static BOOLEAN BmUiGraphics;
static BOOLEAN BmUiBootActive;
static ULONG BmUiBootProgressValue;

static
VOID
BmUiFill(
    _In_ ULONG Left,
    _In_ ULONG Top,
    _In_ ULONG Width,
    _In_ ULONG Height,
    _In_ ULONG Color)
{
    ULONG X, Y;

    for (Y = Top; Y < min(Top + Height, BmUiHeight); Y++)
    {
        for (X = Left; X < min(Left + Width, BmUiWidth); X++)
            BmUiPixels[(SIZE_T)Y * BmUiWidth + X] = Color;
    }
}

static
ULONG
BmUiBlend(
    _In_ ULONG Background,
    _In_ ULONG Foreground,
    _In_ ULONG Alpha)
{
    ULONG Result = 0;
    ULONG Shift, Back, Fore;

    for (Shift = 0; Shift < 24; Shift += 8)
    {
        Back = (Background >> Shift) & 0xFF;
        Fore = (Foreground >> Shift) & 0xFF;
        Result |= ((Back * (255 - Alpha) + Fore * Alpha) / 255) << Shift;
    }

    return Result;
}

static
ULONG
BmUiDrawText(
    _In_ FT_Face Face,
    _In_ LONG X,
    _In_ LONG Baseline,
    _In_ PCWSTR Text,
    _In_ ULONG Color)
{
    FT_GlyphSlot Slot = Face->glyph;
    LONG Pen = X;
    LONG PixelX, PixelY;
    ULONG Row, Column, Alpha;
    PULONG Pixel;

    for (; *Text != UNICODE_NULL; Text++)
    {
        if (FT_Load_Char(Face, *Text, FT_LOAD_RENDER) != FT_Err_Ok)
            continue;

        for (Row = 0; Row < Slot->bitmap.rows; Row++)
        {
            PixelY = Baseline - Slot->bitmap_top + (LONG)Row;
            if (PixelY < 0 || PixelY >= (LONG)BmUiHeight)
                continue;

            for (Column = 0; Column < Slot->bitmap.width; Column++)
            {
                PixelX = Pen + Slot->bitmap_left + (LONG)Column;
                Alpha = Slot->bitmap.buffer[Row * Slot->bitmap.pitch + Column];
                if (Alpha == 0 || PixelX < 0 || PixelX >= (LONG)BmUiWidth)
                    continue;

                Pixel = &BmUiPixels[(SIZE_T)PixelY * BmUiWidth + PixelX];
                *Pixel = BmUiBlend(*Pixel, Color, Alpha);
            }
        }

        Pen += Slot->advance.x >> 6;
    }

    return (ULONG)(Pen - X);
}

static
VOID
BmUiPresent(
    _In_ ULONG Top,
    _In_ ULONG Height)
{
    if (BmUiGraphics)
        VidFbPresent(BmUiPixels, 0, Top, BmUiWidth, min(Height, BmUiHeight - Top));
}

static
VOID
BmUiAttrColors(
    _In_ UCHAR Attr,
    _Out_ PULONG Foreground,
    _Out_ PULONG Background)
{
    *Background = (Attr == BMUI_ATTR_INVERSE) ? BMUI_GRAY : BMUI_BLACK;
    *Foreground = (Attr == BMUI_ATTR_INVERSE) ? BMUI_BLACK :
                  (Attr == BMUI_ATTR_BRIGHT) ? BMUI_WHITE : BMUI_GRAY;
}

static
ULONG
BmUiColumnX(
    _In_ ULONG Column)
{
    return (ULONG)((ULONGLONG)Column * BmUiWidth / BmUiColumns);
}

static
ULONG
BmUiRowY(
    _In_ ULONG Row)
{
    return (ULONG)((ULONGLONG)Row * BmUiHeight / BMUI_ROWS);
}

static
VOID
BmUiPutText(
    _In_ ULONG Row,
    _In_ ULONG Column,
    _In_ ULONG Width,
    _In_ PCWSTR Text,
    _In_ UCHAR Attr)
{
    WCHAR Line[BMUI_TEXT_CHARS];
    ULONG Foreground, Background, i;

    Width = min(Width, BmUiColumns - min(Column, BmUiColumns));
    Width = min(Width, BMUI_TEXT_CHARS - 1);

    for (i = 0; i < Width; i++)
        Line[i] = (Text && *Text) ? *Text++ : L' ';
    Line[Width] = UNICODE_NULL;

    if (!BmUiGraphics)
    {
        for (i = 0; i < Width; i++)
            FbConsPutChar(Line[i] <= 0x7F ? (UCHAR)Line[i] : '?', Attr, Column + i, Row);
        return;
    }

    BmUiAttrColors(Attr, &Foreground, &Background);
    BmUiFill(BmUiColumnX(Column),
             BmUiRowY(Row),
             BmUiColumnX(Column + Width) - BmUiColumnX(Column),
             BmUiRowY(Row + 1) - BmUiRowY(Row),
             Background);

    for (i = 0; i < Width; i++)
    {
        WCHAR Glyph[2] = {Line[i], UNICODE_NULL};

        if (Glyph[0] == L' ')
            continue;

        BmUiDrawText(BmUiMono,
                     (LONG)BmUiColumnX(Column + i),
                     (LONG)BmUiRowY(Row) +
                         (LONG)(BmUiCellHeight + (BmUiMono->size->metrics.ascender >> 6) +
                                (BmUiMono->size->metrics.descender >> 6)) / 2,
                     Glyph,
                     Foreground);
    }
}

static
VOID
BmUiPutRow(
    _In_ ULONG Row,
    _In_ PCWSTR Text,
    _In_ UCHAR Attr)
{
    BmUiPutText(Row, 0, BmUiColumns, Text, Attr);
}

static
VOID
BmUiClear(
    _In_ ULONG Color,
    _In_ UCHAR Attr)
{
    if (BmUiGraphics)
        BmUiFill(0, 0, BmUiWidth, BmUiHeight, Color);
    else
        FbConsClearScreen(Attr);
}

static
int
BmUiWaitKey(
    _Inout_opt_ PLONG Seconds)
{
    ULONG Poll;

    if (!Seconds || *Seconds < 0)
        return FwReadKey();

    for (Poll = 0; Poll < BMUI_POLLS_PER_SECOND; Poll++)
    {
        if (FwKeyAvailable())
            return FwReadKey();

        StallExecutionProcessor(BMUI_POLL_MICROSECONDS);
    }

    (*Seconds)--;
    return -1;
}

static
int
BmUiReadKey(
    _Inout_opt_ PLONG Seconds,
    _Out_ PBOOLEAN Extended)
{
    int Key = BmUiWaitKey(Seconds);

    *Extended = (Key == KEY_EXTENDED);
    return *Extended ? FwReadKey() : Key;
}

BOOLEAN
BmUiWaitForKey(
    _In_ LONG Seconds)
{
    BOOLEAN Extended;

    while (Seconds > 0)
    {
        if (BmUiReadKey(&Seconds, &Extended) >= 0)
            return TRUE;
    }

    return FALSE;
}

BOOLEAN
BmUiInitialize(VOID)
{
    ULONG Depth;

    VidFbGetDisplaySize(&BmUiWidth, &BmUiHeight, &Depth);
    BmUiGraphics = FALSE;
    BmUiColumns = BMUI_TEXT_COLUMNS;

    if (BmUiWidth == 0 || BmUiHeight == 0 ||
        BmUiWidth > MAXULONG / sizeof(ULONG) / BmUiHeight)
        return TRUE;

    BmUiPixelBytes = (SIZE_T)BmUiWidth * BmUiHeight * sizeof(ULONG);
    BmUiPixels = FwAllocatePages(BmUiPixelBytes);
    if (!BmUiPixels)
        return TRUE;

    if (!BmUiLibrary &&
        (FT_Init_FreeType(&BmUiLibrary) != FT_Err_Ok ||
         FT_New_Memory_Face(BmUiLibrary, bootui_mono, bootui_mono_SIZE, 0, &BmUiMono) != FT_Err_Ok))
    {
        FwFreePages(BmUiPixels, BmUiPixelBytes);
        BmUiPixels = NULL;
        return TRUE;
    }

    BmUiColumns = BMUI_GRAPHICS_COLUMNS;
    BmUiCellWidth = BmUiWidth / BmUiColumns;
    BmUiCellHeight = BmUiHeight / BMUI_ROWS;
    BmUiGraphics = (BmUiCellWidth != 0 && BmUiCellHeight != 0);
    return TRUE;
}

VOID
BmUiShutdown(VOID)
{
    BmUiClear(BMUI_BLACK, BMUI_ATTR_NORMAL);
    BmUiPresent(0, BmUiHeight);

    if (BmUiPixels)
        FwFreePages(BmUiPixels, BmUiPixelBytes);

    BmUiPixels = NULL;
    BmUiGraphics = FALSE;
}

static
VOID
BmUiSetMonoSize(VOID)
{
    FT_F26Dot6 Height = (FT_F26Dot6)BmUiCellHeight * BMUI_MONO_EM_NUMERATOR * 64 / BMUI_MONO_EM_DENOMINATOR;
    FT_F26Dot6 Width = (FT_F26Dot6)BmUiCellWidth * BmUiMono->units_per_EM * 64 / BmUiMono->max_advance_width;

    FT_Set_Char_Size(BmUiMono, Width, Height, 72, 72);
}

BOOLEAN
BmUiBootProgress(
    _In_opt_ PCSTR Component,
    _In_ ULONG Progress)
{
    WCHAR Line[128];
    ULONG Width, Left, Top, Height, Count, i;

    if (!BmUiBootActive)
        return FALSE;
    if (Progress != MAXULONG)
        BmUiBootProgressValue = max(BmUiBootProgressValue, min(Progress, 10000UL));

    if (BmUiGraphics)
    {
        Height = max(1UL, BmUiCellHeight / 4);
        Width = (ULONG)(((ULONGLONG)BmUiWidth * BmUiBootProgressValue) / 10000);
        BmUiFill(0, 0, BmUiWidth, Height, 0x303030);
        BmUiFill(0, 0, Width, Height, BMUI_WHITE);
        BmUiPresent(0, Height);
    }

    if (!Component || !Component[0])
        return TRUE;

    Count = min((ULONG)RTL_NUMBER_OF(Line) - 10, BmUiColumns > 10 ? BmUiColumns - 10 : 0);
    RtlCopyMemory(Line, L"Loading ", 8 * sizeof(WCHAR));
    for (i = 0; i < Count && Component[i]; i++)
        Line[8 + i] = (WCHAR)(UCHAR)Component[i];
    Line[8 + i] = 0x2026;
    Line[9 + i] = UNICODE_NULL;

    if (BmUiGraphics)
    {
        Width = (8 + i + 1) * BmUiCellWidth;
        Left = (BmUiWidth > Width) ? (BmUiWidth - Width) / 2 : 0;
        Top = (ULONG)(((ULONGLONG)BmUiHeight * 13) / 16);
        Top = Top > 20 + BmUiCellHeight / 2 ? Top - 20 - BmUiCellHeight / 2 : 0;
        Top = min(Top, BmUiHeight - BmUiCellHeight);
        BmUiFill(0, Top, BmUiWidth, BmUiCellHeight, BMUI_BLACK);
        BmUiSetMonoSize();
        BmUiDrawText(BmUiMono, Left,
                     Top + (BmUiCellHeight + (BmUiMono->size->metrics.ascender >> 6) +
                            (BmUiMono->size->metrics.descender >> 6)) / 2,
                     Line, BMUI_WHITE);
        BmUiPresent(Top, BmUiCellHeight);
    }
    else
    {
        Line[8 + i] = '.';
        BmUiPutText(19, 0, BMUI_TEXT_COLUMNS, L"", BMUI_ATTR_NORMAL);
        BmUiPutText(19, (BMUI_TEXT_COLUMNS - 9 - i) / 2,
                     BMUI_TEXT_COLUMNS, Line, BMUI_ATTR_BRIGHT);
    }
    return TRUE;
}

static ULONG
BmUiBootMarkPixel(
    _In_ const UCHAR *Bits,
    _In_ ULONG Width,
    _In_ ULONG Height,
    _In_ ULONG X,
    _In_ ULONG Y)
{
    const UCHAR *Pixel = Bits + ((SIZE_T)(Height - 1 - Y) * Width + X) * 4;

    return (((Pixel[2] * Pixel[3] + 127) / 255) << 16) |
           (((Pixel[1] * Pixel[3] + 127) / 255) << 8) |
           ((Pixel[0] * Pixel[3] + 127) / 255);
}

static VOID
BmUiDrawBootMark(VOID)
{
    RELIEF_BOOT_LOGO_RECT Rect;
    ULONG Width = LeUlong(bootui_mark + 18);
    ULONG Height = LeUlong(bootui_mark + 22);
    const UCHAR *Bits = bootui_mark + LeUlong(bootui_mark + 10);
    ULONG X, Y, Sx, Sy, Sx1, Sy1, C00, C10, C01, C11, Color, Shift;

    if (!ReliefBootLogoGetRect(BmUiWidth, BmUiHeight, Width, Height, &Rect))
        return;

    for (Y = 0; Y < Rect.Height; Y++)
    {
        Sy = (ULONG)(((ULONGLONG)Y * Height) / Rect.Height);
        Sy1 = Rect.Height < Height ? min(Sy + 1, Height - 1) : Sy;
        for (X = 0; X < Rect.Width; X++)
        {
            Sx = (ULONG)(((ULONGLONG)X * Width) / Rect.Width);
            Sx1 = Rect.Width < Width ? min(Sx + 1, Width - 1) : Sx;
            C00 = BmUiBootMarkPixel(Bits, Width, Height, Sx, Sy);
            C10 = BmUiBootMarkPixel(Bits, Width, Height, Sx1, Sy);
            C01 = BmUiBootMarkPixel(Bits, Width, Height, Sx, Sy1);
            C11 = BmUiBootMarkPixel(Bits, Width, Height, Sx1, Sy1);
            Color = 0;
            for (Shift = 0; Shift < 24; Shift += 8)
                Color |= ((((C00 >> Shift) & 255) + ((C10 >> Shift) & 255) +
                           ((C01 >> Shift) & 255) + ((C11 >> Shift) & 255)) / 4) << Shift;
            BmUiPixels[(SIZE_T)(Rect.Y + Y) * BmUiWidth + Rect.X + X] = Color;
        }
    }
}

VOID
BmUiBeginBoot(VOID)
{

    BmUiInitialize();
    BmUiBootActive = TRUE;
    BmUiBootProgressValue = 0;
    BmUiClear(BMUI_BLACK, BMUI_ATTR_NORMAL);
    if (BmUiGraphics)
    {
        BmUiDrawBootMark();
        BmUiPresent(0, BmUiHeight);
    }
    BmUiBootProgress("winload.exe", 0);
}

VOID
BmUiEndBoot(VOID)
{
    if (!BmUiBootActive)
        return;
    if (BmUiPixels)
        FwFreePages(BmUiPixels, BmUiPixelBytes);
    BmUiPixels = NULL;
    BmUiGraphics = FALSE;
    BmUiBootActive = FALSE;
}

static FT_Face BmUiCryptFace;
static ULONG BmUiCryptLanguage;
static BOOLEAN BmUiCryptRecovery;

static VOID
BmUiCryptRect(ULONG X, ULONG Y, ULONG Width, ULONG Height, ULONG Radius, ULONG Color)
{
    ULONG Row, Column, Alpha;
    LONG Dx, Dy, Distance;
    Radius = min(Radius, min(Width, Height) / 2);
    for (Row = Y; Row < min(Y + Height, BmUiHeight); ++Row)
        for (Column = X; Column < min(X + Width, BmUiWidth); ++Column)
        {
            Dx = max((LONG)Radius - (LONG)(Column - X), (LONG)Column - (LONG)(X + Width - 1 - Radius));
            Dy = max((LONG)Radius - (LONG)(Row - Y), (LONG)Row - (LONG)(Y + Height - 1 - Radius));
            Alpha = 255;
            if (Radius && Dx > 0 && Dy > 0)
            {
                Distance = Dx * Dx + Dy * Dy;
                if (Distance > (LONG)(Radius * Radius)) continue;
                if (Distance > (LONG)((Radius - 1) * (Radius - 1)))
                    Alpha = 255 * (Radius * Radius - Distance) / (2 * Radius - 1);
            }
            BmUiPixels[(SIZE_T)Row * BmUiWidth + Column] =
                BmUiBlend(BmUiPixels[(SIZE_T)Row * BmUiWidth + Column], Color, Alpha);
        }
}

static VOID
BmUiCryptText(PCWSTR Text, ULONG Size, ULONG Center, ULONG Baseline, ULONG Color)
{
    PCWSTR Character;
    ULONG Width = 0;
    FT_Set_Pixel_Sizes(BmUiCryptFace, 0, Size);
    for (Character = Text; *Character; ++Character)
        if (FT_Load_Char(BmUiCryptFace, *Character, FT_LOAD_DEFAULT) == FT_Err_Ok)
            Width += BmUiCryptFace->glyph->advance.x >> 6;
    BmUiDrawText(BmUiCryptFace, (LONG)Center - (LONG)Width / 2, Baseline, Text, Color);
}

static VOID
BmUiCryptLogo(ULONG Top)
{
    const UCHAR *Bits = bootui_mark + LeUlong(bootui_mark + 10), *Pixel;
    ULONG Width = LeUlong(bootui_mark + 18), Height = LeUlong(bootui_mark + 22);
    ULONG X, Y, Left = (BmUiWidth - 64) / 2;
    for (Y = 0; Y < 64; ++Y)
        for (X = 0; X < 64; ++X)
        {
            Pixel = Bits + ((SIZE_T)(Height - 1 - Y * Height / 64) * Width + X * Width / 64) * 4;
            BmUiPixels[(SIZE_T)(Top + Y) * BmUiWidth + Left + X] =
                BmUiBlend(BmUiPixels[(SIZE_T)(Top + Y) * BmUiWidth + Left + X],
                          (Pixel[2] << 16) | (Pixel[1] << 8) | Pixel[0], Pixel[3]);
        }
}

static ULONG
BmUiCryptHit(ULONG X, ULONG Y)
{
    ULONG Left = (BmUiWidth - 360) / 2, Top = (BmUiHeight - 460) / 2;
    if (X >= Left && X < Left + 360)
    {
        if (Y >= Top + 166 && Y < Top + 214) return 0;
        if (Y >= Top + 264 && Y < Top + 308)
            return X < Left + 172 ? 1 : (X >= Left + 188 ? 3 : MAXULONG);
        if (Y >= Top + 326 && Y < Top + 362) return 2;
    }
    if (X >= BmUiWidth / 2 - 100 && X < BmUiWidth / 2 + 100 &&
        Y >= BmUiHeight - 68 && Y < BmUiHeight - 32) return 4;
    return MAXULONG;
}

static VOID
BmUiCryptFrame(const BM_CRYPT_EDIT *Edit, PCSTR Id, ULONG Status, BOOLEAN Caret,
               const MACH_POINTER_STATE *Pointer, BOOLEAN ShowPointer)
{
    const BM_CRYPT_TEXT *Text = &BmCryptText[BmUiCryptLanguage];
    ULONG X, Y, Color, Left = (BmUiWidth - 360) / 2, Top = (BmUiHeight - 460) / 2;
    ULONG Start, End, Index, Width;
    WCHAR Volume[38];
    PCWSTR Message = NULL;
    for (Y = 0; Y < BmUiHeight; ++Y)
    {
        Color = BmUiBlend(0x163966, 0x080F20, Y * 255 / BmUiHeight);
        BmUiFill(0, Y, BmUiWidth, 1, Color);
    }
    BmUiCryptLogo(Top + 6);
    BmUiCryptText(Text->Title, 30, BmUiWidth / 2, Top + 111, 0xFFFFFF);
    BmUiCryptText(Edit->Recovery ? Text->RecoveryDescription : Text->Description,
                 16, BmUiWidth / 2, Top + 143, 0xCDD9EA);
    if (Status < 4)
    {
        Color = Edit->Focus == 0 ? 0x9CCBFF : 0x657B99;
        BmUiCryptRect(Left, Top + 166, 360, 48, 7, Color);
        BmUiCryptRect(Left + 1, Top + 167, 358, 46, 6, 0x0A172A);
        if (!Edit->Length)
        {
            FT_Set_Pixel_Sizes(BmUiCryptFace, 0, 17);
            BmUiDrawText(BmUiCryptFace, Left + 16, Top + 197,
                         Edit->Recovery ? Text->Recovery : Text->Password, 0xA9BAD1);
        }
        Start = Edit->Cursor > 26 ? Edit->Cursor - 26 : 0;
        End = min(Edit->Length, Start + 27);
        FT_Set_Pixel_Sizes(BmUiCryptFace, 0, 18);
        for (Index = Start; Index < End; ++Index)
        {
            WCHAR Bullet[2] = {0x2022, 0};
            BmUiDrawText(BmUiCryptFace, Left + 16 + (Index - Start) * 12, Top + 197, Bullet, 0xFFFFFF);
        }
        if (Edit->Focus == 0 && Caret && Status != 3)
            BmUiFill(Left + 16 + (Edit->Cursor - Start) * 12, Top + 178, 1, 23, 0xFFFFFF);
    }
    if (Status == 1) Message = Text->Wrong;
    if (Status == 2) Message = Edit->Recovery ? Text->RecoveryFormat : Text->PasswordFormat;
    if (Status == 3) Message = Text->Busy;
    if (Status == 4) Message = Text->Failed;
    if (Status == 5) Message = Text->Incomplete;
    if (Message) BmUiCryptText(Message, Status >= 4 ? 15 : 14, BmUiWidth / 2, Top + 241,
                             Status == 3 ? 0xCDD9EA : 0xFFD2C9);
    if (Status >= 4)
    {
        BmUiCryptRect(Left, Top + 264, 360, 44, 6, 0x9CCBFF);
        BmUiCryptText(Text->Back, 17, BmUiWidth / 2, Top + 293, 0x071629);
    }
    else
    {
        for (Index = 0; Index < 2; ++Index)
        {
            X = Left + Index * 188;
            Color = Index ? 0x354A65 : 0x9CCBFF;
            if (Edit->Focus == (Index ? 3 : 1))
                BmUiCryptRect(X - 3, Top + 261, 178, 50, 8, 0xFFFFFF);
            BmUiCryptRect(X, Top + 264, 172, 44, 6, Color);
            BmUiCryptText(Index ? Text->Cancel : Text->Unlock, 17, X + 86, Top + 293,
                         Index ? 0xFFFFFF : 0x071629);
        }
        if (Edit->Focus == 2) BmUiCryptRect(Left, Top + 326, 360, 36, 5, 0x304A6D);
        BmUiCryptText(Edit->Recovery ? Text->UsePassword : Text->UseRecovery,
                     16, BmUiWidth / 2, Top + 350, 0xBDDDFE);
    }
    if (Id)
    {
        for (Index = 0; Index < 36 && Id[Index]; ++Index) Volume[Index] = (UCHAR)Id[Index];
        Volume[Index] = 0;
        BmUiCryptText(Volume, 12, BmUiWidth / 2, Top + 390, 0x9AAEC9);
    }
    if (Status < 4)
    {
        if (Edit->Focus == 4)
            BmUiCryptRect(BmUiWidth / 2 - 100, BmUiHeight - 68, 200, 36, 5, 0x304A6D);
        BmUiCryptText(Text->Language, 15, BmUiWidth / 2, BmUiHeight - 44, 0xCDD9EA);
        BmUiCryptText(Text->Hint, 12, BmUiWidth / 2, BmUiHeight - 14, 0x9AAEC9);
    }
    if (ShowPointer && Pointer)
        for (Y = 0; Y < 24; ++Y)
            for (X = 0; X < 24; ++X)
            {
                LONG Px = (LONG)Pointer->X - 6 + X, Py = (LONG)Pointer->Y - 3 + Y;
                if (Px < 0 || Py < 0 || Px >= (LONG)BmUiWidth || Py >= (LONG)BmUiHeight) continue;
                Width = bootui_icons[3 * 24 * 24 + Y * 24 + X];
                BmUiPixels[(SIZE_T)Py * BmUiWidth + Px] =
                    BmUiBlend(BmUiPixels[(SIZE_T)Py * BmUiWidth + Px], 0xFFFFFF, Width);
            }
    BmUiPresent(0, BmUiHeight);
}

static BOOLEAN
BmUiCryptStart(VOID)
{
    if (!BmUiPixels) BmUiInitialize();
    if (!BmUiGraphics || BmUiWidth < 640 || BmUiHeight < 480) return FALSE;
    if (!BmUiCryptFace && FT_New_Memory_Face(BmUiLibrary, bootui_unlock, bootui_unlock_SIZE,
                                           0, &BmUiCryptFace) != FT_Err_Ok) return FALSE;
    return TRUE;
}

BOOLEAN
BmUiCryptPassword(PCSTR Id, BOOLEAN Failed, PCHAR Secret, ULONG Capacity, PULONG Length, PBOOLEAN Cancelled)
{
    BM_CRYPT_EDIT Edit = {0};
    MACH_POINTER_STATE Pointer = {0}, Sample;
    BM_CRYPT_POINTER Click = {0};
    BOOLEAN Extended, Caret = TRUE, Dirty = TRUE, ShowPointer = FALSE;
    ULONG Status = Failed ? 1 : 0, Hit, Polls = 0;
    int Key, Action;
    *Length = 0;
    *Cancelled = FALSE;
    BmCryptClear(&Edit, Secret, Capacity);
    Edit.Recovery = BmUiCryptRecovery;
    if (!BmUiCryptStart()) return FALSE;
    FwDebugPrint("Graphical disk unlock ready\n");
    for (;;)
    {
        if (Dirty)
        {
            BmUiCryptFrame(&Edit, Id, Status, Caret, &Pointer, ShowPointer);
            Dirty = FALSE;
        }
        Key = -1;
        Extended = FALSE;
        if (FwKeyAvailable())
        {
            Key = FwReadKey();
            Extended = Key == KEY_EXTENDED;
            if (Extended) Key = FwReadKey();
        }
        if (FwGetPointerState(BmUiWidth, BmUiHeight, &Sample))
        {
            Pointer = Sample;
            Dirty = TRUE;
            ShowPointer = !Pointer.Absolute;
            Hit = BmUiCryptHit(Pointer.X, Pointer.Y);
            if (BmCryptPointer(&Edit, &Click, TRUE, Hit, Pointer.LeftButton))
            {
                Key = KEY_ENTER;
                Extended = FALSE;
            }
        }
        if (Key >= 0)
        {
            Action = BmCryptKey(&Edit, Secret, Capacity, Key, Extended);
            if (Action == BmCryptCancel)
            {
                BmCryptClear(&Edit, Secret, Capacity);
                *Cancelled = TRUE;
                return FALSE;
            }
            if (Action == BmCryptSubmit)
            {
                BmUiCryptRecovery = Edit.Recovery;
                *Length = Edit.Length;
                BmUiCryptFrame(&Edit, Id, 3, FALSE, NULL, FALSE);
                return TRUE;
            }
            if (Action == BmCryptLanguage) BmUiCryptLanguage = (BmUiCryptLanguage + 1) % RTL_NUMBER_OF(BmCryptText);
            if (Action == BmCryptInvalid) Status = 2;
            else if (Action != BmCryptLanguage) Status = 0;
            Caret = TRUE;
            Polls = 0;
            Dirty = TRUE;
        }
        StallExecutionProcessor(BMUI_POLL_MICROSECONDS);
        if (++Polls == 50)
        {
            Polls = 0;
            Caret = !Caret;
            Dirty = TRUE;
        }
    }
}

VOID
BmUiCryptFailure(PCSTR Id, BOOLEAN Incomplete)
{
    BM_CRYPT_EDIT Edit = {0};
    MACH_POINTER_STATE Pointer;
    BM_CRYPT_POINTER Click = {0};
    int Key;
    if (!BmUiCryptStart())
    {
        BmUiShutdown();
        FwPrint(L"The graphical disk unlock screen is unavailable.\r\n");
        return;
    }
    BmUiCryptFrame(&Edit, Id, Incomplete ? 5 : 4, FALSE, NULL, FALSE);
    for (;;)
    {
        if (FwKeyAvailable())
        {
            Key = FwReadKey();
            if (Key == KEY_ENTER || Key == KEY_ESC) break;
            if (Key == KEY_EXTENDED) FwReadKey();
        }
        if (FwGetPointerState(BmUiWidth, BmUiHeight, &Pointer) &&
            BmCryptPointer(&Edit, &Click, TRUE,
                Pointer.X >= (BmUiWidth - 360) / 2 && Pointer.X < (BmUiWidth + 360) / 2 &&
                Pointer.Y >= (BmUiHeight - 460) / 2 + 264 &&
                Pointer.Y < (BmUiHeight - 460) / 2 + 308 ? 1 : MAXULONG, Pointer.LeftButton)) break;
        StallExecutionProcessor(BMUI_POLL_MICROSECONDS);
    }
    BmUiShutdown();
}

static
VOID
BmUiDrawEntries(
    _In_reads_(Count) PCWSTR* Names,
    _In_ ULONG Count,
    _In_ ULONG Selected)
{
    WCHAR Text[BMUI_TEXT_CHARS];
    ULONG Width = BmUiColumns - BMUI_COLUMN_ENTRY - BMUI_ENTRY_RIGHT_MARGIN;
    ULONG i;

    for (i = 0; i < Count; i++)
    {
        RtlStringCchCopyW(Text, RTL_NUMBER_OF(Text), Names[i]);
        BmUiPutText(BMUI_ROW_ENTRIES + i,
                    BMUI_COLUMN_ENTRY,
                    Width,
                    Text,
                    (i == Selected) ? BMUI_ATTR_INVERSE : BMUI_ATTR_NORMAL);
        if (i == Selected)
            BmUiPutText(BMUI_ROW_ENTRIES + i, BMUI_COLUMN_ENTRY + Width - 1, 1, L">", BMUI_ATTR_INVERSE);
    }
}

static
VOID
BmUiDrawDescription(
    _In_opt_ PCSTR Text)
{
    static const WCHAR Label[] = L"Description: ";
    WCHAR Line[BMUI_TEXT_CHARS];
    ULONG Column = BMUI_COLUMN_ENTRY + RTL_NUMBER_OF(Label) - 1;
    ULONG Width = BmUiColumns - min(Column + BMUI_ENTRY_RIGHT_MARGIN, BmUiColumns);
    ULONG Row, Count, Fit, Space, i;

    Width = min(Width, BMUI_TEXT_CHARS - 1);
    for (Row = 0; Row < BMUI_DESCRIPTION_ROWS; Row++)
    {
        Fit = 0;
        while (Text && *Text == ' ')
            Text++;
        if (Text && *Text)
        {
            Space = 0;
            for (Count = 0; Text[Count] && Count < Width; Count++)
            {
                if (Text[Count] == ' ')
                    Space = Count;
            }
            Fit = (Text[Count] == ANSI_NULL || Text[Count] == ' ' || Space == 0) ? Count : Space;
            for (i = 0; i < Fit; i++)
                Line[i] = (UCHAR)Text[i];
            Text += Fit;
        }
        Line[Fit] = UNICODE_NULL;
        BmUiPutText(BMUI_ROW_DESCRIPTION + Row, 0, Column, (Row == 0 && Fit != 0) ? Label : NULL, BMUI_ATTR_NORMAL);
        BmUiPutText(BMUI_ROW_DESCRIPTION + Row, Column, BmUiColumns - min(Column, BmUiColumns), Line, BMUI_ATTR_NORMAL);
    }
}

static
VOID
BmUiDrawFrame(
    _In_ PCWSTR Title,
    _In_ PCWSTR Heading,
    _In_ PCWSTR Escape)
{
    WCHAR Footer[BMUI_TEXT_CHARS];
    ULONG TitleColumn, Length, i;

    if (BmUiGraphics)
        BmUiSetMonoSize();

    BmUiClear(BMUI_BLACK, BMUI_ATTR_NORMAL);
    TitleColumn = (BmUiColumns - min((ULONG)wcslen(Title), BmUiColumns)) / 2;
    BmUiPutRow(BMUI_ROW_TITLE, NULL, BMUI_ATTR_INVERSE);
    BmUiPutText(BMUI_ROW_TITLE, TitleColumn, BmUiColumns - TitleColumn, Title, BMUI_ATTR_INVERSE);
    BmUiPutRow(BMUI_ROW_HEADING, Heading, BMUI_ATTR_BRIGHT);
    BmUiPutRow(BMUI_ROW_HINT, L"(Use the arrow keys to highlight your choice, then press ENTER.)", BMUI_ATTR_NORMAL);

    for (i = 0; i < BmUiColumns; i++)
        Footer[i] = L' ';
    Footer[BmUiColumns] = UNICODE_NULL;
    RtlCopyMemory(Footer, L"ENTER=Choose", sizeof(L"ENTER=Choose") - sizeof(WCHAR));
    Length = (ULONG)wcslen(Escape);
    RtlCopyMemory(&Footer[BmUiColumns - Length], Escape, Length * sizeof(WCHAR));
    BmUiPutRow(BMUI_ROW_FOOTER, Footer, BMUI_ATTR_INVERSE);
}

static
VOID
BmUiMoveSelection(
    _In_ int Key,
    _In_ ULONG Count,
    _Inout_ PULONG Selected)
{
    if (Key == KEY_UP && *Selected > 0)
        (*Selected)--;
    else if (Key == KEY_DOWN && *Selected + 1 < Count)
        (*Selected)++;
    else if (Key == KEY_HOME)
        *Selected = 0;
    else if (Key == KEY_END)
        *Selected = Count - 1;
}

BM_UI_ACTION
BmUiChooseEntry(
    _In_ PCWSTR Title,
    _In_reads_(Count) PCWSTR* Names,
    _In_ ULONG Count,
    _Inout_ PULONG Selected,
    _In_ LONG Timeout)
{
    WCHAR Text[BMUI_TEXT_CHARS];
    BOOLEAN Extended;
    int Key;

    Count = min(Count, BMUI_MAX_ENTRIES);
    if (*Selected >= Count)
        *Selected = 0;

    BmUiDrawFrame(Title, L"Choose an operating system to start:", L"ESC=Exit");
    BmUiPutRow(BMUI_ROW_ADVANCED, L"To choose a startup option for ReliefWin, press F8.", BMUI_ATTR_NORMAL);

    for (;;)
    {
        BmUiDrawEntries(Names, Count, *Selected);

        if (Timeout >= 0)
        {
            RtlStringCchPrintfW(Text,
                                RTL_NUMBER_OF(Text),
                                L"Seconds until the highlighted choice will be started automatically: %ld",
                                Timeout);
            BmUiPutRow(BMUI_ROW_COUNTDOWN, Text, BMUI_ATTR_NORMAL);
        }

        BmUiPresent(0, BmUiHeight);

        if (Timeout == 0)
            return BmUiActionStart;

        Key = BmUiReadKey(&Timeout, &Extended);
        if (Key < 0)
            continue;

        if (Timeout >= 0)
        {
            Timeout = -1;
            BmUiPutRow(BMUI_ROW_COUNTDOWN, NULL, BMUI_ATTR_NORMAL);
        }

        if (Key == KEY_ENTER)
            return BmUiActionStart;

        if (Key == KEY_ESC)
            return BmUiActionExit;

        if (!Extended)
            continue;

        if (Key == KEY_F8)
            return BmUiActionAdvanced;

        BmUiMoveSelection(Key, Count, Selected);
    }
}

BOOLEAN
BmUiStartupSettings(
    _In_reads_(Count) PCSTR* Names,
    _In_reads_(Count) PCSTR* Descriptions,
    _In_ ULONG Count,
    _Out_ PULONG Choice)
{
    WCHAR Text[BMUI_MAX_ENTRIES][BMUI_TEXT_CHARS];
    PCWSTR Lines[BMUI_MAX_ENTRIES];
    BOOLEAN Extended;
    ULONG i, j;
    int Key;

    Count = min(Count, BMUI_MAX_ENTRIES);
    for (i = 0; i < Count; i++)
    {
        for (j = 0; Names[i][j] != ANSI_NULL && j < BMUI_TEXT_CHARS - 1; j++)
            Text[i][j] = (UCHAR)Names[i][j];
        Text[i][j] = UNICODE_NULL;
        Lines[i] = Text[i];
    }

    *Choice = 0;
    BmUiDrawFrame(L"Startup options", L"Choose a startup option:", L"ESC=Back");

    for (;;)
    {
        BmUiDrawEntries(Lines, Count, *Choice);
        BmUiDrawDescription(Descriptions[*Choice]);
        BmUiPresent(0, BmUiHeight);

        Key = BmUiReadKey(NULL, &Extended);

        if (!Extended && Key == KEY_ENTER)
            return TRUE;

        if (!Extended && Key == KEY_ESC)
            return FALSE;

        if (Extended)
            BmUiMoveSelection(Key, Count, Choice);
    }
}
