#!/usr/bin/env python3
"""
PROJECT:     ReliefWin kernel host tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Verify boot-logo framebuffer handoff without previous-stage residue
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
from test_boot_brand import ROOT, function


def pixel_parity():
    kernel = (ROOT / "ntos/inbv/inbvgop.c").read_text()
    loader = (ROOT / "bootloader/gui/bmui.c").read_text()
    asset = (ROOT / "ntos/inbv/resources/reliefwin_logo.bmp").read_bytes()
    assert asset[:2] == b"BM"
    assert struct.unpack_from("<IiiHHI", asset, 14) == (108, 652, 120, 1, 32, 3)
    assert struct.unpack_from("<IIII", asset, 54) == (0xff0000, 0xff00, 0xff, 0xff000000)
    assert struct.unpack_from("<I", asset, 10)[0] == 122
    assert len(asset) == 122 + 652 * 120 * 4
    assert "ntos/inbv/resources/reliefwin_logo.bmp" in (ROOT / "bootloader/CMakeLists.txt").read_text()
    harness = r'''
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _In_
#define _In_opt_
#define _Out_
#define _In_reads_bytes_(n)
#define _In_reads_opt_(n)
#define VOID void
#define TRUE 1
#define FALSE 0
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define RtlCopyMemory(p,q,n) memcpy(p,q,n)
#define MINLONG INT32_MIN
#define BI_RGB 0
#define BI_BITFIELDS 3
#define IDB_GOP_LOGO 23
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT 96
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_MIN 96
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_MAX 480
typedef uint32_t ULONG, *PULONG;
typedef uint64_t ULONGLONG;
typedef int32_t LONG;
typedef unsigned char UCHAR, *PUCHAR, BOOLEAN;
typedef size_t SIZE_T;
typedef struct { ULONG biSize; LONG biWidth,biHeight; uint16_t biPlanes,biBitCount;
                 ULONG biCompression,biSizeImage; LONG biXPelsPerMeter,biYPelsPerMeter;
                 ULONG biClrUsed,biClrImportant; } BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct { UCHAR rgbBlue,rgbGreen,rgbRed,rgbReserved; } RGBQUAD,*LPRGBQUAD;
typedef struct { ULONG RedMask,GreenMask,BlueMask,Dpi; } LOADER_PARAMETER_FRAMEBUFFER;
#include <reliefwin/bootlogo.h>
typedef RELIEF_BOOT_LOGO_RECT INBV_GOP_RECT,*PINBV_GOP_RECT;
static unsigned Checks;
static ULONG BmUiWidth,BmUiHeight;
static PULONG BmUiPixels,KernelPixels;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"pixel line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static ULONG LeUlong(const UCHAR *Data)
{ return Data[0] | (Data[1]<<8) | (Data[2]<<16) | ((ULONG)Data[3]<<24); }
static BOOLEAN InbvGopQueryInfo(LOADER_PARAMETER_FRAMEBUFFER *Info)
{ *Info = (LOADER_PARAMETER_FRAMEBUFFER){0xff0000,0xff00,0xff,96}; return TRUE; }
static VOID VidBufferToScreenBlt(PUCHAR Data,ULONG X,ULONG Y,ULONG Width,ULONG Height,ULONG Stride)
{
    CHECK(X < BmUiWidth && Width <= BmUiWidth-X && Y < BmUiHeight && Height == 1 && Stride == Width*4);
    memcpy(KernelPixels+(SIZE_T)Y*BmUiWidth+X,Data,Stride);
}
'''
    harness += "\nstatic UCHAR bootui_wordmark[] = {" + ",".join(map(str, asset)) + "};\n"
    harness += r'''
static PUCHAR InbvGetResourceAddress(ULONG Id) { CHECK(Id == IDB_GOP_LOGO); return bootui_wordmark+14; }
'''
    harness += re.search(r"typedef struct _INBV_GOP_RGB\b.*?\}\s*[^;]+;", kernel, re.S).group() + "\n"
    for name, result in (
        ("InbvMaskShift", "ULONG"), ("InbvMaskMax", "ULONG"), ("InbvPackColor", "ULONG"),
        ("InbvGopValidateWordmarkHeader", "BOOLEAN"), ("InbvGopComputeWordmarkRect", "BOOLEAN"),
        ("InbvGopExtractChannel", "UCHAR"), ("InbvGopReadWordmarkPixel", "VOID"),
        ("InbvGopDrawWordmark", "BOOLEAN")):
        harness += function(kernel, name, result)
    for name, result in (("BmUiBootLogoPixel", "ULONG"), ("BmUiDrawBootLogo", "VOID")):
        harness += function(loader, name, result)
    harness += r'''
int main(VOID)
{
    static const ULONG Sizes[][2] = {{1,1},{16,16},{640,480},{1280,900},{3840,2160}};
    RELIEF_BOOT_LOGO_RECT Rect;
    CHECK(!ReliefBootLogoGetRect(0,900,652,120,96,&Rect));
    CHECK(!ReliefBootLogoGetRect(1280,0,652,120,96,&Rect));
    CHECK(!ReliefBootLogoGetRect(1280,900,0,120,96,&Rect));
    CHECK(!ReliefBootLogoGetRect(1280,900,652,0,96,&Rect));
    CHECK(!ReliefBootLogoGetRect(1280,900,652,120,96,NULL));
    CHECK(ReliefBootLogoGetRect(1280,900,652,120,96,&Rect));
    CHECK(Rect.X == 509 && Rect.Y == 72 && Rect.Width == 261 && Rect.Height == 48);
    for (unsigned i=0;i<sizeof(Sizes)/sizeof(Sizes[0]);i++)
    {
        SIZE_T Bytes = (SIZE_T)Sizes[i][0]*Sizes[i][1]*4;
        BmUiWidth=Sizes[i][0]; BmUiHeight=Sizes[i][1];
        BmUiPixels=calloc(1,Bytes); KernelPixels=calloc(1,Bytes);
        CHECK(BmUiPixels && KernelPixels);
        BmUiDrawBootLogo();
        CHECK(InbvGopDrawWordmark(BmUiWidth,BmUiHeight,NULL));
        CHECK(!memcmp(BmUiPixels,KernelPixels,Bytes));
        for (ULONG Dpi=0;Dpi<=600;Dpi+=24)
        {
            CHECK(ReliefBootLogoGetRect(BmUiWidth,BmUiHeight,652,120,Dpi,&Rect));
            CHECK(Rect.Width && Rect.Height && Rect.X+Rect.Width<=BmUiWidth && Rect.Y+Rect.Height<=BmUiHeight);
            CHECK(Rect.X==(BmUiWidth-Rect.Width)/2);
        }
        free(BmUiPixels); free(KernelPixels);
    }
    printf("Loader/kernel boot logo: %u checks passed; framebuffers match pixel for pixel at five display sizes\n",Checks);
    return 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    c_file, binary = folder / "boot-logo-pixels.c", folder / "boot-logo-pixels"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    "-I", str(ROOT / "sdk/include"), str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline")
    args = parser.parse_args()
    path = "ntos/inbv/inbvgop.c"
    source = (subprocess.check_output(["git", "show", args.baseline + ":" + path], cwd=ROOT).decode()
              if args.baseline else (ROOT / path).read_text())
    harness = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define VOID void
#define NTAPI
#define TRUE 1
#define FALSE 0
#define BV_COLOR_BLACK 0
#define BV_COLOR_WHITE 15
#define INBV_WORDMARK_TOP_VPOS 0.08f
#define INBV_DISPLAY_STATE_OWNED 1
#define ASSERT(x) assert(x)
typedef uint32_t ULONG;
typedef unsigned char BOOLEAN;
typedef char CHAR;
typedef struct { ULONG Unused; } LOADER_PARAMETER_FRAMEBUFFER;
typedef struct { ULONG Width, Height, PhysicalWidth, PhysicalHeight, CharacterWidth; } VID_DISPLAY_INFO;
static VID_DISPLAY_INFO Display = {1280,900,1280,900,8};
static ULONG Canvas[1280 * 900];
static unsigned Checks, Clears, Logos, Status, Locked, Owned = 1, Installed = 1;
static BOOLEAN Gop = TRUE, Query = TRUE, Resource = TRUE;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static BOOLEAN InbvGopQueryInfo(LOADER_PARAMETER_FRAMEBUFFER *Info) { (void)Info; return Gop; }
static BOOLEAN InbvQueryDisplayInfo(VID_DISPLAY_INFO *Info) { *Info = Display; return Query; }
static BOOLEAN InbvIsBootDriverInstalled(VOID) { return Installed; }
static ULONG InbvGetDisplayState(VOID) { return Owned; }
static VOID InbvAcquireLock(VOID) { CHECK(!Locked); Locked = 1; }
static VOID InbvReleaseLock(VOID) { CHECK(Locked); Locked = 0; }
static VOID VidSolidColorFill(ULONG Left,ULONG Top,ULONG Right,ULONG Bottom,unsigned char Color)
{
    CHECK(Locked && Left == 0 && Top == 0 && Right == Display.Width-1 && Bottom == Display.Height-1);
    CHECK(Color == BV_COLOR_BLACK);
    memset(Canvas,0,sizeof(Canvas)); Clears++;
}
static VOID InbvSetTextColor(ULONG Color) { CHECK(Color == BV_COLOR_WHITE); }
static BOOLEAN InbvGopDrawWordmark(ULONG Width,ULONG Height,const VOID *Rect)
{
    CHECK(Width == Display.PhysicalWidth && Height == Display.PhysicalHeight && !Rect);
    if (!Resource) return FALSE;
    Canvas[72 * 1280 + 640] = 0x123456; Logos++; return TRUE;
}
static VOID InbvInitializeBootStatus(VOID)
{
    CHECK(!Locked); Canvas[0] = 0xffffff; Canvas[700 * 1280 + 640] = 0xffffff; Status++;
}
static VOID VidDisplayStringXY(const CHAR *Text,ULONG X,ULONG Y,BOOLEAN Clear)
{ CHECK(!strcmp(Text,"ReliefWin") && X < Display.Width && Y < Display.Height && Clear); }
static VOID InbvEnableDisplayString(BOOLEAN Enabled) { CHECK(!Enabled); }
'''
    harness += function(source, "InbvGopHandleBootBitmap", "BOOLEAN")
    harness += r'''
int main(VOID)
{
    for (unsigned Pass = 0; Pass < 2; Pass++)
    {
        for (unsigned i = 0; i < 1280 * 900; i++) Canvas[i] = 0xfeed;
        CHECK(InbvGopHandleBootBitmap());
        CHECK(Canvas[245 * 1280 + 640] == BV_COLOR_BLACK);
        CHECK(Canvas[72 * 1280 + 640] == 0x123456 && Canvas[0] == 0xffffff);
        CHECK(Canvas[700 * 1280 + 640] == 0xffffff && !Locked);
    }
    Resource = FALSE;
    CHECK(InbvGopHandleBootBitmap() && !Locked);
    CHECK(Canvas[72 * 1280 + 640] == BV_COLOR_BLACK);
    CHECK(Clears == 3 && Logos == 2 && Status == 3);
    Gop = FALSE; CHECK(!InbvGopHandleBootBitmap() && Clears == 3);
    Gop = TRUE; Query = FALSE; CHECK(!InbvGopHandleBootBitmap() && Clears == 3);
    Query = TRUE; Display.PhysicalWidth = 0; CHECK(!InbvGopHandleBootBitmap() && Clears == 3);
    Display.PhysicalWidth = 1280; Display.Width = 0; CHECK(!InbvGopHandleBootBitmap() && Clears == 3);
    Display.Width = 1280; Installed = 0; CHECK(!InbvGopHandleBootBitmap() && Clears == 3 && !Locked);
    Installed = 1; Owned = 0; CHECK(!InbvGopHandleBootBitmap() && Clears == 3 && !Locked);
    printf("Boot logo handoff: %u checks passed\n",Checks);
    return 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    c_file, binary = folder / "boot-logo.c", folder / "boot-logo"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-Wno-unused-function",
                    "-O2", str(c_file), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    pixel_parity()


if __name__ == "__main__":
    main()
