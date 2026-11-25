# PROJECT:     ReliefWin visual style
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Generate rounded native control surfaces and Fluent glyphs
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
import hashlib
import io
import json
from pathlib import Path
import re
import struct

import cairosvg
from PIL import Image, ImageDraw


HERE = Path(__file__).resolve().parent
THEME = HERE.parent
SCALE = 4
ICONS = {
    "down": ("ic_fluent_chevron_down_12_regular.svg", "91dfae66b43cb13d5e869a90b43a0f0a40a24f7fa1e7e79f996ac3709483ed60"),
    "up": ("ic_fluent_chevron_up_12_regular.svg", "2b141c0979131530f9ae91f13d1388ba33bfe17e848dfaaccb93b86866b23d21"),
    "right": ("ic_fluent_chevron_right_12_regular.svg", "3a2b5a4d900ea90e03a87937d87c9a39db2a6490b5fd8cf8d2824ebf3f1b0b91"),
    "left": ("ic_fluent_chevron_left_12_regular.svg", "708c00270c632b1d5c7605ea832ace5b222f58dd382d018bacc05f178100b24c"),
    "check": ("ic_fluent_checkmark_12_regular.svg", "c2d6a0ffd428ecfb47c54d1a36b9c958b51300a407dac4947c43d973daae649a"),
    "mixed": ("ic_fluent_subtract_12_regular.svg", "1b2a6415d84d0cb9e1eda7fb9abfad3e6474eaad6a427ead9902a659e2d9eb64"),
}
PALETTES = {
    "NORMAL": {"button": (50, 53, 59), "hot": (61, 65, 72), "pressed": (43, 47, 54),
               "field": (32, 32, 32), "surface": (28, 32, 40), "border": (93, 97, 104),
               "disabled": (39, 43, 49), "disabled_border": (65, 69, 76),
               "text": (245, 245, 245), "disabled_text": (125, 129, 136),
               "accent": (76, 194, 255), "accent_text": (8, 37, 52), "track": (67, 71, 78)},
    "LIGHT": {"button": (251, 251, 251), "hot": (245, 245, 245), "pressed": (236, 236, 236),
              "field": (255, 255, 255), "surface": (243, 243, 243), "border": (180, 180, 180),
              "disabled": (245, 245, 245), "disabled_border": (213, 213, 213),
              "text": (26, 26, 26), "disabled_text": (161, 161, 161),
              "accent": (0, 103, 192), "accent_text": (255, 255, 255), "track": (205, 205, 205)},
}


def clean(image):
    pixels = list(image.getdata())
    image.putdata([pixel if pixel[3] else (0, 0, 0, 0) for pixel in pixels])
    return image


