# PROJECT:     ReliefWin visual style
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Generate flat caption buttons using original Fluent UI icons
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
import hashlib
import io
from pathlib import Path
import struct

import cairosvg
from PIL import Image


SOURCE_DIR = Path(__file__).resolve().parent
ICON_DIR = SOURCE_DIR / "fluentui-system-icons"
BITMAP_DIR = SOURCE_DIR.parent / "bitmaps"
BUTTONS = {
    "CLOSEBUTTON": ("ic_fluent_dismiss_16_regular.svg", "118c94c171be3364e5104983a97ba3d37d53b21ebc4d6f127eef31dc4aa76716"),
    "MINBUTTON": ("ic_fluent_subtract_16_regular.svg", "663f8030c439784943e978023348c52c5e24f629b4d85e85da70b61ffb3e5f6f"),
    "MAXBUTTON": ("ic_fluent_square_16_regular.svg", "474c2b224afeec2a522542335ab93bf5cdf0dda6b3b525347eec254be0cb44b1"),
    "RESTOREBUTTON": ("ic_fluent_square_multiple_16_regular.svg", "3e10bcc784b7bfa841f171837a164eae5caf174a64281b1a397947ddfc528e7b"),
}
PALETTES = {
    "NORMAL": {
        "active": (28, 32, 40),
        "inactive": (39, 43, 51),
        "glyph": (255, 255, 255),
        "inactive_glyph": (160, 160, 160),
        "disabled_glyph": (109, 109, 109),
        "hover": 24,
        "pressed": 14,
    },
    "LIGHT": {
        "active": (238, 238, 238),
        "inactive": (243, 243, 243),
        "glyph": (0, 0, 0),
        "inactive_glyph": (95, 95, 95),
        "disabled_glyph": (146, 146, 146),
        "hover": -24,
        "pressed": -14,
    },
}
SIZE = (46, 32)
GLYPH_SIZE = (16, 16)


def load_masks():
    masks = {}
    for button, (filename, digest) in BUTTONS.items():
        data = (ICON_DIR / filename).read_bytes()
        if hashlib.sha256(data).hexdigest() != digest:
            raise ValueError("Fluent source differs from the pinned original: " + filename)
        png = cairosvg.svg2png(bytestring=data)
        with Image.open(io.BytesIO(png)) as glyph:
            if glyph.size != GLYPH_SIZE:
                raise ValueError("Fluent source is not native 16 pixels: " + filename)
            masks[button] = glyph.convert("RGBA").getchannel("A")
    return masks


def render(button, mask, palette):
    strip = Image.new("RGBA", (SIZE[0], SIZE[1] * 8))
    for index in range(8):
        state = index % 4
        inactive = index >= 4
        background = palette["inactive" if inactive else "active"]
        foreground = palette["inactive_glyph" if inactive else "glyph"]
        if state == 3:
            foreground = palette["disabled_glyph"]
        elif state in (1, 2):
            if button == "CLOSEBUTTON":
                background = (196, 43, 28) if state == 1 else (176, 39, 25)
                foreground = (255, 255, 255)
            else:
                amount = palette["hover" if state == 1 else "pressed"]
                background = tuple(value + amount for value in background)
                foreground = palette["glyph"]
        frame = Image.new("RGBA", SIZE, (*background, 255))
        glyph = Image.new("RGBA", GLYPH_SIZE, (*foreground, 255))
        glyph.putalpha(mask)
        frame.alpha_composite(glyph, ((SIZE[0] - GLYPH_SIZE[0]) // 2,
                                     (SIZE[1] - GLYPH_SIZE[1]) // 2))
        strip.paste(frame, (0, index * SIZE[1]))
    return strip


def bitmap_bytes(image):
    width, height = image.size
    pixels = image.tobytes("raw", "BGRA", 0, -1)
    header = struct.pack("<IiiHHIIiiII", 108, width, height, 1, 32, 3,
                         len(pixels), 2835, 2835, 0, 0)
    header += struct.pack("<IIII", 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)
    header += b"BGRs" + bytes(36) + struct.pack("<III", 0, 0, 0)
    offset = 14 + len(header)
    return b"BM" + struct.pack("<IHHI", offset + len(pixels), 0, 0, offset) + header + pixels


def main():
    parser = argparse.ArgumentParser(description="Render native Fluent caption buttons without an atlas")
    parser.add_argument("output", nargs="?", type=Path, default=BITMAP_DIR)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    masks = load_masks()
    failures = []
    for name, palette in PALETTES.items():
        for button, mask in masks.items():
            path = args.output / (name + "_" + button + ".bmp")
            data = bitmap_bytes(render(button, mask, palette))
            current = path.read_bytes() if path.is_file() else None
            if current == data:
                continue
            if args.check:
                failures.append(str(path))
            else:
                path.write_bytes(data)
    if failures:
        parser.exit(1, "Caption bitmap differs or is missing:\n" + "\n".join(failures) + "\n")
    print("8 Fluent caption bitmaps " + ("verified" if args.check else "generated"))


if __name__ == "__main__":
    main()
