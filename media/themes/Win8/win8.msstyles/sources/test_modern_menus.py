# PROJECT:     ReliefWin visual style tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify generated menu assets and shared original Fluent masks
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import configparser
import hashlib
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

import modern_menus as GEN


HERE = Path(__file__).resolve().parent
THEME = HERE.parent
MANIFEST = json.loads((HERE / "modern_menus.json").read_text())


def read_bitmap(name):
    data = (THEME / "bitmaps" / name).read_bytes()
    width, height = struct.unpack_from("<ii", data, 18)
    return data, Image.frombytes("RGBA", (width, height), data[54:], "raw", "BGRA", 0, -1)


class MenuAssets(unittest.TestCase):
    def test_actual_bitmap_format_and_transparency(self):
        self.assertEqual(len(MANIFEST), 20)
        for name, item in MANIFEST.items():
            with self.subTest(asset=name):
                data, image = read_bitmap(name)
                w, h = item["size"]
                self.assertEqual(image.size, (w, h * item["states"]))
                self.assertEqual(data[:2], b"BM")
                self.assertEqual(struct.unpack_from("<I", data, 2)[0], len(data))
                self.assertEqual(struct.unpack_from("<I", data, 10)[0], 54)
                self.assertEqual(struct.unpack_from("<IiiHHI", data, 14), (40, w, h * item["states"], 1, 32, 0))
                self.assertEqual(len(data), 54 + w * h * item["states"] * 4)
                self.assertEqual(hashlib.sha256(data).hexdigest(), item["sha256"])
                self.assertTrue(all(pixel[:3] == (0, 0, 0) for pixel in image.getdata() if not pixel[3]))
                self.assertFalse(any(pixel[:3] == (255, 0, 255) for pixel in image.getdata()))
                if item["radius"]:
                    self.assertTrue(any(0 < pixel[3] < 255 for pixel in image.getdata()))
                    for state in range(item["states"]):
                        for x, y in ((0, 0), (w - 1, 0), (0, h - 1), (w - 1, h - 1)):
                            self.assertEqual(image.getpixel((x, state * h + y)), (0, 0, 0, 0))

    def test_menu_geometry_and_state_order(self):
        shapes = {"POPUPBACKGROUND": (40, 40, 1, 8), "POPUPBORDERS": (40, 40, 1, 8),
                  "POPUPITEM": (32, 28, 4, 4), "BARITEM": (32, 28, 6, 4),
                  "POPUPCHECK": (12, 12, 4, 0), "POPUPSUBMENU": (12, 12, 2, 0),
                  "POPUPSEPARATOR": (32, 9, 1, 0), "POPUPCHECKBACKGROUND": (16, 16, 3, 0),
                  "POPUPGUTTER": (1, 1, 1, 0), "BARBACKGROUND": (1, 1, 2, 0)}
        centers = {"NORMAL": ((32, 32, 32), (49, 49, 49), (56, 56, 56), (42, 42, 42)),
                   "LIGHT": ((249, 249, 249), (237, 237, 237), (229, 229, 229), (243, 243, 243))}
        for palette in centers:
            for part, expected in shapes.items():
                item = MANIFEST[palette + "_MENU" + part + ".bmp"]
                self.assertEqual((*item["size"], item["states"], item["radius"]), expected)
            _, panel = read_bitmap(palette + "_MENUPOPUPBACKGROUND.bmp")
            self.assertEqual(panel.getpixel((20, 20)), (*centers[palette][0], 255))
            self.assertEqual(panel.getpixel((20, 0))[3], 255)
            _, border = read_bitmap(palette + "_MENUPOPUPBORDERS.bmp")
            self.assertEqual(border.getpixel((20, 20)), (0, 0, 0, 0))
            _, items = read_bitmap(palette + "_MENUPOPUPITEM.bmp")
            for state in (0, 2):
                self.assertEqual(set(items.crop((0, state * 28, 32, (state + 1) * 28)).getdata()), {(0, 0, 0, 0)})
            for state, color in ((1, centers[palette][1]), (3, centers[palette][3])):
                self.assertEqual(items.getpixel((16, state * 28 + 14)), (*color, 255))
            _, bar = read_bitmap(palette + "_MENUBARITEM.bmp")
            for state in (0, 3):
                self.assertEqual(set(bar.crop((0, state * 28, 32, (state + 1) * 28)).getdata()), {(0, 0, 0, 0)})
            self.assertEqual(bar.getpixel((16, 2 * 28 + 14)), (*centers[palette][2], 255))
            _, separator = read_bitmap(palette + "_MENUPOPUPSEPARATOR.bmp")
            for y in range(9):
                self.assertEqual(set(separator.getchannel("A").crop((0, y, 32, y + 1)).getdata()), {255 if y == 4 else 0})

    def test_unmodified_native_fluent_coverage_and_header(self):
        text = GEN.HEADER.read_text()
        geometry = {"POPUP_RADIUS": 8, "POPUP_INSET": 6, "ITEM_RADIUS": 4, "ROW_MIN": 28,
                    "ROW_MARGIN": 6, "SEPARATOR_HEIGHT": 9, "GLYPH_SIZE": 12}
        for key, value in geometry.items():
            self.assertRegex(text, r"(?m)^#define RELIEF_MENU_" + key + " " + str(value) + "$")
        for name, (filename, digest) in GEN.ICONS.items():
            data = (HERE / "fluentui-system-icons" / filename).read_bytes()
            self.assertEqual(hashlib.sha256(data).hexdigest(), digest)
            with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as image:
                self.assertEqual(image.size, (12, 12))
                mask = image.convert("RGBA").getchannel("A")
            body = re.search(r"ReliefMenu" + name + r"Alpha\[144\] =\s*\{([^}]+)\}", text)
            self.assertIsNotNone(body, name)
            actual = bytes(int(value) for value in re.findall(r"\d+", body.group(1)))
            self.assertEqual(actual, mask.tobytes(), name)
            if name not in ("Checkmark", "CircleFill", "ChevronRight"):
                continue
            part = "POPUPSUBMENU" if name == "ChevronRight" else "POPUPCHECK"
            offset = 2 if name == "CircleFill" else 0
            for palette in ("NORMAL", "LIGHT"):
                _, atlas = read_bitmap(palette + "_MENU" + part + ".bmp")
                for state in range(2):
                    actual = atlas.crop((0, (offset + state) * 12, 12, (offset + state + 1) * 12))
                    self.assertEqual(actual.getchannel("A").tobytes(), mask.tobytes())

    def test_three_theme_variants_and_registered_assets(self):
        rc = (THEME / "win8.rc").read_text()
        for name in MANIFEST:
            self.assertEqual(rc.count(name[:-4] + '_BMP BITMAP "bitmaps/' + name + '"'), 1)
        for filename in ("NormalNormal.INI", "LargeFontsNormal.INI", "ExtraLargeNormal.INI"):
            ini = configparser.ConfigParser(strict=False, interpolation=None)
            ini.read(THEME / "textfiles" / filename)
            names = {name.lower(): name for name in ini.sections()}
            for prefix, palette in (("", "NORMAL"), ("ContentLight::", "LIGHT")):
                for part, values in GEN.bindings(palette, GEN.assets(palette, GEN.load_icons())).items():
                    section = names[(prefix + part).lower()]
                    for key, value in values.items():
                        self.assertEqual(ini.get(section, key).lower(), value.lower(), (filename, section, key))
                    if "ImageFile" in values:
                        resource = values["ImageFile"].replace("\\", "_").upper()[:-4] + ".bmp"
                        self.assertIn(resource, MANIFEST)
                    self.assertFalse(ini.has_option(section, "TransparentColor"))
                for part in ("PopupCheck", "PopupSubmenu"):
                    section = names[(prefix + "Menu." + part).lower()]
                    self.assertEqual(ini.get(section, "SizingType").lower(), "truesize")
                    self.assertTrue(ini.getboolean(section, "UniformSizing"))
                    self.assertEqual(ini.get(section, "TrueSizeScalingType").lower(), "dpi")
                for part, margins in (("PopupBackground", "6, 6, 6, 6"), ("PopupItem", "8, 8, 6, 6")):
                    self.assertEqual(ini.get(names[(prefix + "Menu." + part).lower()], "ContentMargins"), margins)

    def test_deterministic_artifacts_and_corruption_rejection(self):
        command = [sys.executable, str(HERE / "modern_menus.py")]
        env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
        subprocess.run(command + ["--check"], check=True, capture_output=True, env=env)
        with tempfile.TemporaryDirectory() as folder:
            subprocess.run(command + [folder], check=True, capture_output=True, env=env)
            output = Path(folder)
            self.assertEqual({path.name for path in output.iterdir()}, set(MANIFEST))
            before = {path.name: (path.read_bytes(), path.stat().st_mtime_ns) for path in output.iterdir()}
            for name, (data, _) in before.items():
                self.assertEqual(data, (THEME / "bitmaps" / name).read_bytes())
            subprocess.run(command + [folder], check=True, capture_output=True, env=env)
            self.assertEqual(before, {path.name: (path.read_bytes(), path.stat().st_mtime_ns) for path in output.iterdir()})
            broken = output / "NORMAL_MENUPOPUPBACKGROUND.bmp"
            data = broken.read_bytes()
            broken.write_bytes(data[:-1] + bytes((data[-1] ^ 1,)))
            result = subprocess.run(command + [folder, "--check"], capture_output=True, env=env)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"NORMAL_MENUPOPUPBACKGROUND.bmp", result.stderr)
            self.assertNotEqual(broken.read_bytes(), data)


if __name__ == "__main__":
    unittest.main()
