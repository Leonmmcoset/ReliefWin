# Modern native menus

`modern_menus.py` generates the default theme's MENU resources and
[`sdk/include/reliefwin/menuart.h`](../../../../../sdk/include/reliefwin/menuart.h)
from original geometric surfaces and genuine Microsoft Fluent SVGs. There is
no Windows atlas, screenshot or binary input. Microsoft's published
[geometry guidance](https://learn.microsoft.com/en-us/windows/apps/design/signature-experiences/geometry)
is the design reference: popup panels use 8 px corners, item hover backplates
use 4 px corners, and popup content has 6 px outer padding at 96 DPI. Popup
rows have a 28 px minimum height and 6 px padding above and below their text.
Separators occupy a 9 px row containing one centered pixel line. These are
ReliefWin product design values, not measured Windows 11 pixel constants.

The shared header contains the same geometry and the unmodified native
12 × 12 alpha coverage of Checkmark, Chevron Right, Chevron Left, Chevron Up,
Chevron Down and Circle.
The native kernel menu renderer consumes those values and glyph masks; it
derives colors from system colors. It does not load these bitmap resources
or call uxtheme from kernel mode. The MENU resources serve existing public
theme API consumers and keep their parts and state order coherent with the
native menu design. Rendering and input verification belongs to the actual
guest tests, not the host asset checks.

Run from the repository root:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/modern_menus.py --update-theme
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/modern_menus.py --check
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/test_modern_menus.py
```

The positional argument selects an existing output directory. Without
`--update-theme`, generation writes only the menu bitmap files. Theme update
also writes the shared header and artifact manifest, registers the 20 new
resources and applies MENU bindings to all three font variants. Unrelated
control sections, fonts, metrics and existing bitmaps are preserved. The
`NORMAL_MENU`/`LIGHT_MENU` names are separate from the modern-controls resource
family, so either generator can update its own resources independently.

| Part | Native cell and states | Resource contract |
| --- | --- | --- |
| PopupBackground | 40 × 40, one state | 8 px rounded solid panel with subtle 1 px border |
| PopupBorders | 40 × 40, one state | Same border with transparent center |
| PopupItem | 32 × 28, four states | Normal and disabled transparent; hot and disabled-hot 4 px backplates |
| BarItem | 32 × 28, six states | Normal, hot, pushed, disabled, disabled-hot, disabled-pushed |
| PopupCheck | 12 × 12, four states | Checkmark normal/disabled, circle normal/disabled |
| PopupSubmenu | 12 × 12, two states | Complete native right-chevron normal/disabled; RTL-capable binding |
| PopupSeparator | 32 × 9, one state | One opaque center line, other rows transparent |
| PopupCheckBackground | 16 × 16, three states | Transparent; checked items receive the glyph without a second box |
| PopupGutter | 1 × 1, one state | Same solid fill as the popup panel |
| BarBackground | 1 × 1, two states | Same active/inactive menubar fill |

All BMPs use a 40-byte BITMAPINFOHEADER, positive height, 32-bit straight BGRA
and BI_RGB. Transparent pixels have zero RGB. The existing uxtheme loader
performs its normal premultiplication. Shape edges use the existing generator's
four-times antialias raster. The Fluent SVGs are separately rasterized at their
intrinsic 12 × 12 size; the glyph path, alpha coverage, rotation, position and
gamma are not edited. Only the final foreground color changes by state.
The shared header arrays retain that exact native raster coverage.

The normal theme retains enabled RGB (240,240,240) and disabled RGB
(128,128,128) menu text. ContentLight retains enabled black, disabled popup
RGB (159,159,159) and disabled menubar RGB (109,109,109). These preserve the
existing text-color contract. Default fixed resource panels use RGB (32,32,32)
and (249,249,249); native menus continue using the current system menu colors.

The public MENU part and state identities were checked against Microsoft's
published Windows SDK
[`vsstyle.h`](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/um/vsstyle.h).
The existing style-map names provide the INI spellings. Glyph sources,
pinned hashes and the MIT license are in
[`fluentui-system-icons/SOURCE.txt`](fluentui-system-icons/SOURCE.txt).
The host tests independently check actual BMP layouts, state order, opacity,
native glyph coverage, generated header bytes, all three INIs, RC references,
deterministic output and corrupted-artifact rejection. They do not establish
Windows 11 parity or successful native menu operation.
