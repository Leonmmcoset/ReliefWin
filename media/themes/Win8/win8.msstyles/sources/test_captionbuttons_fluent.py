# PROJECT:     ReliefWin visual style tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Validate generated caption button pixels and source provenance
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import hashlib
import io
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

import cairosvg
from PIL import Image


HERE = Path(__file__).resolve().parent
BITMAPS = HERE.parent / "bitmaps"
REPO = HERE.parents[4]
SOURCES = {
    "CLOSEBUTTON": ("ic_fluent_dismiss_16_regular.svg", "118c94c171be3364e5104983a97ba3d37d53b21ebc4d6f127eef31dc4aa76716"),
    "MINBUTTON": ("ic_fluent_subtract_16_regular.svg", "663f8030c439784943e978023348c52c5e24f629b4d85e85da70b61ffb3e5f6f"),
    "MAXBUTTON": ("ic_fluent_square_16_regular.svg", "474c2b224afeec2a522542335ab93bf5cdf0dda6b3b525347eec254be0cb44b1"),
    "RESTOREBUTTON": ("ic_fluent_square_multiple_16_regular.svg", "3e10bcc784b7bfa841f171837a164eae5caf174a64281b1a397947ddfc528e7b"),
}
STATE_COLORS = {
    "NORMAL": (
        ((28, 32, 40), (255, 255, 255)),
        ((52, 56, 64), (255, 255, 255)),
        ((42, 46, 54), (255, 255, 255)),
        ((28, 32, 40), (109, 109, 109)),
        ((39, 43, 51), (160, 160, 160)),
        ((63, 67, 75), (255, 255, 255)),
        ((53, 57, 65), (255, 255, 255)),
        ((39, 43, 51), (109, 109, 109)),
    ),
    "LIGHT": (
        ((238, 238, 238), (0, 0, 0)),
        ((214, 214, 214), (0, 0, 0)),
        ((224, 224, 224), (0, 0, 0)),
        ((238, 238, 238), (146, 146, 146)),
        ((243, 243, 243), (95, 95, 95)),
        ((219, 219, 219), (0, 0, 0)),
        ((229, 229, 229), (0, 0, 0)),
        ((243, 243, 243), (146, 146, 146)),
    ),
}


def decode(path):
    data = path.read_bytes()
    offset = struct.unpack_from("<I", data, 10)[0]
    width, height = struct.unpack_from("<ii", data, 18)
    rows = []
    for y in range(height):
        start = offset + (height - 1 - y) * width * 4
        rows.append([tuple(data[start + x * 4 + channel] for channel in (2, 1, 0, 3))
                     for x in range(width)])
    return data, width, height, rows


