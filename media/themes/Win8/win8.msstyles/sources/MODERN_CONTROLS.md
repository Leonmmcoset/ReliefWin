# Rounded native controls

`modern_controls.py` generates 78 NORMAL and LIGHT control bitmaps directly
from geometric surfaces and six genuine Microsoft Fluent SVGs. It reads no
Windows atlas, resource dump or screenshot. Microsoft’s published
[geometry guidance](https://learn.microsoft.com/en-us/windows/apps/design/signature-experiences/geometry)
is the design reference: controls and bars use 4 px corners at 96 DPI. Radio
buttons and slider thumbs are circular. Shared split-control edges are straight.
This is an original theme following that guidance, not a Windows 11 pixel-parity
claim.

Run from the repository root:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/modern_controls.py --update-theme
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/modern_controls.py --check
PYTHONDONTWRITEBYTECODE=1 python3 media/themes/Win8/win8.msstyles/sources/test_modern_controls.py
```

Without `--update-theme`, generation writes only bitmap files. The optional
positional argument selects an existing output directory. `--check` only reads.
`--update-theme` also applies bindings to all three font INIs, registers the new
resources in win8.rc, and writes the artifact manifest `modern_controls.json`.
Existing font settings, system metrics and unrelated sections are preserved.
Push buttons retain their original 5,5,4,4 content padding so rounded surfaces
do not reduce the text area in existing fixed-size dialog buttons.
The generator does not touch existing bitmap files, caption buttons or frames.
The checked output used Pillow 9.0.1, CairoSVG 2.9.1 and Cairo 1.16.0.

Each bitmap has a 40-byte BITMAPINFOHEADER, positive height, 32-bit straight
BGRA pixels and BI_RGB compression. uxtheme's `prepare_alpha` recognizes this
format and performs its own premultiplication. Every zero-alpha pixel has zero
RGB; no magenta or opaque corner patch is used. Shape edges are antialiased
from a four-times raster. Complete Fluent SVGs are rasterized at their native
12-pixel size separately, without scaling, rotation, path edits or gamma
changes. The original files, hashes and MIT license are recorded in
[fluentui-system-icons/SOURCE.txt](fluentui-system-icons/SOURCE.txt).

| Control | Native asset size | States and shape |
| --- | --- | --- |
| Push button | 32 x 24 | Normal, hot, pressed, disabled, defaulted; 4 px corners |
| Edit text | 32 x 24 | All eight ETS states; 4 px corners |
| Four edit border parts | 32 x 24 | Normal, hot, focused, disabled; 4 px corners |
| Combo class, Border, ReadOnly | 32 x 24 | Four native states; 4 px corners |
| Combo drop button | 24 x 24 | Four states; shared inner edge straight, outer corners round |
| Checkbox | 16 x 16 | 12 unchecked/checked/mixed states; native 12 px mark, 4 px corners |
| Radio | 16 x 16 | Eight unchecked/checked states; circular ring and dot |
| Group box | 32 x 24 | Normal/disabled; transparent center, 4 px outline |
| Scroll thumb/track | 24 x 12 or 12 x 24 | Four native states; 4 px corners |
| Scroll arrow | 24 x 24 | 16 direction/state backgrounds; separate native 12 px glyph strip |
| Progress track/fill | 32 x 12 or 12 x 32 | Track one state; fill normal/error/paused/partial; 4 px corners |
| Eight tab item parts | 32 x 24 | Five native states; top corners round, pane edge straight |
| Tab pane | 32 x 24 | One state; 4 px corners |
| Spin directions | 24 x 24 | Four states; shared edge straight, separate native 12 px glyphs |
| Slider thumb/channel | 20 x 20; 32 x 8 or 8 x 32 | Five circular-thumb states; 4 px channel |
| Toolbar/split buttons | 24 x 24 | Six states; normal/disabled background transparent; shared edge straight |
| List view and tree view items | 32 x 24 | Six public states; normal/disabled transparent, other states 4 px backplates |

ListItem and TreeItem both use the six public vsstyle states, in order: normal,
hot, selected, disabled, selected without focus, hot selected. State 6 is hot
selected; there is no state 7 or error state in either public state enum. These
identities were checked against Microsoft's published Windows SDK header in
[win32metadata](https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/um/vsstyle.h),
as well as the existing native consumers. NORMAL hot/selected/unfocused/hot
selected centers are RGB (49,54,63), (32,65,87), (50,55,64), (38,79,103); LIGHT
uses (235,239,245), (229,241,251), (234,234,234), (216,235,250). Selected states
have an accent border; the unfocused selection uses the ordinary border. Disabled
text uses the theme's disabled foreground. List and tree client backgrounds,
outer content rectangles and geometry are unchanged.

Glyph strips use true native cells through GlyphImageFile; their paths are not
embedded in a stretchable center slice. Check and radio use TrueSize and retain
a compact 16 px footprint for existing dialogs. The original FONT values remain
in all font variants. The NORMAL edit interior is RGB (32,32,32), matching its
native COLOR_WINDOW/disabled client fill. LIGHT uses white for enabled fields.
Small content rectangles remain ordinary content; only actual control surfaces
receive rounded corners. Toolbars under ExplorerBar and Placesbar receive the
same control surfaces while taskbar-specific surfaces remain separate.

Checkbox, radio and the six slider-thumb parts retain explicit
`TrueSizeScalingType=Dpi`, `TrueSizeStretchMark=50` and `UniformSizing=True`.
The single modern source bitmap has its renderer-defined source DPI of 96;
old ImageFile1/2/3 selectors and density labels are not restored. In the current
`get_image_part_size` implementation, TS_DRAW scales a TrueSize source when the
theme DPI is at least 150% of its source DPI. With no destination rectangle,
the new 16 px checkbox/radio therefore produces 16/24/32 px at 96/144/192 DPI;
the 20 px thumb produces 20/30/40 px. TS_TRUE continues to report the stored
16/20 px source. When a smaller target rectangle is present, UniformSizing
fits the result to that rectangle, retaining compact native control geometry.
These expectations describe this theme and the current renderer contract, not
Windows pixel parity. Empty scrollbar grippers and standalone toolbar glyphs
do not acquire these DPI properties.

Part/state identities follow the existing public vsstyle declarations and
uxtheme style mappings. The host test reads actual BMP headers, state dimensions,
pixels, alpha and native glyph coverage; checks every generated INI binding and
RC resource; and verifies repeated output plus corrupted-artifact rejection.
Runtime transparency also depends on each native control restoring its parent
background before an alpha draw. The native rendering tests and real screenshots
cover the controls listed in the validation report; group-box and toolbar native
rendering are outside that runtime coverage. Host checks alone do not establish
native rendering behavior.
