#!/usr/bin/env python3
# PROJECT:     ReliefWin Boot Loader
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Reproduce the OFL font subset for the pre-boot unlock screen
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

from pathlib import Path
import re
from fontTools import subset
from fontTools.ttLib import TTFont

assets = Path(__file__).resolve().parent
source = assets.parents[2] / 'media/fonts/NotoSansSC-Regular.otf'
strings = (assets.parent / 'cryptstrings.h').read_text(encoding='utf-8')
text = ''.join(re.findall(r'L"([^"]*)"', strings)) + ''.join(chr(i) for i in range(32, 127)) + '•'
font = TTFont(source, recalcTimestamp=False)
missing = set(map(ord, text)) - set(font.getBestCmap())
if missing:
    raise ValueError('Missing glyphs: ' + repr(missing))
options = subset.Options()
options.name_IDs = [0, 1, 2, 3, 4, 5, 6, 13, 14, 16, 17]
options.name_legacy = True
options.name_languages = ['*']
subsetter = subset.Subsetter(options=options)
subsetter.populate(text=text)
subsetter.subset(font)
for record in font['name'].names:
    if record.nameID in (1, 3, 4, 6, 16):
        record.string = ('ReliefWinUnlock-Regular' if record.nameID == 6 else 'ReliefWin Unlock').encode(record.getEncoding())
if 'CFF ' in font:
    font['CFF '].cff.fontNames[0] = 'ReliefWinUnlock-Regular'
    top = font['CFF '].cff.topDictIndex[0]
    top.FullName = top.FamilyName = 'ReliefWin Unlock'
font.save(assets / 'unlock.otf')
