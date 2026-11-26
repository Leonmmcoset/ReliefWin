# PROJECT:     ReliefWin visual style
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Generate shared menu geometry, Fluent masks and theme surfaces
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
import hashlib
import io
import json
from pathlib import Path
import re

import cairosvg
from PIL import Image, ImageDraw

from modern_controls import bitmap, clean, glyph, solid, strip, surface, update_ini


HERE = Path(__file__).resolve().parent
THEME = HERE.parent
ROOT = HERE.parents[4]
HEADER = ROOT / "sdk/include/reliefwin/menuart.h"
GEOMETRY = {
    "POPUP_RADIUS": 8,
    "POPUP_INSET": 6,
    "ITEM_RADIUS": 4,
    "ROW_MIN": 28,
    "ROW_MARGIN": 6,
    "SEPARATOR_HEIGHT": 9,
    "GLYPH_SIZE": 12,
}
ICONS = {
    "Checkmark": ("ic_fluent_checkmark_12_regular.svg", "c2d6a0ffd428ecfb47c54d1a36b9c958b51300a407dac4947c43d973daae649a"),
    "ChevronRight": ("ic_fluent_chevron_right_12_regular.svg", "3a2b5a4d900ea90e03a87937d87c9a39db2a6490b5fd8cf8d2824ebf3f1b0b91"),
    "ChevronLeft": ("ic_fluent_chevron_left_12_regular.svg", "708c00270c632b1d5c7605ea832ace5b222f58dd382d018bacc05f178100b24c"),
    "ChevronUp": ("ic_fluent_chevron_up_12_regular.svg", "2b141c0979131530f9ae91f13d1388ba33bfe17e848dfaaccb93b86866b23d21"),
    "ChevronDown": ("ic_fluent_chevron_down_12_regular.svg", "91dfae66b43cb13d5e869a90b43a0f0a40a24f7fa1e7e79f996ac3709483ed60"),
    "CircleFill": ("ic_fluent_circle_12_filled.svg", "8b4bbbbe4a87608f37cfcd45e046698b91137ee09bb8549c1b38b6d962ff542d"),
}
PALETTES = {
    "NORMAL": {"background": (32, 32, 32), "bar": (32, 32, 32), "border": (67, 67, 67),
               "hot": (49, 49, 49), "pressed": (56, 56, 56), "disabled_hot": (42, 42, 42),
               "text": (240, 240, 240), "disabled_text": (128, 128, 128),
               "bar_disabled_text": (128, 128, 128), "separator": (62, 62, 62)},
    "LIGHT": {"background": (249, 249, 249), "bar": (243, 243, 243), "border": (220, 220, 220),
              "hot": (237, 237, 237), "pressed": (229, 229, 229), "disabled_hot": (243, 243, 243),
              "text": (0, 0, 0), "disabled_text": (159, 159, 159),
              "bar_disabled_text": (109, 109, 109), "separator": (224, 224, 224)},
}


def load_icons():
    result = {}
    for name, (filename, digest) in ICONS.items():
        data = (HERE / "fluentui-system-icons" / filename).read_bytes()
        if hashlib.sha256(data).hexdigest() != digest:
            raise ValueError("Changed Fluent source: " + filename)
        with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as image:
            if image.size != (GEOMETRY["GLYPH_SIZE"], GEOMETRY["GLYPH_SIZE"]):
                raise ValueError("Changed Fluent native size: " + filename)
            result[name] = image.convert("RGBA").getchannel("A")
    return result


def shared_header(icons):
    lines = ["/*", " * PROJECT:     ReliefWin user interface",
             " * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)",
             " * PURPOSE:     Shared menu geometry and original native Fluent glyph coverage",
             " * COPYRIGHT:   Copyright 2026 ReliefWin contributors", " */", "", "#pragma once", ""]
    lines.extend("#define RELIEF_MENU_" + key + " " + str(value) for key, value in GEOMETRY.items())
    for name, image in icons.items():
        values = list(image.getdata())
        lines.extend(("", "static const unsigned char ReliefMenu" + name + "Alpha[144] =", "{"))
        lines.extend("    " + ", ".join(str(value) for value in values[y:y + 12]) + ","
                     for y in range(0, len(values), 12))
        lines.append("};")
    return "\n".join(lines) + "\n"


def assets(palette, icons):
    p = PALETTES[palette]
    result = {}

    def add(name, frames, margins=(0, 0, 0, 0), radius=0):
        result[name] = {"image": strip(frames), "size": list(frames[0].size),
                        "states": len(frames), "margins": list(margins), "radius": radius}

    add("PopupBackground", [surface((40, 40), solid(p["background"]), p["border"], radius=8)], (9, 9, 9, 9), 8)
    add("PopupBorders", [surface((40, 40), (0, 0, 0, 0), p["border"], radius=8)], (9, 9, 9, 9), 8)
    add("PopupItem", [Image.new("RGBA", (32, 28)), surface((32, 28), solid(p["hot"])),
                      Image.new("RGBA", (32, 28)), surface((32, 28), solid(p["disabled_hot"]))], (5, 5, 5, 5), 4)
    add("BarItem", [Image.new("RGBA", (32, 28)), surface((32, 28), solid(p["hot"])),
                    surface((32, 28), solid(p["pressed"])), Image.new("RGBA", (32, 28)),
                    surface((32, 28), solid(p["disabled_hot"])),
                    surface((32, 28), solid(p["disabled_hot"]))], (5, 5, 5, 5), 4)
    add("PopupCheck", [glyph(icons[name], p["disabled_text"] if disabled else p["text"])
                       for name in ("Checkmark", "CircleFill") for disabled in (False, True)])
    add("PopupSubmenu", [glyph(icons["ChevronRight"], p["disabled_text"] if disabled else p["text"])
                         for disabled in (False, True)])
    separator = Image.new("RGBA", (32, GEOMETRY["SEPARATOR_HEIGHT"]))
    ImageDraw.Draw(separator).line((0, 4, 31, 4), fill=solid(p["separator"]))
    add("PopupSeparator", [separator], (0, 0, 4, 4))
    add("PopupCheckBackground", [Image.new("RGBA", (16, 16)) for _ in range(3)])
    add("PopupGutter", [Image.new("RGBA", (1, 1), solid(p["background"]))])
    add("BarBackground", [Image.new("RGBA", (1, 1), solid(p["bar"])) for _ in range(2)])
    return result


