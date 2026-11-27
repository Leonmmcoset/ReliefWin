# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Build actual language code and exercise firmware preference I/O
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

from pathlib import Path
import re
import subprocess
from fontTools.ttLib import TTFont

root = Path(__file__).resolve().parents[2]
flags = ['clang', '-std=c11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-I' + str(root / 'include')]
binary = Path('/tmp/reliefwin-bootlang-test')
subprocess.run(flags + [str(root / 'gui/bootlang.c'), str(root / 'gui/tests/bootlang.c'), '-o', str(binary)], check=True)
subprocess.run([str(binary)], check=True)

source = (root / 'gui/bootlangio.c').read_text()
functions = source[source.index('static BOOLEAN UiLanguageLoaded;'):]
fixture = r'''
#include <bootlang.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef unsigned ULONG;
typedef int BOOLEAN;
typedef void *PVOID;
#define TRUE 1
#define FALSE 0
static char Storage[16];
static unsigned Stored, WriteCalls, FreeCalls;
static int WriteFails, ReadFails;
static PVOID FwReadBootFile(const void *Path, ULONG *Size)
{(void)Path; *Size = Stored; return ReadFails || !Stored ? NULL : Storage;}
static int FwWriteBootFile(const void *Path, const void *Data, ULONG Size)
{(void)Path; ++WriteCalls; if (WriteFails) return 0; assert(Size <= sizeof(Storage)); memcpy(Storage, Data, Size); Stored = Size; return 1;}
static void FwFreePages(void *Data, ULONG Size)
{assert(Data == Storage && Size == Stored); ++FreeCalls;}
'''
tests = r'''
int main(void)
{
    UiLoadLanguage();
    assert(UiGetLanguage() == UiLanguageChinese && !FreeCalls);
    memcpy(Storage, "ja-JP", 5); Stored = 5; UiLanguageLoaded = 0;
    UiLoadLanguage();
    assert(UiGetLanguage() == UiLanguageJapanese && FreeCalls == 1);
    memcpy(Storage, "ja-JPxx", 7); Stored = 7; UiLanguageLoaded = 0;
    UiSelectLanguage(UiLanguageChinese); UiLoadLanguage();
    assert(UiGetLanguage() == UiLanguageChinese && FreeCalls == 2);
    assert(UiSaveLanguage(UiLanguageEnglish));
    assert(Stored == 5 && !memcmp(Storage, "en-US", 5) && WriteCalls == 1);
    UiLanguageLoaded = 0; UiSelectLanguage(UiLanguageChinese); UiLoadLanguage();
    assert(UiGetLanguage() == UiLanguageEnglish);
    WriteFails = 1;
    assert(!UiSaveLanguage(UiLanguageJapanese) && UiGetLanguage() == UiLanguageJapanese);
    assert(!memcmp(Storage, "en-US", 5));
    WriteFails = 0; ReadFails = 1;
    assert(!UiSaveLanguage(UiLanguageChinese) && UiGetLanguage() == UiLanguageChinese);
    assert(!UiSaveLanguage(UiLanguageCount) && WriteCalls == 3);
    puts("10 boot preference I/O checks passed");
    return 0;
}
'''
program = Path('/tmp/reliefwin-bootlang-io.c')
program.write_text(fixture + functions + tests)
subprocess.run(flags + [str(root / 'gui/bootlang.c'), str(program), '-o', str(binary)], check=True)
subprocess.run([str(binary)], check=True)

source = (root / 'bootmgr/main.c').read_text()
start = source.index('static\nBOOLEAN\nBmMenuIdle(')
end = source.index('\nstatic\n', start + 1)
fixture = r'''
#include <bootlang.h>
#include <assert.h>
#include <stdio.h>
typedef int BOOLEAN;
typedef BOOLEAN *PBOOLEAN;
typedef unsigned ULONG;
typedef ULONG *PULONG;
#define IN
#define OUT
#define TRUE 1
#define FALSE 0
typedef struct {
    const char **MenuItemList;
    const char *MenuHeader;
    ULONG SelectedMenuItem, MenuItemCount;
} UI_MENU_INFO, *PUI_MENU_INFO;
static const char *BmMenuNames[4];
static PULONG BmTrackedItem;
static ULONG BmCount = 1, Rebuilt;
static BOOLEAN ScanChanged;
#define BM_EXTRA_ITEMS 3
static BOOLEAN BmScanStep(BOOLEAN *Changed)
{*Changed = ScanChanged; return 0;}
static void BmBuildMenu(void)
{++Rebuilt;}
'''
tests = r'''
int main(void)
{
    BOOLEAN Changed;
    UI_MENU_INFO Menu = {BmMenuNames, UiText(UiTextChooseBoot), 0, 4};
    assert(!BmMenuIdle(&Menu, &Changed) && !Changed);
    UiSelectLanguage(UiLanguageJapanese);
    assert(!BmMenuIdle(&Menu, &Changed) && Changed);
    assert(Menu.MenuHeader == UiText(UiTextChooseBoot));
    assert(!BmMenuIdle(&Menu, &Changed) && !Changed);
    ScanChanged = 1;
    assert(!BmMenuIdle(&Menu, &Changed) && Changed && Rebuilt == 1);
    assert(!BmTrackedItem);
    puts("5 parent menu refresh checks passed");
    return 0;
}
'''
program = Path('/tmp/reliefwin-bootlang-menu.c')
program.write_text(fixture + source[start:end] + tests)
subprocess.run(flags + [str(root / 'gui/bootlang.c'), str(program), '-o', str(binary)], check=True)
subprocess.run([str(binary)], check=True)

font = TTFont(root / 'gui/assets/unlock.otf')
cmap = set(font.getBestCmap())
text = (root / 'include/bootstrings.inc').read_text() + (root / 'gui/cryptstrings.h').read_text()
characters = set(map(ord, ''.join(re.findall(r'"([^"]*)"', text))))
assert not characters - cmap, sorted(characters - cmap)
print(f'{len(characters)} menu/unlock glyphs present')
