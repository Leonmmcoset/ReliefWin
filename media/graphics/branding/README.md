# ReliefWin artwork

`reliefwin-source.jpg` is the original image supplied by the ReliefWin maintainer.
`reliefwin-mark.png` removes its white background and crops the surrounding margin;
the blue shapes retain their aspect ratio. The logo and banner add the product name
using the repository's FreeSans font. System icons, the Start button, boot logos,
setup, logon, screen saver, System Properties and About resources derive from it.

Regenerate the checked-in resources from the repository root with Python 3 and
Pillow 8.4 or later:

```sh
python3 scripts/generate_brand_assets.py
python3 scripts/check_brand_assets.py
python3 scripts/test_boot_brand.py
```

`assets.json` records the original image's SHA-256 and all 40 generated resources.
`obsolete-assets.json` records only hashes and original paths of replaced artwork;
the checker scans repository image assets to reject old artwork under any filename.
The generator preserves existing resource identifiers, dimensions and bitmap
contracts: separate RGB/alpha masks for setup and System Properties, premultiplied
48 × 42 RGBA for the boot menu, BGRA with explicit alpha masks for the GOP logo,
and 16-color RLE4 for the VGA boot logo, attribution and shutdown message. All three
VGA resources share one palette, with black at index 0 and white at index 15, so a
later bitmap cannot change the colors of pixels already displayed by bootvid.

The bitmap header and pixel encoding follow Microsoft's
[BITMAPV4HEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header)
and [bitmap compression](https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-compression)
contracts. Resource dimensions and alpha interpretation follow their existing
bootloader, bootvid, setup and shell consumers.

The host regression compiles the kernel's actual rasterization functions. It
checks every supported integer DPI, fill progression, animation rectangle limits,
buffer bounds, undersized displays and invalid DPI fallback. `--baseline` verifies
that the committed original geometry fails the progress-bar regression before the
rebranding change is committed.

The Explorer settings previews retain their existing sample layout and use the
new product name and mark. They are illustrations shipped in settings, not
screenshots of a validation run.
