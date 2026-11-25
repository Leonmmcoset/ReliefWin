#!/usr/bin/env python3
"""
PROJECT:     ReliefWin kernel host tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise actual boot progress drawing, component text and display teardown
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def function(source, name, result):
    start = 0
    while True:
        start = source.index(name + "(", start)
        body = source.index("{", start)
        semicolon = source.find(";", start, body)
        if semicolon == -1:
            break
        start += len(name)
    depth = 1
    end = body + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static " + result + " " + source[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", metavar="REF")
    args = parser.parse_args()
    path = "ntos/inbv/inbvgop.c"
    source = (subprocess.check_output(["git", "show", args.baseline + ":" + path], cwd=ROOT).decode()
              if args.baseline else (ROOT / path).read_text())
    if args.baseline and "InbvDrawBootProgressLocked(" not in source:
        print("Baseline rejects the new behavior: it has the obsolete lower animation and no top progress renderer")
        return
    state = re.search(r"typedef struct _INBV_STATUS_LINE\b.*?\}\s*[^;]+;", source, re.S).group()
    functions = "".join(function(source, name, result) for name, result in (
        ("InbvGopFillStatusBand", "VOID"), ("InbvGopStatusFree", "VOID"),
        ("InbvMaskShift", "ULONG"), ("InbvMaskMax", "ULONG"), ("InbvPackColor", "ULONG"),
        ("InbvDrawBootProgressLocked", "VOID"), ("InbvInitializeBootStatus", "VOID"),
        ("InbvFormatBootComponent", "ULONG"), ("InbvDisplayStatusLocked", "VOID"),
        ("InbvBootProgress", "VOID"), ("InbvGopDisplayStatus", "BOOLEAN"),
        ("InbvGopClearStatus", "VOID")))
    harness = r'''
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define _In_
#define _In_opt_
#define _Out_writes_(n)
#define _In_reads_(n)
#define VOID void
#define NTAPI
#define CODE_SEG(x)
#define TRUE 1
#define FALSE 0
#define ANSI_NULL '\0'
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define RTL_NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))
#define RtlZeroMemory(p,n) memset(p,0,n)
#define RtlCopyMemory(p,q,n) memcpy(p,q,n)
#define ASSERT(x) assert(x)
#define MAXULONG UINT32_MAX
#define MAXULONG_PTR SIZE_MAX
#define INBV_LOGO_POOL_TAG 0
#define INBV_BOOT_PROGRESS_MAX 10000
#define INBV_BOOT_PROGRESS_START 5000
#define INBV_DISPLAY_STATE_OWNED 1
#define BV_COLOR_BLACK 0
#define BV_COLOR_DARK_GRAY 7
#define BV_COLOR_WHITE 15
#define NonPagedPool 0
typedef uint32_t ULONG, *PULONG;
typedef uint64_t ULONGLONG;
typedef unsigned char UCHAR, *PUCHAR, BOOLEAN;
typedef char CHAR, *PCHAR;
typedef const char *PCSTR;
typedef size_t SIZE_T;
typedef struct { ULONG Width, Height, CharacterWidth, CharacterHeight, PhysicalWidth, PhysicalHeight; } VID_DISPLAY_INFO;
typedef struct { ULONG RedMask, GreenMask, BlueMask; } LOADER_PARAMETER_FRAMEBUFFER;
static unsigned Checks, Locked, Owned = 1, Installed = 1, HaveGop, Render, Allocations, Frees, FailAllocation;
static unsigned Fills, Blits;
static VID_DISPLAY_INFO Display;
static ULONG *Canvas;
static CHAR LastText[128];
static ULONG TextLeft, TextTop;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
'''
    harness += state + r'''
static INBV_STATUS_LINE g_Status;
static ULONG InbvBootProgressValue = INBV_BOOT_PROGRESS_START;
static VOID InbvBootProgress(PCSTR, ULONG);
static VOID InbvDisplayStatusLocked(PCSTR);
static VOID InbvGopDisplayFilter(PCSTR *Text) { (void)Text; }
static VOID InbvInstallDisplayStringFilter(VOID (*Filter)(PCSTR *)) { CHECK(Filter != NULL); }
static VOID InbvAcquireLock(VOID) { CHECK(Locked == 0); Locked = 1; }
static VOID InbvReleaseLock(VOID) { CHECK(Locked == 1); Locked = 0; }
static BOOLEAN InbvIsBootDriverInstalled(VOID) { return Installed; }
static ULONG InbvGetDisplayState(VOID) { return Owned; }
static BOOLEAN InbvQueryDisplayInfo(VID_DISPLAY_INFO *Info) { *Info = Display; return TRUE; }
static BOOLEAN InbvGopQueryInfo(LOADER_PARAMETER_FRAMEBUFFER *Info)
{ *Info = (LOADER_PARAMETER_FRAMEBUFFER){0xff0000,0xff00,0xff}; return HaveGop; }
static VOID *ExAllocatePoolWithTag(ULONG Pool, SIZE_T Size, ULONG Tag)
{
    (void)Pool; (void)Tag;
    if (FailAllocation && --FailAllocation == 0) return NULL;
    Allocations++;
    return calloc(1, Size);
}
static VOID ExFreePoolWithTag(VOID *Pointer, ULONG Tag)
{ (void)Tag; CHECK(!Locked); Frees++; free(Pointer); }
static VOID VidSolidColorFill(ULONG Left, ULONG Top, ULONG Right, ULONG Bottom, ULONG Color)
{
    ULONG x, y;
    CHECK(Locked && Owned && Installed);
    CHECK(Left <= Right && Right < Display.Width && Top <= Bottom && Bottom < Display.Height);
    Fills++;
    for (y = Top; y <= Bottom; y++)
        for (x = Left; x <= Right; x++) Canvas[(SIZE_T)y * Display.Width + x] = Color;
}
static VOID VidSetScrollRegion(ULONG Left, ULONG Top, ULONG Right, ULONG Bottom)
{
    CHECK(Locked && Left <= Right && Right < Display.Width && Bottom < Display.Height);
    TextLeft = Left; TextTop = Top;
}
static VOID VidDisplayString(PCSTR Text)
{
    CHECK(Locked && strlen(Text) < sizeof(LastText));
    strcpy(LastText, Text);
    Canvas[(SIZE_T)TextTop * Display.Width + TextLeft] = BV_COLOR_WHITE;
}
static BOOLEAN VidRenderStatusText(PCSTR Text, PUCHAR Coverage, ULONG Width, ULONG Height)
{
    CHECK(Locked && strlen(Text) < sizeof(LastText));
    strcpy(LastText, Text);
    if (!Render) return FALSE;
    memset(Coverage, 0, (SIZE_T)Width * Height);
    Coverage[(SIZE_T)(Height / 2) * Width + Width / 2] = 255;
    return TRUE;
}
static VOID VidBufferToScreenBlt(PUCHAR Data, ULONG Left, ULONG Top, ULONG Width, ULONG Height, ULONG Stride)
{
    CHECK(Locked && Owned && Installed && Data != NULL);
    CHECK(Left == 0 && Width == Display.PhysicalWidth && Top < Display.PhysicalHeight && Height == 1);
    CHECK(Stride == Width * sizeof(ULONG));
    Blits++;
}
'''
    harness += functions
    harness += r'''
static VOID Setup(ULONG Width, ULONG Height, ULONG PhysicalWidth, ULONG PhysicalHeight, ULONG CharHeight)
{
    Display = (VID_DISPLAY_INFO){Width, Height, 8, CharHeight, PhysicalWidth, PhysicalHeight};
    Canvas = malloc((SIZE_T)max(Width,1) * max(Height,1) * sizeof(*Canvas));
    CHECK(Canvas != NULL);
    for (SIZE_T i = 0; i < (SIZE_T)Width * Height; i++) Canvas[i] = 0xfeed;
    InbvInitializeBootStatus();
}
static VOID Teardown(VOID)
{
    InbvGopClearStatus();
    InbvGopClearStatus();
    CHECK(!g_Status.Ready && !g_Status.Coverage && !g_Status.Row && !Locked);
    CHECK(Allocations == Frees);
    free(Canvas);
    Canvas = NULL;
}
int main(void)
{
    ULONG Width, Progress, LastFill;
    CHAR Line[130], Name[400];
    memset(Name, 'x', sizeof(Name)); Name[399] = 0;
    for (Width = 0; Width <= 300; Width++)
    {
        ULONG Length;
        memset(Line, 0x7f, sizeof(Line));
        Length = InbvFormatBootComponent(Name, Width, Line + 1);
        CHECK(Line[0] == 0x7f && Line[129] == 0x7f && Length <= min(Width,127));
        CHECK(Line[1 + Length] == 0);
        if (Width >= 10)
        {
            CHECK(!strncmp(Line + 1, "Loading ", 8));
            CHECK((UCHAR)Line[Length] == 0x85);
        }
        else CHECK(Length == 0);
    }
    CHECK(InbvFormatBootComponent(NULL,127,Line) == 0);
    CHECK(InbvFormatBootComponent("",127,Line) == 0);
    CHECK(InbvFormatBootComponent("ntfs.sys",127,Line) == 17 && !strcmp(Line,"Loading ntfs.sys\x85"));
    for (Width = 1; Width <= 2048; Width += 17)
    {
        HaveGop = 0; Setup(Width,480,Width,480,16);
        CHECK(g_Status.Ready && g_Status.Top > 300 && g_Status.Top < 400);
        LastFill = 0;
        InbvBootProgressValue = 0;
        for (Progress = 0; Progress <= 10000; Progress += 125)
        {
            ULONG Filled = 0;
            InbvBootProgress(NULL,Progress);
            for (ULONG x = 0; x < Width; x++)
            {
                if (Canvas[x] == BV_COLOR_WHITE) Filled++;
                else CHECK(Canvas[x] == BV_COLOR_DARK_GRAY);
                CHECK(Canvas[3 * Width + x] == Canvas[x]);
            }
            CHECK(Filled == (ULONG)((uint64_t)Width * Progress / 10000) && Filled >= LastFill);
            LastFill = Filled;
        }
        InbvBootProgress(NULL,10001);
        CHECK(InbvBootProgressValue == 10000);
        InbvBootProgress(NULL,0);
        CHECK(InbvBootProgressValue == 10000);
        InbvBootProgress(NULL,MAXULONG);
        CHECK(InbvBootProgressValue == 10000);
        Teardown();
    }
    Setup(800,600,800,600,16);
    InbvBootProgress(Name,7000);
    CHECK(!strncmp(LastText,"Loading ",8) && LastText[strlen(LastText)-1] == '.');
    InbvBootProgress("hal.dll",7000);
    CHECK(!strcmp(LastText,"Loading hal.dll.") && TextLeft > 300 && TextTop == g_Status.Top);
    CHECK(Canvas[(SIZE_T)g_Status.Top * Display.Width] == BV_COLOR_BLACK);
    CHECK(InbvGopDisplayStatus("first\r\n  Debug key 123  \r\n") && !strcmp(LastText,"Debug key 123"));
    Owned = 0; unsigned Previous = Fills;
    InbvBootProgress("must not draw",9000);
    CHECK(!InbvGopDisplayStatus("must not draw") && Fills == Previous);
    Owned = 1; Installed = 0;
    InbvBootProgress("must not draw",9000);
    CHECK(Fills == Previous);
    Installed = 1; Teardown();
    for (ULONG Scale = 1; Scale <= 5; Scale++)
    {
        HaveGop = Render = 1;
        Setup(640,480,640 * Scale,480 * Scale,16);
        CHECK(g_Status.PixelTop == g_Status.Top * Scale && g_Status.PixelHeight == 16 * Scale);
        Previous = Blits;
        InbvBootProgress("USBXHCI.SYS",8000);
        CHECK(Blits == Previous + g_Status.PixelHeight && !strcmp(LastText,"Loading USBXHCI.SYS\x85"));
        Teardown();
    }
    for (ULONG Failure = 1; Failure <= 2; Failure++)
    {
        FailAllocation = Failure; Setup(640,480,640,480,16);
        CHECK(g_Status.Ready && !g_Status.Coverage && !g_Status.Row);
        InbvBootProgress("acpi.sys",7500);
        CHECK(!strcmp(LastText,"Loading acpi.sys."));
        Teardown();
    }
    HaveGop = 0;
    Setup(0,480,0,480,16); CHECK(!g_Status.Ready); Teardown();
    Setup(640,16,640,16,16); CHECK(!g_Status.Ready); Teardown();
    printf("Boot progress: %u checks passed against actual kernel drawing, formatting and teardown functions\n",Checks);
    return 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    c_file, binary = folder / "boot-brand.c", folder / "boot-brand"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)

    loader = (ROOT / "bootloader/gui/bmui.c").read_text()
    loader_functions = "".join(function(loader, name, result) for name, result in (
        ("BmUiFill", "VOID"), ("BmUiBootProgress", "BOOLEAN"),
        ("BmUiBeginBoot", "VOID"), ("BmUiEndBoot", "VOID")))
    loader_harness = r'''
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _In_
#define _In_opt_
#define VOID void
#define TRUE 1
#define FALSE 0
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define RTL_NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))
#define RtlCopyMemory(p,q,n) memcpy(p,q,n)
#define MAXULONG UINT32_MAX
#define BMUI_BLACK 0
#define BMUI_WHITE 0xffffff
#define BMUI_ATTR_NORMAL 7
#define BMUI_ATTR_BRIGHT 15
#define BMUI_TEXT_COLUMNS 80
#define UNICODE_NULL 0
typedef uint32_t ULONG, *PULONG;
typedef uint64_t ULONGLONG;
typedef int32_t LONG;
typedef uint16_t WCHAR;
typedef unsigned char UCHAR, BOOLEAN;
typedef size_t SIZE_T;
typedef const char *PCSTR;
static unsigned Checks, Presents, Freed, Graphics = 1;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"loader line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static ULONG BmUiWidth, BmUiHeight, BmUiCellHeight, BmUiCellWidth, BmUiColumns;
static SIZE_T BmUiPixelBytes;
static PULONG BmUiPixels, Allocation;
static ULONG BmUiBootProgressValue;
static unsigned BmUiBootActive, BmUiGraphics;
static WCHAR LastLine[128];
static LONG TextX, TextBaseline;
static struct { struct { struct { LONG ascender, descender; } metrics; } *size; } Mono;
static __typeof__(&Mono) BmUiMono = &Mono;
static VOID BmUiFill(ULONG,ULONG,ULONG,ULONG,ULONG);
static VOID BmUiInitialize(VOID)
{
    BmUiColumns = Graphics ? 100 : 80;
    BmUiCellWidth = BmUiWidth / BmUiColumns;
    BmUiCellHeight = BmUiHeight / 25;
    BmUiGraphics = Graphics && BmUiCellWidth && BmUiCellHeight;
    if (BmUiGraphics)
    {
        BmUiPixelBytes = (SIZE_T)BmUiWidth * BmUiHeight * sizeof(ULONG);
        Allocation = calloc(1,BmUiPixelBytes + 2 * sizeof(ULONG));
        CHECK(Allocation != NULL);
        Allocation[0] = Allocation[BmUiPixelBytes / sizeof(ULONG) + 1] = 0xfeedface;
        BmUiPixels = Allocation + 1;
    }
    else BmUiPixels = NULL;
}
static VOID BmUiClear(ULONG Color,ULONG Attribute)
{ (void)Attribute; if (BmUiGraphics) BmUiFill(0,0,BmUiWidth,BmUiHeight,Color); }
static VOID BmUiPresent(ULONG Top,ULONG Height)
{ CHECK(BmUiGraphics && Top < BmUiHeight && Height <= BmUiHeight - Top); Presents++; }
static VOID BmUiSetMonoSize(VOID) {}
static VOID BmUiDrawText(VOID *Font,LONG X,LONG Baseline,const WCHAR *Text,ULONG Color)
{
    (void)Font; CHECK(Color == BMUI_WHITE && X >= 0 && (!BmUiGraphics || Baseline < (LONG)BmUiHeight));
    TextX = X; TextBaseline = Baseline;
    for (ULONG i = 0; i < RTL_NUMBER_OF(LastLine); i++)
    { LastLine[i] = Text[i]; if (!Text[i]) break; }
}
static VOID BmUiPutText(ULONG Row,ULONG Column,ULONG Width,const WCHAR *Text,ULONG Attribute)
{
    CHECK(Row == 19 && Column < 80 && Width == 80);
    if (Attribute == BMUI_ATTR_BRIGHT)
        BmUiDrawText(NULL,Column,Row,Text,BMUI_WHITE);
}
static VOID FwFreePages(VOID *Pointer,SIZE_T Size)
{
    CHECK(Pointer == Allocation + 1 && Size == BmUiPixelBytes);
    CHECK(Allocation[0] == 0xfeedface && Allocation[Size / sizeof(ULONG) + 1] == 0xfeedface);
    Freed++; free(Allocation); Allocation = NULL;
}
'''
    loader_harness += "\nstatic UCHAR bootui_logo[] = {" + ",".join(
        str(value) for value in (ROOT / "bootloader/gui/assets/logo.rgba").read_bytes()) + "};\n"
    loader_harness += loader_functions
    loader_harness += r'''
static int Equals(const WCHAR *Wide,const char *Narrow)
{ while (*Narrow) { unsigned Char = (UCHAR)*Narrow++; if (Char == 0x85) Char = 0x2026; if (*Wide++ != Char) return 0; } return *Wide == 0; }
int main(VOID)
{
    static struct { struct { LONG ascender,descender; } metrics; } Size = {{10 * 64,-3 * 64}};
    Mono.size = (void *)&Size;
    for (ULONG Scale = 1; Scale <= 5; Scale++)
    {
        BmUiWidth = 640 * Scale; BmUiHeight = 480 * Scale;
        BmUiBeginBoot(); CHECK(BmUiBootActive && BmUiGraphics);
        CHECK(Equals(LastLine,"Loading winload.exe\x85"));
        for (ULONG Progress = 0; Progress <= 10000; Progress += 125)
        {
            ULONG Count = 0;
            BmUiBootProgress("ntfs.sys",Progress);
            for (ULONG x = 0; x < BmUiWidth; x++) if (BmUiPixels[x] == BMUI_WHITE) Count++;
            CHECK(Count == (ULONG)((ULONGLONG)BmUiWidth * Progress / 10000));
            CHECK(Equals(LastLine,"Loading ntfs.sys\x85") && TextX > 0);
        }
        BmUiBootProgress(NULL,0); CHECK(BmUiBootProgressValue == 10000);
        BmUiBootProgress(NULL,MAXULONG); CHECK(BmUiBootProgressValue == 10000);
        unsigned Previous = Presents;
        BmUiEndBoot(); BmUiEndBoot(); BmUiBootProgress("must not draw",10000);
        CHECK(!BmUiBootActive && !BmUiPixels && !BmUiGraphics && Presents == Previous);
    }
    CHECK(Freed == 5);
    Graphics = 0; BmUiWidth = BmUiHeight = 0;
    BmUiBeginBoot(); CHECK(!BmUiGraphics && Equals(LastLine,"Loading winload.exe."));
    BmUiBootProgress("acpi.sys",5000); CHECK(Equals(LastLine,"Loading acpi.sys."));
    BmUiEndBoot(); CHECK(Freed == 5);
    printf("Loader boot progress: %u checks passed against actual drawing and page teardown functions\n",Checks);
    return 0;
}
'''
    c_file, binary = folder / "loader-boot-brand.c", folder / "loader-boot-brand"
    c_file.write_text(loader_harness)
    subprocess.run(["cc", "-std=c11", "-fshort-wchar", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


    font_source = (ROOT / "sdk/lib/bootfont/bootfont.c").read_text()
    font_harness = r'''
#include <stdint.h>
#include <stdio.h>
#define _In_
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FALSE 0
#define FT_LOAD_TARGET_MONO 1
#define FT_LOAD_FORCE_AUTOHINT 2
#define FT_LOAD_TARGET_LIGHT 4
#define FT_LOAD_DEFAULT 8
#define FT_LOAD_NO_AUTOHINT 16
#define FT_LOAD_NO_BITMAP 32
#define FT_Err_Ok 0
#define BOOT_FONT_STYLE_MONO 1
#define BOOT_FONT_STYLE_LIGHT 2
typedef unsigned short WCHAR;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef int BOOLEAN, FT_Int32;
typedef unsigned FT_UInt;
typedef void *FT_Face;
static WCHAR Requested;
static FT_UInt FT_Get_Char_Index(FT_Face Face,WCHAR Unicode) { (void)Face;Requested=Unicode;return 1; }
static int FT_Load_Glyph(FT_Face Face,FT_UInt Index,FT_Int32 Flags) { (void)Face;(void)Index;(void)Flags;return 0; }
'''
    for name in ("BootFontCp437ControlMap", "BootFontCp437UpperHalfMap"):
        font_harness += re.search(r"static const WCHAR " + name + r".*?\n\};", font_source, re.S).group() + "\n"
    font_harness += function(font_source, "BootFontCp437ToUnicode", "WCHAR")
    font_harness += function(font_source, "BootFontLoadGlyph", "BOOLEAN")
    font_harness += r'''
int main(void)
{
    for (unsigned Character=0;Character<256;Character++)
        for (unsigned Style=0;Style<3;Style++)
        {
            if (!BootFontLoadGlyph(0,(UCHAR)Character,Style)) return 1;
            WCHAR Expected=Style==2&&Character==0x85?0x2026:BootFontCp437ToUnicode((UCHAR)Character);
            if (Requested!=Expected) return 2;
        }
    puts("Boot font: 768 glyph mapping checks passed; status ellipsis is U+2026 and other mappings are preserved");
    return 0;
}
'''
    c_file, binary = folder / "boot-status-font.c", folder / "boot-status-font"
    c_file.write_text(font_harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    main()