def surface(size, fill, border=None, radius=4, straight="", width=1):
    image = Image.new("RGBA", (size[0] * SCALE, size[1] * SCALE))
    draw = ImageDraw.Draw(image)

    def rounded(box, r, color):
        draw.rounded_rectangle(box, radius=max(0, r), fill=color)
        x0, y0, x1, y1 = box
        if "L" in straight:
            draw.rectangle((x0, y0, (x0 + x1) // 2, y1), fill=color)
        if "R" in straight:
            draw.rectangle(((x0 + x1) // 2, y0, x1, y1), fill=color)
        if "T" in straight:
            draw.rectangle((x0, y0, x1, (y0 + y1) // 2), fill=color)
        if "B" in straight:
            draw.rectangle((x0, (y0 + y1) // 2, x1, y1), fill=color)

    box = (0, 0, image.width - 1, image.height - 1)
    rounded(box, radius * SCALE, (*border, 255) if border else fill)
    if border:
        inset = width * SCALE
        rounded((inset, inset, image.width - 1 - inset, image.height - 1 - inset),
                max(0, radius * SCALE - inset), fill)
    return clean(image.resize(size, Image.LANCZOS))


def solid(color):
    return (*color, 255)


def load_icons():
    result = {}
    for name, (filename, digest) in ICONS.items():
        data = (HERE / "fluentui-system-icons" / filename).read_bytes()
        if hashlib.sha256(data).hexdigest() != digest:
            raise ValueError("Changed Fluent source: " + filename)
        with Image.open(io.BytesIO(cairosvg.svg2png(bytestring=data))) as image:
            result[name] = image.convert("RGBA").getchannel("A")
    return result


def glyph(mask, color):
    image = Image.new("RGBA", mask.size, solid(color))
    image.putalpha(mask)
    return clean(image)


def strip(frames):
    size = frames[0].size
    image = Image.new("RGBA", (size[0], size[1] * len(frames)))
    for state, frame in enumerate(frames):
        if frame.size != size:
            raise ValueError("Inconsistent state dimensions")
        image.paste(frame, (0, state * size[1]))
    return image


def bitmap(image):
    pixels = clean(image.copy()).tobytes("raw", "BGRA", 0, -1)
    dib = struct.pack("<IiiHHIIiiII", 40, image.width, image.height, 1, 32, 0,
                      len(pixels), 2835, 2835, 0, 0)
    return b"BM" + struct.pack("<IHHI", 54 + len(pixels), 0, 0, 54) + dib + pixels


def assets(palette, icons):
    p = PALETTES[palette]
    result = {}

    def add(name, frames, margins=(5, 5, 5, 5), radius=4):
        result[name] = {"image": strip(frames), "states": len(frames),
                        "size": list(frames[0].size), "margins": list(margins), "radius": radius}

    def buttons(size=(32, 24), straight=""):
        return [surface(size, solid(fill), border, straight=straight)
                for fill, border in ((p["button"], p["border"]), (p["hot"], p["border"]),
                                     (p["pressed"], p["border"]), (p["disabled"], p["disabled_border"]),
                                     (p["button"], p["accent"]))]

    add("Button", buttons())
    fields = [(p["field"], p["border"]), (p["field"], p["border"]),
              (p["field"], p["accent"]), (p["field"] if palette == "NORMAL" else p["disabled"], p["disabled_border"]),
              (p["field"], p["accent"]), (p["field"] if palette == "NORMAL" else p["disabled"], p["border"]),
              (p["field"], p["border"]), (p["field"], p["border"])]
    add("Edit", [surface((32, 24), solid(fill), border) for fill, border in fields])
    add("FieldBorder", [surface((32, 24), solid(fill), border) for fill, border in fields[:4]])
    add("Combo", [surface((32, 24), solid(fill), border) for fill, border in fields[:4]])
    add("ComboArrow", buttons((24, 24), "L")[:4])
    add("ComboArrowLeft", buttons((24, 24), "R")[:4])
    checks, radios = [], []
    for index in range(12):
        state, kind = index % 4, index // 4
        fill = p["disabled"] if state == 3 else p["hot"] if state == 1 else p["pressed"] if state == 2 else p["field"]
        border = p["disabled_border"] if state == 3 else p["border"]
        if kind:
            fill = p["disabled_border"] if state == 3 else p["accent"]
            border = fill
        image = surface((16, 16), solid(fill), border)
        if kind:
            icon = glyph(icons["check" if kind == 1 else "mixed"],
                         p["disabled_text"] if state == 3 else p["accent_text"])
            image.alpha_composite(icon, ((16 - icon.width) // 2, (16 - icon.height) // 2))
        checks.append(image)
        if kind < 2:
            image = surface((16, 16), solid(p["disabled"] if state == 3 else p["field"]),
                            border, radius=8, width=2 if kind else 1)
            if kind:
                dot = surface((8, 8), solid(fill), radius=4)
                image.alpha_composite(dot, (4, 4))
            radios.append(image)
    add("Check", checks, (0, 0, 0, 0))
    add("Radio", radios, (0, 0, 0, 0), 8)
    add("Group", [surface((32, 24), (0, 0, 0, 0), color)
                  for color in (p["border"], p["disabled_border"])])
    for axis, size in (("H", (24, 12)), ("V", (12, 24))):
        add("ScrollThumb" + axis, [surface(size, solid(color)) for color in
                                  (p["border"], p["disabled_text"], p["text"], p["disabled_border"])])
        add("ScrollTrack" + axis, [surface(size, solid(p["surface"])) for _ in range(4)])
    add("ScrollArrow", buttons((24, 24))[:4] * 4)
    add("ScrollGlyph", [glyph(icons[direction], p["disabled_text"] if state == 3 else p["text"])
                        for direction in ("up", "down", "left", "right") for state in range(4)], (0, 0, 0, 0), 0)
    for axis, size in (("H", (32, 12)), ("V", (12, 32))):
        add("ProgressTrack" + axis, [surface(size, solid(p["track"]))])
        add("ProgressFill" + axis, [surface(size, solid(color)) for color in
                                   (p["accent"], (196, 43, 28), (222, 150, 0), p["accent"])])
    tabs = [surface((32, 24), solid(fill), border, straight="B") for fill, border in
            ((p["surface"], p["border"]), (p["hot"], p["border"]), (p["button"], p["accent"]),
             (p["disabled"], p["disabled_border"]), (p["button"], p["accent"]))]
    add("Tab", tabs)
    add("Pane", [surface((32, 24), solid(p["surface"]), p["border"])])
    for direction, straight in (("Up", "B"), ("Down", "T"), ("Left", "R"), ("Right", "L")):
        add("Spin" + direction, buttons((24, 24), straight)[:4])
        add(direction + "Glyph", [glyph(icons[direction.lower()], p["disabled_text"] if state == 3 else p["text"])
                                  for state in range(4)], (0, 0, 0, 0), 0)
    add("TrackThumb", [surface((20, 20), solid(p["accent"] if state != 4 else p["disabled_border"]),
                               p["border"] if state != 3 else p["accent"], radius=10,
                               width=2 if state == 3 else 1) for state in range(5)], (0, 0, 0, 0), 10)
    add("TrackH", [surface((32, 8), solid(p["track"]))], (5, 5, 3, 3))
    add("TrackV", [surface((8, 32), solid(p["track"]))], (3, 3, 5, 5))
    for name, straight in (("Toolbar", ""), ("ToolbarSplit", "R"), ("ToolbarDrop", "L")):
        add(name, [surface((24, 24), (0, 0, 0, 0) if state in (0, 3) else
                           solid(p["hot"] if state in (1, 5) else p["pressed"]), straight=straight)
                   for state in range(6)])
    add("ToolbarGlyph", [glyph(icons["down"], p["disabled_text"] if state == 3 else p["text"])
                         for state in range(6)], (0, 0, 0, 0), 0)
    item_colors = ((49, 54, 63), (32, 65, 87), (50, 55, 64), (38, 79, 103)) if palette == "NORMAL" else \
                  ((235, 239, 245), (229, 241, 251), (234, 234, 234), (216, 235, 250))
    for name in ("ListItem", "TreeItem"):
        add(name, [Image.new("RGBA", (32, 24)), surface((32, 24), solid(item_colors[0])),
                   surface((32, 24), solid(item_colors[1]), p["accent"]), Image.new("RGBA", (32, 24)),
                   surface((32, 24), solid(item_colors[2]), p["border"]),
                   surface((32, 24), solid(item_colors[3]), p["accent"])])
    add("Empty", [Image.new("RGBA", (1, 1)) for _ in range(4)], (0, 0, 0, 0), 0)
    return result


def bindings(palette, generated):
    prefix = "Normal" if palette == "NORMAL" else "Light"
    p = PALETTES[palette]
    sections = {}

    def bind(section, name, content="3, 3, 2, 2", true_size=False, **extra):
        item = generated[name]
        values = {"BgType": "ImageFile", "ImageFile": prefix + "\\Modern" + name + ".bmp",
                  "ImageCount": str(item["states"]), "ImageLayout": "Vertical",
                  "SizingMargins": ", ".join(map(str, item["margins"])),
                  "SizingType": "TrueSize" if true_size else "Stretch", "Transparent": "True",
                  "ContentMargins": content, "TextColor": " ".join(map(str, p["text"])),
                  "TextShadowType": "None"}
        if true_size:
            values["UniformSizing"] = "True"
        values.update(extra)
        sections[section] = values

    def with_glyph(section, name, icon, **extra):
        bind(section, name, "0, 0, 0, 0", GlyphType="ImageGlyph", GlyphTransparent="True",
             GlyphImageFile=prefix + "\\Modern" + icon + ".bmp", **extra)

    bind("Button.Pushbutton", "Button", "5, 5, 4, 4")
    for state in ("Normal", "Hot", "Pressed", "Defaulted", "Disabled"):
        sections["Button.Pushbutton(" + state + ")"] = {"TextColor": " ".join(map(str, p["disabled_text"] if state == "Disabled" else p["text"]))}
    bind("Button.Checkbox", "Check", "0, 0, 0, 0", True)
    bind("Button.Radiobutton", "Radio", "0, 0, 0, 0", True)
    bind("Button.Groupbox", "Group", "5, 5, 5, 5", BorderOnly="True")
    for control, states in (("Checkbox", ("Uncheckeddisabled", "Checkeddisabled", "Mixeddisabled")),
                            ("Radiobutton", ("Uncheckeddisabled", "Checkeddisabled"))):
        for state in states:
            sections["Button." + control + "(" + state + ")"] = {"TextColor": " ".join(map(str, p["disabled_text"]))}
    bind("Edit", "Edit", "4, 4, 2, 2")
    bind("Edit.Edittext", "Edit", "4, 4, 2, 2")
    for kind in ("NOSCROLL", "HSCROLL", "VSCROLL", "HVSCROLL"):
        bind("Edit.Editborder_" + kind, "FieldBorder", "2, 2, 2, 2")
    for state in ("Disabled", "ReadOnly"):
        sections["Edit.Edittext(" + state + ")"] = {"TextColor": " ".join(map(str, p["disabled_text"] if state == "Disabled" else p["text"]))}
    bind("Combobox", "Combo", "4, 4, 2, 2")
    bind("Combobox.Border", "Combo", "2, 2, 2, 2")
    bind("Combobox.ReadOnly", "Combo", "4, 4, 2, 2")
    for state in ("Disabled", "Hot"):
        sections["Combobox(" + state + ")"] = {"TextColor": " ".join(map(str, p["disabled_text"] if state == "Disabled" else p["text"]))}
    for part in ("DropDownButton", "DropDownButtonRight", "DropDownButtonLeft"):
        with_glyph("Combobox." + part, "ComboArrowLeft" if part.endswith("Left") else "ComboArrow", "DownGlyph")
    for part in ("LowerTrackHorz", "UpperTrackHorz", "LowerTrackVert", "UpperTrackVert"):
        bind("ScrollBar." + part, "ScrollTrackH" if part.endswith("Horz") else "ScrollTrackV", "0, 0, 0, 0")
    bind("ScrollBar.ThumbBtnHorz", "ScrollThumbH", "0, 0, 0, 0")
    bind("ScrollBar.ThumbBtnVert", "ScrollThumbV", "0, 0, 0, 0")
    with_glyph("ScrollBar.ArrowBtn", "ScrollArrow", "ScrollGlyph")
    bind("ScrollBar.GripperHorz", "Empty", "0, 0, 0, 0", True)
    bind("ScrollBar.GripperVert", "Empty", "0, 0, 0, 0", True)
    for part, name in (("Bar", "ProgressTrackH"), ("BarVert", "ProgressTrackV"),
                       ("Chunk", "ProgressFillH"), ("ChunkVert", "ProgressFillV"),
                       ("Fill", "ProgressFillH"), ("FillVert", "ProgressFillV")):
        bind("Progress." + part, name, "0, 0, 0, 0")
    for suffix in ("TabItem", "TabItemBothEdge", "TabItemLeftEdge", "TabItemRightEdge",
                   "TopTabItem", "TopTabItemBothEdge", "TopTabItemLeftEdge", "TopTabItemRightEdge"):
        bind("Tab." + suffix, "Tab", "6, 6, 3, 3")
    bind("Tab.Pane", "Pane", "5, 5, 5, 5")
    sections["Tab.Body"] = {"BgType": "BorderFill", "BorderSize": "0", "FillColor": " ".join(map(str, p["surface"])), "TextShadowType": "None"}
    sections["Spin"] = {"BgType": "None"}
    for part, direction in (("Up", "Up"), ("Down", "Down"), ("Uphorz", "Right"), ("Downhorz", "Left")):
        with_glyph("Spin." + part, "Spin" + direction, direction + "Glyph")
    for part in ("Thumb", "ThumbBottom", "ThumbTop", "ThumbLeft", "ThumbRight", "ThumbVert"):
        bind("TrackBar." + part, "TrackThumb", "0, 0, 0, 0", True)
    bind("TrackBar.Track", "TrackH", "0, 0, 0, 0")
    bind("TrackBar.TrackVert", "TrackV", "0, 0, 0, 0")
    for part, name in (("Button", "Toolbar"), ("DropdownButton", "Toolbar"), ("SplitButton", "ToolbarSplit")):
        bind("Toolbar." + part, name, "3, 3, 3, 3")
    with_glyph("Toolbar.SplitButtonDropdown", "ToolbarDrop", "ToolbarGlyph")
    bind("Toolbar.DropdownButtonGlyph", "ToolbarGlyph", "0, 0, 0, 0", True)
    if palette == "NORMAL":
        for part in ("Button", "DropdownButton", "SplitButton", "SplitButtonDropdown"):
            sections["ExplorerBar::Toolbar." + part] = dict(sections["Toolbar." + part])
        sections["Placesbar::Toolbar.Button"] = dict(sections["Toolbar.Button"])
    bind("ListView.ListItem", "ListItem", "3, 3, 2, 2")
    bind("TreeView.TreeItem", "TreeItem", "3, 3, 2, 2")
    for section in ("ListView.ListItem", "TreeView.TreeItem"):
        sections[section + "(Disabled)"] = {"TextColor": " ".join(map(str, p["disabled_text"]))}
    for section in ("Button.Checkbox", "Button.Radiobutton", "TrackBar.Thumb", "TrackBar.ThumbVert",
                    "TrackBar.ThumbBottom", "TrackBar.ThumbTop", "TrackBar.ThumbLeft", "TrackBar.ThumbRight"):
        sections[section]["TrueSizeScalingType"] = "Dpi"
        sections[section]["TrueSizeStretchMark"] = "50"
    return sections


def update_ini(text, replacements):
    blocks = re.split(r"(?m)(^\[[^\r\n]+\]\r?$)", text)
    used = set()
    result = [blocks[0]]
    for index in range(1, len(blocks), 2):
        header, body = blocks[index], blocks[index + 1]
        name = header.strip()[1:-1]
        match = next((key for key in replacements if key.lower() == name.lower()), None)
        if match is None:
            result.extend((header, body))
            continue
        values = dict(replacements[match])
        font = re.search(r"(?im)^font\s*=\s*([^\r\n]+)", body)
        if font:
            values["Font"] = font.group(1)
        result.append("[" + name + "]\n" + "\n".join(key + " = " + value for key, value in values.items()) + "\n\n")
        used.add(match)
    for name, values in replacements.items():
        if name not in used:
            result.append("[" + name + "]\n" + "\n".join(key + " = " + value for key, value in values.items()) + "\n\n")
    return "".join(result).rstrip() + "\n"


def main():
    parser = argparse.ArgumentParser(description="Generate rounded native Win32 control assets")
    parser.add_argument("output", nargs="?", type=Path, default=THEME / "bitmaps")
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--update-theme", action="store_true")
    args = parser.parse_args()
    icons = load_icons()
    manifest, replacements, resources, failures = {}, {}, [], []
    for palette in PALETTES:
        generated = assets(palette, icons)
        prefix = "" if palette == "NORMAL" else "ContentLight::"
        replacements.update({prefix + key: value for key, value in bindings(palette, generated).items()})
        for name, item in generated.items():
            filename = palette + "_MODERN" + name.upper() + ".bmp"
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
    if args.update_theme:
        if args.check:
            parser.error("--check and --update-theme cannot be combined")
        for path in (THEME / "textfiles").glob("*.INI"):
            if path.name.endswith("_utf16.INI"):
                continue
            text = path.read_text()
            updated = update_ini(text, replacements)
            with path.open("w", newline="\r\n") as stream:
                stream.write(updated)
        rc = THEME / "win8.rc"
        text = rc.read_text()
        text = re.sub(r'(?m)^(?:NORMAL|LIGHT)_MODERN\w+_BMP BITMAP[^\n]*\n?', '', text)
        rc.write_text(text.rstrip() + "\n\n" + "\n".join(resources) + "\n")
        (HERE / "modern_controls.json").write_text(json.dumps(manifest, indent=2) + "\n")
    if failures:
        parser.exit(1, "Rounded assets differ or are missing:\n" + "\n".join(failures) + "\n")
    print(str(len(manifest)) + " modern control bitmaps " + ("verified" if args.check else "generated"))


if __name__ == "__main__":
    main()