class CaptionAssets(unittest.TestCase):
    def test_strip_layout_and_opacity(self):
        for palette in STATE_COLORS:
            for button in SOURCES:
                with self.subTest(palette=palette, button=button):
                    data, width, height, pixels = decode(BITMAPS / (palette + "_" + button + ".bmp"))
                    self.assertEqual(data[:2], b"BM")
                    self.assertEqual((width, height), (46, 256))
                    self.assertEqual(struct.unpack_from("<I", data, 2)[0], len(data))
                    self.assertEqual(struct.unpack_from("<I", data, 10)[0], 122)
                    self.assertEqual(struct.unpack_from("<I", data, 14)[0], 108)
                    self.assertEqual(struct.unpack_from("<HHI", data, 26), (1, 32, 3))
                    self.assertEqual(struct.unpack_from("<I", data, 34)[0], 46 * 256 * 4)
                    self.assertEqual(struct.unpack_from("<IIII", data, 54),
                                     (0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000))
                    self.assertEqual(len(data), 122 + 46 * 256 * 4)
                    self.assertTrue(all(pixel[3] == 255 for row in pixels for pixel in row))

    def test_sources_are_original_native_cells(self):
        for filename, digest in SOURCES.values():
            with self.subTest(source=filename):
                data = (HERE / "fluentui-system-icons" / filename).read_bytes()
                self.assertEqual(hashlib.sha256(data).hexdigest(), digest)
                self.assertEqual(data, (REPO / "base/shell/shellframe/res/icons" / filename).read_bytes())
                with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as image:
                    self.assertEqual(image.size, (16, 16))

    def test_actual_pixels_use_complete_unmodified_coverage(self):
        for button, (filename, _) in SOURCES.items():
            data = (HERE / "fluentui-system-icons" / filename).read_bytes()
            with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as image:
                mask = image.convert("RGBA").getchannel("A")
            for palette, colors in STATE_COLORS.items():
                _, width, height, pixels = decode(BITMAPS / (palette + "_" + button + ".bmp"))
                self.assertEqual((width, height), (46, 256))
                for index, (background, foreground) in enumerate(colors):
                    if button == "CLOSEBUTTON" and index in (1, 2, 5, 6):
                        background = (196, 43, 28) if index in (1, 5) else (176, 39, 25)
                        foreground = (255, 255, 255)
                    for y in range(32):
                        for x in range(46):
                            alpha = mask.getpixel((x - 15, y - 8)) if 15 <= x < 31 and 8 <= y < 24 else 0
                            rgb = tuple((fg * alpha + bg * (255 - alpha) + 127) // 255
                                        for bg, fg in zip(background, foreground))
                            self.assertEqual(pixels[index * 32 + y][x], (*rgb, 255),
                                             (palette, button, index + 1, x, y))

    def test_normal_backgrounds_equal_actual_caption(self):
        for palette in STATE_COLORS:
            with Image.open(BITMAPS / (palette + "_FRAMECAPTION.bmp")) as image:
                caption = image.convert("RGB")
            for button in SOURCES:
                _, width, height, pixels = decode(BITMAPS / (palette + "_" + button + ".bmp"))
                self.assertEqual((width, height), (46, 256))
                for index in (0, 3, 4, 7):
                    frame = 0 if index < 4 else 1
                    self.assertEqual(pixels[index * 32][0][:3],
                                     caption.getpixel((caption.width // 2,
                                                       frame * (caption.height // 2) + caption.height // 4)))

    def test_reproducible_generation_and_check(self):
        env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
        generator = [sys.executable, str(HERE / "captionbuttons_fluent.py")]
        subprocess.run(generator + ["--check"], env=env, check=True, capture_output=True)
        with tempfile.TemporaryDirectory(prefix="reliefwin-caption-") as folder:
            output = Path(folder)
            subprocess.run(generator + [folder], env=env, check=True, capture_output=True)
            expected = {palette + "_" + button + ".bmp" for palette in STATE_COLORS for button in SOURCES}
            self.assertEqual({p.name for p in output.iterdir()}, expected)
            before = {}
            for name in expected:
                path = output / name
                self.assertEqual(path.read_bytes(), (BITMAPS / name).read_bytes())
                before[name] = (path.read_bytes(), path.stat().st_mtime_ns)
            subprocess.run(generator + [folder], env=env, check=True, capture_output=True)
            after = {name: ((output / name).read_bytes(), (output / name).stat().st_mtime_ns) for name in expected}
            self.assertEqual(before, after)
            damaged = output / "NORMAL_CLOSEBUTTON.bmp"
            original = damaged.read_bytes()
            damaged.write_bytes(original[:-1] + bytes((original[-1] ^ 1,)))
            rejected = subprocess.run(generator + [folder, "--check"], env=env, capture_output=True)
            self.assertEqual(rejected.returncode, 1)
            self.assertIn(b"NORMAL_CLOSEBUTTON.bmp", rejected.stderr)
            self.assertNotEqual(damaged.read_bytes(), original)
            damaged.unlink()
            missing = subprocess.run(generator + [folder, "--check"], env=env, capture_output=True)
            self.assertEqual(missing.returncode, 1)
            self.assertFalse(damaged.exists())


if __name__ == "__main__":
    unittest.main()
