# PROJECT:     ReliefWin visual style tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify actual rounded control assets and theme consumers
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import configparser
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

import cairosvg
from PIL import Image


HERE = Path(__file__).resolve().parent
THEME = HERE.parent
SPEC = importlib.util.spec_from_file_location("modern_asset_contract", HERE / "modern_controls.py")
GEN = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(GEN)
MANIFEST = json.loads((HERE / "modern_controls.json").read_text())


def read_bitmap(path):
    data = path.read_bytes()
    width, height = struct.unpack_from("<ii", data, 18)
    image = Image.frombytes("RGBA", (width, height), data[54:], "raw", "BGRA", 0, -1)
    return data, image


class RoundedAssets(unittest.TestCase):
    def test_actual_format_alpha_and_corners(self):
        self.assertEqual(len(MANIFEST), 78)
        for name, item in MANIFEST.items():
            with self.subTest(asset=name):
                data, image = read_bitmap(THEME / "bitmaps" / name)
                w, h = item["size"]
                self.assertEqual(image.size, (w, h * item["states"]))
                self.assertEqual(data[:2], b"BM")
                self.assertEqual(struct.unpack_from("<I", data, 2)[0], len(data))
                self.assertEqual(struct.unpack_from("<I", data, 10)[0], 54)
                self.assertEqual(struct.unpack_from("<IiiHHI", data, 14), (40, w, h * item["states"], 1, 32, 0))
                self.assertEqual(len(data), 54 + w * h * item["states"] * 4)
                self.assertEqual(hashlib.sha256(data).hexdigest(), item["sha256"])
                for pixel in image.getdata():
                    if pixel[3] == 0:
                        self.assertEqual(pixel[:3], (0, 0, 0))
                    self.assertNotEqual(pixel[:3], (255, 0, 255))
                if item["radius"]:
                    self.assertTrue(any(pixel[3] == 0 for pixel in image.getdata()))
                    self.assertTrue(any(0 < pixel[3] < 255 for pixel in image.getdata()))
                for state in range(item["states"]):
                    cell = image.crop((0, state * h, w, (state + 1) * h))
                    if item["radius"] and not any(tag in name for tag in ("TOOLBARSPLIT", "TOOLBARDROP", "SPIN", "COMBOARROW", "TAB.bmp")):
                        for corner in ((0, 0), (w - 1, 0), (0, h - 1), (w - 1, h - 1)):
                            self.assertEqual(cell.getpixel(corner)[3], 0, (name, state, corner))
                if "GLYPH" not in name and not any(tag in name for tag in ("GROUP", "EMPTY", "TOOLBAR", "LISTITEM", "TREEITEM")):
                    self.assertEqual(image.getpixel((w // 2, h // 2))[3], 255)

    def test_part_states_and_geometry(self):
        expected = {"BUTTON": (32, 24, 5), "EDIT": (32, 24, 8), "FIELDBORDER": (32, 24, 4),
                    "COMBO": (32, 24, 4), "CHECK": (16, 16, 12), "RADIO": (16, 16, 8),
                    "SCROLLARROW": (24, 24, 16), "SCROLLGLYPH": (12, 12, 16),
                    "TAB": (32, 24, 5), "TRACKTHUMB": (20, 20, 5), "TOOLBAR": (24, 24, 6),
                    "LISTITEM": (32, 24, 6), "TREEITEM": (32, 24, 6)}
        for palette in ("NORMAL", "LIGHT"):
            for part, shape in expected.items():
                item = MANIFEST[palette + "_MODERN" + part + ".bmp"]
                self.assertEqual((*item["size"], item["states"]), shape)
            for part in ("PROGRESSFILLH", "PROGRESSFILLV"):
                item = MANIFEST[palette + "_MODERN" + part + ".bmp"]
                self.assertEqual((item["states"], item["radius"]), (4, 4))
            for part in ("GROUP", "TOOLBAR"):
                _, image = read_bitmap(THEME / "bitmaps" / (palette + "_MODERN" + part + ".bmp"))
                self.assertEqual(image.getpixel((image.width // 2, 12))[3], 0)

    def test_genuine_native_glyph_coverage(self):
        for direction in ("up", "down", "left", "right"):
            filename, digest = GEN.ICONS[direction]
            data = (HERE / "fluentui-system-icons" / filename).read_bytes()
            self.assertEqual(hashlib.sha256(data).hexdigest(), digest)
            with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as original:
                mask = original.convert("RGBA").getchannel("A")
            self.assertEqual(mask.size, (12, 12))
            for palette in ("NORMAL", "LIGHT"):
                _, image = read_bitmap(THEME / "bitmaps" / (palette + "_MODERN" + direction.upper() + "GLYPH.bmp"))
                for state in range(4):
                    self.assertEqual(image.crop((0, state * 12, 12, (state + 1) * 12)).getchannel("A").tobytes(), mask.tobytes())
                _, arrows = read_bitmap(THEME / "bitmaps" / (palette + "_MODERNSCROLLGLYPH.bmp"))
                index = ("up", "down", "left", "right").index(direction)
                for state in range(4):
                    self.assertEqual(arrows.crop((0, (index * 4 + state) * 12, 12, (index * 4 + state + 1) * 12)).getchannel("A").tobytes(), mask.tobytes())
        for key in ("check", "mixed"):
            filename, digest = GEN.ICONS[key]
            data = (HERE / "fluentui-system-icons" / filename).read_bytes()
            self.assertEqual(hashlib.sha256(data).hexdigest(), digest)
            with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as original:
                self.assertEqual(original.size, (12, 12))
                mask = original.convert("RGBA").getchannel("A")
            for palette in ("NORMAL", "LIGHT"):
                _, image = read_bitmap(THEME / "bitmaps" / (palette + "_MODERNCHECK.bmp"))
                for state in range(4):
                    disabled = state == 3
                    if palette == "NORMAL":
                        background = (65, 69, 76) if disabled else (76, 194, 255)
                        foreground = (125, 129, 136) if disabled else (8, 37, 52)
                    else:
                        background = (213, 213, 213) if disabled else (0, 103, 192)
                        foreground = (161, 161, 161) if disabled else (255, 255, 255)
                    row = (4 if key == "check" else 8) + state
                    for y in range(12):
                        for x in range(12):
                            alpha = mask.getpixel((x, y))
                            if alpha:
                                expected = tuple((fg * alpha + bg * (255 - alpha) + 127) // 255
                                                 for bg, fg in zip(background, foreground))
                                self.assertEqual(image.getpixel((x + 2, row * 16 + y + 2)),
                                                 (*expected, 255), (palette, key, state, x, y))

    def test_selection_state_contract(self):
        colors = {"NORMAL": ((49, 54, 63), (32, 65, 87), (50, 55, 64), (38, 79, 103)),
                  "LIGHT": ((235, 239, 245), (229, 241, 251), (234, 234, 234), (216, 235, 250))}
        for palette in colors:
            for name in ("LISTITEM", "TREEITEM"):
                _, image = read_bitmap(THEME / "bitmaps" / (palette + "_MODERN" + name + ".bmp"))
                for index in (0, 3):
                    cell = image.crop((0, index * 24, 32, (index + 1) * 24))
                    self.assertEqual(set(cell.getdata()), {(0, 0, 0, 0)})
                for index, color in zip((1, 2, 4, 5), colors[palette]):
                    self.assertEqual(image.getpixel((16, index * 24 + 12)), (*color, 255))
                    for x, y in ((0, 0), (31, 0), (0, 23), (31, 23)):
                        self.assertEqual(image.getpixel((x, index * 24 + y)), (0, 0, 0, 0))

    def test_true_size_dpi_bindings(self):
        parts = ("Button.Checkbox", "Button.Radiobutton", "TrackBar.Thumb", "TrackBar.ThumbVert",
                 "TrackBar.ThumbBottom", "TrackBar.ThumbTop", "TrackBar.ThumbLeft", "TrackBar.ThumbRight")
        for filename in ("NormalNormal.INI", "LargeFontsNormal.INI", "ExtraLargeNormal.INI"):
            ini = configparser.ConfigParser(strict=False, interpolation=None)
            ini.read(THEME / "textfiles" / filename)
            sections = {section.lower(): section for section in ini.sections()}
            for prefix in ("", "ContentLight::"):
                for part in parts:
                    section = sections[(prefix + part).lower()]
                    with self.subTest(ini=filename, section=section):
                        self.assertEqual(ini.get(section, "TrueSizeScalingType", fallback="").lower(), "dpi")
                        self.assertEqual(ini.getint(section, "TrueSizeStretchMark", fallback=-1), 50)
                        self.assertEqual(ini.get(section, "SizingType").lower(), "truesize")
                        self.assertTrue(ini.getboolean(section, "UniformSizing"))
                        self.assertFalse(ini.has_option(section, "ImageSelectType"))
            for section in ("ScrollBar.GripperHorz", "ScrollBar.GripperVert", "Toolbar.DropdownButtonGlyph"):
                for prefix in ("", "ContentLight::"):
                    actual = sections[(prefix + section).lower()]
                    self.assertFalse(ini.has_option(actual, "TrueSizeScalingType"))
                    self.assertFalse(ini.has_option(actual, "TrueSizeStretchMark"))

    def test_three_ini_variants_and_resources(self):
        rc = (THEME / "win8.rc").read_text()
        for name in MANIFEST:
            self.assertEqual(rc.count(name[:-4] + '_BMP BITMAP "bitmaps/' + name + '"'), 1)
        for filename in ("NormalNormal.INI", "LargeFontsNormal.INI", "ExtraLargeNormal.INI"):
            ini = configparser.ConfigParser(strict=False, interpolation=None)
            ini.read(THEME / "textfiles" / filename)
            names = {name.lower(): name for name in ini.sections()}
            for palette in ("NORMAL", "LIGHT"):
                prefix = "" if palette == "NORMAL" else "ContentLight::"
                expected = GEN.bindings(palette, GEN.assets(palette, GEN.load_icons()))
                for part, values in expected.items():
                    section = names[(prefix + part).lower()]
                    for key, value in values.items():
                        self.assertEqual(ini.get(section, key).lower(), value.lower(), (filename, section, key))
                    options = dict(ini[section])
                    self.assertFalse(any(re.match(r"imagefile\d", key) for key in options), section)
                    self.assertNotIn("transparentcolor", options, section)
                    for key in ("imagefile", "glyphimagefile"):
                        if key in options:
                            resource = options[key].replace("\\", "_").upper()
                            resource = resource[:-4] + ".bmp"
                            self.assertIn(resource, MANIFEST)

    def test_deterministic_generation(self):
        command = [sys.executable, str(HERE / "modern_controls.py")]
        env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
        subprocess.run(command + ["--check"], check=True, capture_output=True, env=env)
        with tempfile.TemporaryDirectory(prefix="reliefwin-controls-") as folder:
            subprocess.run(command + [folder], check=True, capture_output=True, env=env)
            output = Path(folder)
            self.assertEqual({p.name for p in output.iterdir()}, set(MANIFEST))
            before = {p.name: (p.read_bytes(), p.stat().st_mtime_ns) for p in output.iterdir()}
            for name, (data, _) in before.items():
                self.assertEqual(data, (THEME / "bitmaps" / name).read_bytes())
            subprocess.run(command + [folder], check=True, capture_output=True, env=env)
            self.assertEqual(before, {p.name: (p.read_bytes(), p.stat().st_mtime_ns) for p in output.iterdir()})
            broken = output / "NORMAL_MODERNBUTTON.bmp"
            data = broken.read_bytes()
            broken.write_bytes(data[:-1] + bytes((data[-1] ^ 1,)))
            result = subprocess.run(command + [folder, "--check"], capture_output=True, env=env)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"NORMAL_MODERNBUTTON.bmp", result.stderr)
            self.assertNotEqual(broken.read_bytes(), data)


if __name__ == "__main__":
    unittest.main()