def bindings(palette, generated):
    p = PALETTES[palette]
    prefix = "Normal" if palette == "NORMAL" else "Light"
    sections = {}

    def bind(part, content="0, 0, 0, 0", true_size=False, **extra):
        item = generated[part]
        values = {"BgType": "ImageFile", "ImageFile": prefix + "\\Menu" + part + ".bmp",
                  "ImageCount": str(item["states"]), "ImageLayout": "Vertical",
                  "SizingMargins": ", ".join(map(str, item["margins"])),
                  "SizingType": "TrueSize" if true_size else "Stretch", "Transparent": "True",
                  "ContentMargins": content, "TextColor": " ".join(map(str, p["text"])),
                  "TextShadowType": "None"}
        if true_size:
            values.update({"UniformSizing": "True", "TrueSizeScalingType": "Dpi", "TrueSizeStretchMark": "50"})
        values.update(extra)
        sections["Menu." + part] = values

    bind("PopupBackground", "6, 6, 6, 6")
    bind("PopupBorders", "6, 6, 6, 6", BorderOnly="True")
    bind("PopupItem", "8, 8, 6, 6")
    bind("BarItem", "8, 8, 6, 6")
    bind("PopupCheck", true_size=True)
    bind("PopupSubmenu", true_size=True, MirrorImage="True")
    bind("PopupSeparator")
    bind("PopupCheckBackground")
    bind("PopupGutter")
    bind("BarBackground")
    for part, disabled in (("PopupItem", p["disabled_text"]), ("BarItem", p["bar_disabled_text"])):
        states = ("Disabled", "DisabledHot", "DisabledPushed") if part == "BarItem" else ("Disabled", "DisabledHot")
        for state in states:
            sections["Menu." + part + "(" + state + ")"] = {"TextColor": " ".join(map(str, disabled))}
    sections["Menu.PopupItem(Hot)"] = {"TextColor": " ".join(map(str, p["text"]))}
    return sections


def main():
    parser = argparse.ArgumentParser(description="Generate modern menu assets and shared native glyph masks")
    parser.add_argument("output", nargs="?", type=Path, default=THEME / "bitmaps")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--update-theme", action="store_true")
    args = parser.parse_args()
    if args.check and args.update_theme:
        parser.error("--check and --update-theme cannot be combined")
    icons = load_icons()
    manifest, replacements, resources, failures = {}, {}, [], []
    for palette in PALETTES:
        generated = assets(palette, icons)
        prefix = "" if palette == "NORMAL" else "ContentLight::"
        replacements.update({prefix + key: value for key, value in bindings(palette, generated).items()})
        for name, item in generated.items():
            filename = palette + "_MENU" + name.upper() + ".bmp"
            data = bitmap(item["image"])
            path = args.output / filename
            if not path.is_file() or path.read_bytes() != data:
                if args.check:
                    failures.append(filename)
                else:
                    path.write_bytes(data)
            manifest[filename] = {key: value for key, value in item.items() if key != "image"}
            manifest[filename]["sha256"] = hashlib.sha256(data).hexdigest()
            resources.append(filename[:-4] + '_BMP BITMAP "bitmaps/' + filename + '"')
    header = shared_header(icons)
    if args.check and (not HEADER.is_file() or HEADER.read_text() != header):
        failures.append(str(HEADER.relative_to(ROOT)))
    if args.update_theme:
        HEADER.write_text(header)
        for path in (THEME / "textfiles").glob("*.INI"):
            if path.name.endswith("_utf16.INI"):
                continue
            updated = update_ini(path.read_text(), replacements)
            with path.open("w", newline="\r\n") as stream:
                stream.write(updated)
        rc = THEME / "win8.rc"
        text = re.sub(r'(?m)^(?:NORMAL|LIGHT)_MENU\w+_BMP BITMAP[^\n]*\n?', '', rc.read_text())
        rc.write_text(text.rstrip() + "\n\n" + "\n".join(resources) + "\n")
        (HERE / "modern_menus.json").write_text(json.dumps(manifest, indent=2) + "\n")
    if failures:
        parser.exit(1, "Menu artifacts differ or are missing:\n" + "\n".join(failures) + "\n")
    print(str(len(manifest)) + " modern menu bitmaps " + ("verified" if args.check else "generated"))


if __name__ == "__main__":
    main()
