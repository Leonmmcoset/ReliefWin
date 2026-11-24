`default.png`, `hover.png` and `pressed.png` contain the supplied ReliefWin mark
on the same transparent 640 × 640 canvas, with identical scale and position.
Explorer retains its existing image size, DPI scaling and button margins.
Disabled and keyboard-focused buttons reuse the normal artwork.

Regenerate them with `python3 scripts/generate_brand_assets.py` from the repository
root. See [the artwork instructions](../branding/README.md) for the source image,
resource contracts and validation. Normal builds consume the checked-in PNGs.
