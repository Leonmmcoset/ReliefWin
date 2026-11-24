#!/usr/bin/env python3
"""
PROJECT:     ReliefWin kernel host tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise the boot progress renderer's C geometry across supported DPI
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def function(source, name, result):
    start = source.index(name + "(")
    body = source.index("{", start)
    depth = 1
    end = body + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return "static " + result + " " + source[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", action="store_true")
    args = parser.parse_args()
    path = "ntos/inbv/inbvgop.c"
    source = (subprocess.check_output(["git", "show", "HEAD:" + path], cwd=ROOT).decode()
              if args.baseline else (ROOT / path).read_text())
    declarations = "\n".join(re.findall(r"^#define INBV_LOGO_.*$", source, re.M))
    for name in ("INBV_LOGO_RECT", "INBV_LOGO_STATE"):
        declarations += "\n" + re.search(r"typedef struct _" + name + r"\b.*?\}\s*[^;]+;",
                                          source, re.S).group()
    declarations += "\n" + re.search(r"static const INBV_LOGO_RECT InbvLogoShape.*?\n\};",
                                      source, re.S).group()
    fragment = source[source.index("    S->Width = InbvGopScaleForDpi(INBV_LOGO_WIDTH"):
                      source.index("    S->Left = (ScreenWidth - S->Width) / 2;")]
    functions = "".join(function(source, name, result) for name, result in (
        ("InbvGopNormalizeDpi", "ULONG"), ("InbvGopScaleForDpi", "ULONG"),
        ("InbvGopLogoOverlap", "LONG"), ("InbvGopLogoCover", "VOID"),
        ("InbvGopLogoRasterize", "VOID"), ("InbvGopLogoLinePoint", "VOID"),
        ("InbvGopLogoLineRects", "ULONG"), ("InbvGopLogoScale", "LONG")))
    harness = r'''
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define _In_
#define _Out_
#define _In_reads_(n)
#define _Inout_updates_(n)
#define VOID void
#define TRUE 1
#define FALSE 0
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define RTL_NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_DEFAULT 96
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_MIN 96
#define LOADER_PARAMETER_FRAMEBUFFER_DPI_MAX 480
typedef uint32_t ULONG, *PULONG;
typedef uint64_t ULONGLONG;
typedef int32_t LONG, *PLONG;
typedef size_t SIZE_T;
static unsigned Checks;
#define CHECK(x) do { ++Checks; if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
'''
    harness += declarations + "\nstatic INBV_LOGO_STATE g_Logo;\n" + functions
    harness += r'''
static int setup(ULONG ScreenWidth, ULONG ScreenHeight, ULONG Dpi)
{
    INBV_LOGO_STATE *S = &g_Logo;
    struct { ULONG Dpi; } FbInfo = { Dpi };
    ULONG Arm, Part;
    LONG StemLength, Total, Reach, Head;
'''
    harness += fragment + "    return TRUE;\n}\n"
    harness += r'''
int main(void)
{
    ULONG dpi, i, progress, x, y;
    INBV_LOGO_STATE *S = &g_Logo;
    INBV_LOGO_RECT rects[INBV_LOGO_MAX_RECTS];
    for (dpi = 96; dpi <= 480; ++dpi)
    {
        ULONG width = (120 * dpi + 48) / 96;
        ULONG count;
        uint64_t last = 0;
        uint32_t *allocation;
        LONG tail;
        *S = (INBV_LOGO_STATE){0};
        CHECK(setup(800, 600, dpi));
        CHECK(S->Width == width);
        CHECK(S->Height > 0 && S->Height <= S->Width / 10);
        CHECK(S->Perimeter > 0 && S->LineLength > 0);
        CHECK(S->FillEnd <= INBV_LOGO_PROGRESS_MAX);
        count = S->Width * S->Height;
        allocation = calloc(count + 2, sizeof(*allocation));
        CHECK(allocation != NULL);
        allocation[0] = allocation[count + 1] = 0xfeedface;
        S->Frame = allocation + 1;
        for (i = 0; i < RTL_NUMBER_OF(S->Lut); ++i)
            S->Lut[i] = i;
        for (progress = 0; progress <= 10000; progress += 100)
        {
            uint64_t sum = 0;
            InbvGopLogoRasterize(progress);
            CHECK(allocation[0] == 0xfeedface && allocation[count + 1] == 0xfeedface);
            for (i = 0; i < count; ++i)
            {
                CHECK(S->Frame[i] <= 255);
                sum += S->Frame[i];
            }
            CHECK(sum >= last);
            last = sum;
        }
        for (y = 0; y < S->Height; ++y)
            for (x = 0; x < S->Width; ++x)
                CHECK(S->Frame[y * S->Width + x] == 255);
        for (tail = 0; tail < S->Perimeter; tail += max(S->Perimeter / 99, 1))
        {
            ULONG found = InbvGopLogoLineRects(tail, rects, 0);
            found = InbvGopLogoLineRects((tail + S->Perimeter / 2) % S->Perimeter, rects, found);
            CHECK(found <= INBV_LOGO_MAX_RECTS);
            InbvGopLogoCover(rects, found);
            CHECK(allocation[0] == 0xfeedface && allocation[count + 1] == 0xfeedface);
        }
        free(allocation);
        *S = (INBV_LOGO_STATE){0};
        CHECK(!setup(width - 1, 600, dpi));
        *S = (INBV_LOGO_STATE){0};
        CHECK(!setup(800, 0, dpi));
    }
    CHECK(InbvGopScaleForDpi(120, 95) == 120);
    CHECK(InbvGopScaleForDpi(120, 481) == 120);
    printf("Boot brand: %u checks passed against the kernel's C rasterization functions\n", Checks);
    return 0;
}
'''
    folder = Path(tempfile.gettempdir()) / "reliefwin-brand-host"
    folder.mkdir(exist_ok=True)
    c_file, binary = folder / "boot-brand.c", folder / "boot-brand"
    c_file.write_text(harness)
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                    str(c_file), "-o", str(binary)], check=True)
    result = subprocess.run([str(binary)], capture_output=True, text=True)
    print(result.stdout, end="")
    if args.baseline:
        if result.returncode != 1 or "S->Width == width" not in result.stderr:
            raise SystemExit("The original animation did not fail for the expected obsolete brand geometry: " + result.stderr)
        print("The original animation failed the ReliefWin progress-bar regression as expected")
    else:
        print(result.stderr, end="")
        result.check_returncode()


if __name__ == "__main__":
    main()
