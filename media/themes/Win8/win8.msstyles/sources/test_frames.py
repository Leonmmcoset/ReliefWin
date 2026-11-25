# PROJECT:     ReliefWin visual style tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Validate window frame surfaces without highlighted outlines
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import configparser
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from PIL import Image


HERE = Path(__file__).resolve().parent
BITMAPS = HERE.parent / "bitmaps"
SURFACES = {
    "FRAMECAPTION": (8, 30),
    "SMALLFRAMECAPTION": (8, 22),
    "FRAMECAPTIONMIN": (8, 30),
    "FRAMELEFT": (6, 8),
    "FRAMERIGHT": (6, 8),
    "FRAMEBOTTOM": (8, 6),
    "FRAMECAPTIONSIZING": (40, 20),
    "SMALLFRAMECAPTIONSIZING": (40, 20),
    "FRAMEMAXIMIZED": (8, 30),
}
PALETTES = {
    "NORMAL": ((28, 32, 40), (39, 43, 51)),
    "LIGHT": ((238, 238, 238), (243, 243, 243)),
}
CONSUMERS = {
    "Caption": "FRAMECAPTION",
    "MinCaption": "FRAMECAPTIONMIN",
    "MaxCaption": "FRAMEMAXIMIZED",
    "SmallCaption": "SMALLFRAMECAPTION",
    "FrameLeft": "FRAMELEFT",
    "FrameRight": "FRAMERIGHT",
    "FrameBottom": "FRAMEBOTTOM",
    "SmallFrameLeft": "FRAMELEFT",
    "SmallFrameRight": "FRAMERIGHT",
    "SmallFrameBottom": "FRAMEBOTTOM",
    "CaptionSizingTemplate": "FRAMECAPTIONSIZING",
    "SmallCaptionSizingTemplate": "SMALLFRAMECAPTIONSIZING",
}


class FrameAssets(unittest.TestCase):
    def test_all_frame_pixels(self):
        for palette, colors in PALETTES.items():
            for name, (width, height) in SURFACES.items():
                with self.subTest(palette=palette, surface=name):
                    path = BITMAPS / (palette + "_" + name + ".bmp")
                    with Image.open(path) as image:
                        self.assertEqual(image.size, (width, height * 2))
                        self.assertEqual(image.mode, "RGB")
                        pixels = image.convert("RGBA")
                    for state, body in enumerate(colors):
                        for y in range(height):
                            for x in range(width):
                                self.assertEqual(pixels.getpixel((x, state * height + y)),
                                                 (*body, 255), (path.name, state, x, y))

    def test_bitmap_headers_preserve_geometry(self):
        for palette in PALETTES:
            for name, (width, height) in SURFACES.items():
                with self.subTest(palette=palette, surface=name):
                    data = (BITMAPS / (palette + "_" + name + ".bmp")).read_bytes()
                    self.assertEqual(data[:2], b"BM")
                    self.assertEqual(struct.unpack_from("<I", data, 2)[0], len(data))
                    self.assertEqual(struct.unpack_from("<I", data, 10)[0], 54)
                    self.assertEqual(struct.unpack_from("<IiiHHI", data, 14),
                                     (40, width, height * 2, 1, 24, 0))
                    stride = (width * 3 + 3) // 4 * 4
                    self.assertEqual(len(data), 54 + stride * height * 2)

    def test_all_theme_frame_consumers_are_covered(self):
        for filename in ("NormalNormal.INI", "LargeFontsNormal.INI", "ExtraLargeNormal.INI"):
            ini = configparser.ConfigParser(strict=False, interpolation=None)
            ini.read(HERE.parent / "textfiles" / filename)
            for palette, prefix in (("NORMAL", ""), ("LIGHT", "ContentLight::")):
                for part, image in CONSUMERS.items():
                    section = prefix + "Window." + part
                    with self.subTest(ini=filename, section=section):
                        self.assertTrue(ini.has_section(section))
                        self.assertEqual(ini.get(section, "ImageFile").upper(),
                                         palette + "\\" + image + ".BMP")
                        self.assertEqual(ini.getint(section, "ImageCount"), 2)
                        self.assertEqual(ini.get(section, "ImageLayout").lower(), "vertical")
                        self.assertIn(image, SURFACES)

    def test_generation_is_deterministic_and_limited_to_frames(self):
        command = [sys.executable, str(HERE / "frames.py")]
        env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
        subprocess.run(command + ["--check"], env=env, check=True, capture_output=True)
        expected = {palette + "_" + name + ".bmp" for palette in PALETTES for name in SURFACES}
        with tempfile.TemporaryDirectory(prefix="reliefwin-frame-") as folder:
            output = Path(folder)
            subprocess.run(command + [folder], env=env, check=True, capture_output=True)
            self.assertEqual({p.name for p in output.iterdir()}, expected)
            before = {}
            for name in expected:
                path = output / name
                self.assertEqual(path.read_bytes(), (BITMAPS / name).read_bytes())
                before[name] = (path.read_bytes(), path.stat().st_mtime_ns)
            subprocess.run(command + [folder], env=env, check=True, capture_output=True)
            after = {name: ((output / name).read_bytes(), (output / name).stat().st_mtime_ns) for name in expected}
            self.assertEqual(before, after)
            damaged = output / "LIGHT_FRAMECAPTION.bmp"
            original = damaged.read_bytes()
            damaged.write_bytes(original[:-1] + bytes((original[-1] ^ 1,)))
            result = subprocess.run(command + [folder, "--check"], env=env, capture_output=True)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"LIGHT_FRAMECAPTION.bmp", result.stderr)
            self.assertNotEqual(damaged.read_bytes(), original)
            damaged.unlink()
            result = subprocess.run(command + [folder, "--check"], env=env, capture_output=True)
            self.assertEqual(result.returncode, 1)
            self.assertFalse(damaged.exists())


if __name__ == "__main__":
    unittest.main()
