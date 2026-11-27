# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify graphical unlock cancellation and initialization failure
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
source = (args.source or root / 'bmui.c').read_text()
password = re.search(r'BOOLEAN\nBmUiCryptPassword\(.*?\n\}', source, re.S).group(0)
failure = re.search(r'VOID\nBmUiCryptFailure\(.*?\n\}', source, re.S).group(0)
has_cancel_output = 'PBOOLEAN Cancelled' in password
fixture = r'''
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <wchar.h>
typedef int BOOLEAN, *PBOOLEAN;
typedef unsigned ULONG, *PULONG;
typedef const char *PCSTR;
typedef char *PCHAR;
#define VOID void
#define TRUE 1
#define FALSE 0
#define KEY_EXTENDED 0
#define KEY_ENTER 13
#define KEY_ESC 27
#define MAXULONG UINT_MAX
#define BMUI_POLL_MICROSECONDS 10000
#define RTL_NUMBER_OF(A) (sizeof(A) / sizeof((A)[0]))
typedef struct {unsigned X, Y; int Absolute, LeftButton;} MACH_POINTER_STATE;
static int Available, Message, Reads, Draws;
static const char *Keys;
static unsigned BmUiWidth = 1280, BmUiHeight = 900, BmUiCryptLanguage;
static BOOLEAN BmUiCryptRecovery;
const unsigned BmCryptText[3] = {0};
static BOOLEAN BmUiCryptStart(void) {return Available;}
static void FwDebugPrint(const char *Text) {(void)Text;}
static int FwKeyAvailable(void) {return 1;}
static int FwReadKey(void) {++Reads; assert(Keys && *Keys); return *Keys++;}
static int FwGetPointerState(unsigned W, unsigned H, MACH_POINTER_STATE *P)
{(void)W; (void)H; (void)P; return 0;}
static unsigned BmUiCryptHit(unsigned X, unsigned Y) {(void)X; (void)Y; return 0;}
static void BmUiCryptFrame(BM_CRYPT_EDIT *E, PCSTR Id, unsigned S, int C, MACH_POINTER_STATE *P, int Show)
{(void)E; (void)Id; (void)S; (void)C; (void)P; (void)Show; ++Draws;}
static void StallExecutionProcessor(unsigned T) {(void)T;}
static void FwPrint(const wchar_t *Text) {(void)Text; Message = 1;}
static void BmUiShutdown(void) {Message = 0;}
'''
wrapper = r'''
static int Dialog(char *Secret, unsigned *Length, int *Cancelled)
{
'''
if has_cancel_output:
    wrapper += 'return BmUiCryptPassword("test", 0, Secret, 129, Length, Cancelled);\n}\n'
else:
    wrapper += r'''
int Result = BmUiCryptPassword("test", 0, Secret, 129, Length);
*Cancelled = !Result;
return Result;
}
'''
tests = r'''
static unsigned Checks, Failures;
#define CHECK(C) do {++Checks; if (!(C)) {++Failures; printf("FAIL %u: %s\n", __LINE__, #C);}} while (0)
int main(void)
{
    char Secret[129];
    unsigned Length = 129, Index;
    int Cancelled = 1;
    memset(Secret, 'x', sizeof(Secret));
    CHECK(!Dialog(Secret, &Length, &Cancelled));
    CHECK(!Cancelled && !Length && !Reads && !Draws);
    for (Index = 0; Index < sizeof(Secret); ++Index) CHECK(!Secret[Index]);
    BmUiCryptFailure("test", 0);
    CHECK(Message);
    Available = 1; Keys = "some-secret\033";
    CHECK(!Dialog(Secret, &Length, &Cancelled) && Cancelled && !Length);
    for (Index = 0; Index < sizeof(Secret); ++Index) CHECK(!Secret[Index]);
    Keys = "test-password\r";
    CHECK(Dialog(Secret, &Length, &Cancelled) && !Cancelled);
    CHECK(Length == 13 && !strcmp(Secret, "test-password"));
    printf("%u graphical unlock dialog checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
'''
program = Path('/tmp/reliefwin-cryptdialog.c')
binary = Path('/tmp/reliefwin-cryptdialog')
program.write_text('#include ' + json.dumps(str(root / 'cryptinput.h')) + '\n' +
                   fixture + password + '\n' + failure + wrapper + tests)
subprocess.run(['clang', '-std=c11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', str(program), '-o', str(binary)], check=True)
raise SystemExit(subprocess.run([str(binary)]).returncode)
