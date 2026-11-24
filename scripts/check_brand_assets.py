#!/usr/bin/env python3
"""
PROJECT:     ReliefWin branding tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Verify system image contracts and the supplied ReliefWin mark
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import hashlib
import json
import struct
import subprocess
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]


def check(condition, message):
    if not condition:
        raise AssertionError(message)


def rle4(path, expected_size):
    data = (ROOT / path).read_bytes()
    check(data[:2] == b"BM", path + ": invalid bitmap signature")
    size, width, height, planes, depth, compression = struct.unpack_from("<IiiHHI", data, 14)
    check(size == 40 and planes == 1 and depth == 4 and compression == 2,
          path + ": bootvid requires a 4-bit RLE4 bitmap")
    check((width, height) == expected_size, path + ": wrong resource dimensions")
    offset, = struct.unpack_from("<I", data, 10)
    x, y, count = 0, 0, 0
    pixels = bytearray(width * height)
    while True:
        check(offset + 2 <= len(data), path + ": missing RLE terminator")
        length, code = data[offset:offset + 2]
        offset += 2
        if length:
            values = [(code >> 4) if index % 2 == 0 else (code & 15) for index in range(length)]
        elif code == 0:
            check(x == width, path + ": incomplete row")
            x, y = 0, y + 1
            continue
        elif code == 1:
            check(y == height and x == 0, path + ": incomplete bitmap")
            break
        elif code == 2:
            raise AssertionError(path + ": unexpected RLE delta")
        else:
            length = code
            size = (length + 1) // 2
            check(offset + size <= len(data), path + ": truncated literal data")
            values = [(data[offset + index // 2] >> 4) if index % 2 == 0 else
                      (data[offset + index // 2] & 15) for index in range(length)]
            offset += size + (size & 1)
        check(y < height and x + length <= width, path + ": pixels exceed the bitmap")
        start = (height - y - 1) * width + x
        pixels[start:start + length] = bytes(values)
        x += length
        count += length
    check(count == width * height and offset == len(data), path + ": incorrect decoded extent")
    check(len(set(pixels)) > 1, path + ": blank bitmap")


def main():
    manifest = json.loads((ROOT / "media/graphics/branding/assets.json").read_text())
    source = ROOT / manifest["source"]
    check(hashlib.sha256(source.read_bytes()).hexdigest() == manifest["source_sha256"],
          "The original source image changed")
    for path, checksum in manifest["outputs"].items():
        check(hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == checksum,
              path + ": resource differs from the generation manifest")

    obsolete = json.loads((ROOT / "media/graphics/branding/obsolete-assets.json").read_text())
    files = subprocess.check_output(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT)
    suffixes = {".bmp", ".gif", ".ico", ".jpeg", ".jpg", ".png", ".rgba", ".svg", ".webp"}
    scanned = 0
    for name in set(files.decode().split("\0")):
        path = ROOT / name
        if path.suffix.lower() not in suffixes or not path.is_file():
            continue
        checksum = hashlib.sha256(path.read_bytes()).hexdigest()
        check(checksum not in obsolete,
              name + ": obsolete artwork reintroduced from " + ", ".join(obsolete.get(checksum, [])))
        scanned += 1

    mark = Image.open(ROOT / "media/graphics/branding/reliefwin-mark.png")
    check(mark.mode == "RGBA" and mark.size == (478, 460), "The source mark's aspect ratio changed")
    for point in ((0, 0), (125, 200), (300, 50)):
        check(mark.getpixel(point)[3] == 0, "The leaf's background or negative space became opaque")
    light, dark = mark.getpixel((100, 100)), mark.getpixel((400, 300))
    check(light[3] == dark[3] == 255 and light[2] > dark[2] + 40 and
          light[0] < 10 and dark[0] < 10, "The supplied mark's two blue shapes changed")

    raw = (ROOT / "bootloader/gui/assets/logo.rgba").read_bytes()
    check(len(raw) == 48 * 42 * 4, "The boot menu logo has the wrong extent")
    check(all(max(raw[i:i + 3]) <= raw[i + 3] for i in range(0, len(raw), 4)),
          "The boot menu logo is not premultiplied RGBA")
    check(any(raw[3::4]) and min(raw[3::4]) == 0, "The boot menu logo lost its transparency")

    for folder in ("base/system/userinit/res", "dll/cpl/sysdm/resources"):
        color = Image.open(ROOT / folder / "rosbitmap.bmp")
        mask = Image.open(ROOT / folder / "rosbitmap_mask.bmp")
        check(color.mode == "RGB" and mask.mode == "L" and
              color.size == mask.size == (320, 159), folder + ": color/alpha contract changed")
        check(mask.getextrema() == (255, 255),
              folder + ": the wordmark must retain its light backing on dark themes")
        check(color.getpixel((0, 0)) == color.getpixel((319, 158)) == (238, 244, 250),
              folder + ": the wordmark lost its contrasting light background")

    bitmap = (ROOT / "ntos/inbv/resources/reliefwin_logo.bmp").read_bytes()
    check(struct.unpack_from("<IiiHHI", bitmap, 14) == (108, 652, 120, 1, 32, 3),
          "The GOP bitmap's shape, depth or bitfield encoding changed")
    check(struct.unpack_from("<IIII", bitmap, 54) ==
          (0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000),
          "The GOP bitmap lost its explicit BGRA masks")
    rle4("ntos/inbv/resources/logo.bmp", (190, 224))
    rle4("ntos/inbv/resources/copyright.bmp", (260, 26))
    rle4("ntos/inbv/resources/shutdown.bmp", (213, 11))
    palettes = []
    for name in ("logo", "copyright", "shutdown"):
        data = (ROOT / "ntos/inbv/resources" / (name + ".bmp")).read_bytes()
        header_size, = struct.unpack_from("<I", data, 14)
        palettes.append(data[14 + header_size:14 + header_size + 64])
    check(all(palette[:4] == bytes(4) for palette in palettes),
          "The VGA background index must remain black across all boot resources")
    check(len(set(palettes)) == 1,
          "All VGA boot resources must share one palette so later draws preserve earlier colors")
    check(all(channel >= 240 for channel in palettes[0][60:63]),
          "The VGA text foreground index must remain white")

    for state in ("default", "hover", "pressed"):
        image = Image.open(ROOT / "media/graphics/start-button" / (state + ".png"))
        check(image.mode == "RGBA" and image.size == (640, 640), "The Start button canvas changed")
        check(image.getpixel((0, 0))[3] == 0 and image.getbbox() is not None,
              "The Start button has no transparent margin or artwork")
    for path, expected_size, expected_depth in (
        ("base/shell/explorer/res/bmp/143.bmp", (25, 20), 32),
        ("base/shell/explorer/res/bmp/170.bmp", (300, 180), 8),
        ("base/shell/explorer/res/bmp/171.bmp", (300, 180), 8),
    ):
        data = (ROOT / path).read_bytes()
        check(struct.unpack_from("<ii", data, 18) == expected_size and
              struct.unpack_from("<H", data, 28)[0] == expected_depth,
              path + ": the preview resource's dimensions or depth changed")
    icon = Image.open(ROOT / "image/data/icon.ico")
    check(icon.info["sizes"] == {(size, size) for size in (16, 24, 32, 48, 64, 128, 256)},
          "The system brand icon is missing a display size")
    for size in icon.info["sizes"]:
        frame = icon.ico.getimage(size)
        check(frame.mode == "RGBA" and frame.getpixel((0, 0))[3] == 0,
              "A system icon frame has an opaque margin")
    for path in ("media/graphics/branding/libernt-mark.svg", "media/graphics/branding/libernt-logo.png",
                 "media/graphics/branding/libernt-banner.png", "media/graphics/start_menu_banner.svg",
                 "media/graphics/start-button/reactos-start-color.svg",
                 "media/graphics/start-button/reactos-start-hover.svg",
                 "media/graphics/start-button/reactos-start-pressed.svg",
                 "dll/cpl/sysdm/resources/reactos-banner.svg"):
        check(not (ROOT / path).exists(), path + ": obsolete artwork remains")
    print("ReliefWin artwork: %d resources verified; %d source images scanned for obsolete artwork; source, negative space, color, alpha, RGBA, BGRA and RLE4 checks passed" %
          (len(manifest["outputs"]), scanned))


if __name__ == "__main__":
    main()
