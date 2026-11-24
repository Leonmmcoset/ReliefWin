#!/usr/bin/env python3
"""
PROJECT:     ReliefWin branding tools
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Generate system brand resources from the maintainer's image
COPYRIGHT:   Copyright 2026 ReliefWin contributors
"""

import hashlib
import io
import json
import struct
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont, ImageOps


ROOT = Path(__file__).resolve().parents[1]
BRANDING = ROOT / "media/graphics/branding"
SOURCE = BRANDING / "reliefwin-source.jpg"
RESAMPLE = getattr(Image, "Resampling", Image).LANCZOS
TRANSPOSE = getattr(Image, "Transpose", Image)
OUTPUTS = {}


def write(path, data):
    target = ROOT / path
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_bytes(data)
    OUTPUTS[path] = hashlib.sha256(data).hexdigest()


def save(path, image, image_format=None, **options):
    stream = io.BytesIO()
    image.save(stream, format=image_format or Path(path).suffix[1:].upper(), **options)
    write(path, stream.getvalue())


def font(size, bold=False):
    name = "FreeSansBold.ttf" if bold else "FreeSans.ttf"
    return ImageFont.truetype(str(ROOT / "media/fonts" / name), size)


def mark_source():
    source = Image.open(SOURCE).convert("RGB")
    pixels = []
    for r, g, b in source.getdata():
        floor = min(r, g, b)
        alpha = 255 if floor < 16 else max(0, 255 - floor)
        if alpha < 12 or b - r < 10:
            pixels.append((0, 0, 0, 0))
        else:
            pixels.append(tuple(min(255, max(0, (c - (255 - alpha)) * 255 // alpha))
                                for c in (r, g, b)) + (alpha,))
    result = Image.new("RGBA", source.size)
    result.putdata(pixels)
    return result.crop(result.getbbox())


def contain(image, width, height):
    return ImageOps.contain(image, (width, height), RESAMPLE)


def symbol(size, padding=0):
    image = Image.new("RGBA", size)
    scaled = contain(MARK, max(1, size[0] - padding * 2), max(1, size[1] - padding * 2))
    image.alpha_composite(scaled, ((size[0] - scaled.width) // 2, (size[1] - scaled.height) // 2))
    return image


def wordmark(size, color=(27, 45, 67, 255), vertical=False):
    scale = 4
    width, height = (value * scale for value in size)
    result = Image.new("RGBA", (width, height))
    if vertical:
        logo = contain(MARK, width * 3 // 5, height * 3 // 5)
        typeface = font(max(1, width // 7), True)
        draw = ImageDraw.Draw(result)
        box = draw.textbbox((0, 0), "ReliefWin", font=typeface)
        text_width, text_height = box[2], box[3] - box[1]
        top = (height - logo.height - text_height - height // 14) // 2
        result.alpha_composite(logo, ((width - logo.width) // 2, top))
        draw.text(((width - text_width) // 2, top + logo.height + height // 14 - box[1]),
                  "ReliefWin", font=typeface, fill=color)
    else:
        logo = contain(MARK, height * 4 // 5, height * 4 // 5)
        gap = max(1, height // 7)
        typeface = font(max(1, height * 3 // 5), True)
        draw = ImageDraw.Draw(result)
        box = draw.textbbox((0, 0), "ReliefWin", font=typeface)
        text_width, text_height = box[2], box[3] - box[1]
        total = logo.width + gap + text_width
        if total > width:
            factor = width / total
            typeface = font(max(1, int(height * 3 // 5 * factor)), True)
            logo = contain(logo, max(1, int(logo.width * factor)), max(1, int(logo.height * factor)))
            gap = max(1, int(gap * factor))
            box = draw.textbbox((0, 0), "ReliefWin", font=typeface)
            text_width, text_height = box[2], box[3] - box[1]
            total = logo.width + gap + text_width
        left = (width - total) // 2
        result.alpha_composite(logo, (left, (height - logo.height) // 2))
        draw.text((left + logo.width + gap, (height - text_height) // 2 - box[1]),
                  "ReliefWin", font=typeface, fill=color)
    return result.resize(size, RESAMPLE)


def flatten(image, background):
    result = Image.new("RGBA", image.size, background)
    result.alpha_composite(image)
    return result.convert("RGB")


def bitmap32(path, image):
    width, height = image.size
    pixels = image.transpose(TRANSPOSE.FLIP_TOP_BOTTOM).tobytes("raw", "BGRA")
    header = struct.pack("<IiiHHIIiiII", 108, width, height, 1, 32, 3, len(pixels), 2835, 2835, 0, 0)
    header += struct.pack("<IIIII", 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 0x73524742)
    header += bytes(48)
    write(path, struct.pack("<2sIHHI", b"BM", 14 + len(header) + len(pixels), 0, 0, 14 + len(header)) + header + pixels)


def bitmap4(path, image):
    palette_image = Image.new("P", (1, 1))
    palette_image.putpalette(VGA_PALETTE * 16)
    indexed = image.convert("RGB").quantize(palette=palette_image, dither=0)
    indexed = indexed.point([index % 16 for index in range(256)])
    palette = VGA_PALETTE
    if max(indexed.getdata()) >= 16:
        raise ValueError("The boot bitmap exceeds its 16-color palette")
    table = bytes(c for start in range(0, 48, 3)
                  for c in (palette[start + 2], palette[start + 1], palette[start], 0))
    width, height = indexed.size
    data = bytearray()
    for y in range(height - 1, -1, -1):
        row = [indexed.getpixel((x, y)) for x in range(width)]
        for start in range(0, width, 254):
            chunk = row[start:start + 254]
            if len(chunk) <= 2:
                data.extend((len(chunk), (chunk[0] << 4) | chunk[-1]))
            else:
                data.extend((0, len(chunk)))
                packed = bytes((chunk[x] << 4) | (chunk[x + 1] if x + 1 < len(chunk) else 0)
                               for x in range(0, len(chunk), 2))
                data.extend(packed)
                if len(packed) & 1:
                    data.append(0)
        data.extend((0, 0))
    data.extend((0, 1))
    header = struct.pack("<IiiHHIIiiII", 40, width, height, 1, 4, 2, len(data), 2835, 2835, 16, 16)
    write(path, struct.pack("<2sIHHI", b"BM", 118 + len(data), 0, 0, 118) + header + table + data)


def load_bitmap4(path):
    data = (ROOT / path).read_bytes()
    width, height = struct.unpack_from("<ii", data, 18)
    offset, = struct.unpack_from("<I", data, 10)
    header_size, = struct.unpack_from("<I", data, 14)
    table = data[14 + header_size:14 + header_size + 64]
    palette = [channel for index in range(16)
               for channel in (table[index * 4 + 2], table[index * 4 + 1], table[index * 4])]
    pixels = bytearray(width * height)
    x, y = 0, height - 1
    while True:
        length, code = data[offset:offset + 2]
        offset += 2
        if length:
            values = [(code >> 4) if i % 2 == 0 else (code & 15) for i in range(length)]
        elif code == 0:
            x, y = 0, y - 1
            continue
        elif code == 1:
            break
        elif code == 2:
            x += data[offset]
            y -= data[offset + 1]
            offset += 2
            continue
        else:
            length = code
            size = (length + 1) // 2
            values = [(data[offset + i // 2] >> 4) if i % 2 == 0 else
                      (data[offset + i // 2] & 15) for i in range(length)]
            offset += size + (size & 1)
        if y < 0 or y >= height or x + length > width:
            raise ValueError("The boot bitmap's RLE data exceeds its dimensions")
        pixels[y * width + x:y * width + x + length] = bytes(values)
        x += length
    result = Image.frombytes("P", (width, height), bytes(pixels))
    result.putpalette(palette)
    return result.convert("RGB")


def alpha_pair(path, image):
    save(path + ".bmp", image.convert("RGB"))
    save(path + "_mask.bmp", image.getchannel("A"))


def preserve_palette(original, image):
    colors = list(zip(*(iter(original.getpalette()),) * 3))
    old_colors = list(original.convert("RGB").getdata())
    new_colors = list(image.getdata())
    indices = list(original.getdata())
    closest = {}
    for index, (old, new) in enumerate(zip(old_colors, new_colors)):
        if old != new:
            if new not in closest:
                closest[new] = min(range(len(colors)),
                                   key=lambda i: sum((a - b) ** 2 for a, b in zip(colors[i], new)))
            indices[index] = closest[new]
    result = original.copy()
    result.putdata(indices)
    return result


def legacy_previews():
    folder = "base/shell/explorer/res/bmp/"
    for number in range(145, 154):
        if number == 145:
            continue
        path = folder + str(number) + ".bmp"
        image = Image.open(ROOT / path).convert("RGB")
        draw = ImageDraw.Draw(image)
        draw.rectangle((3, 13, 19, 27), fill=(212, 208, 200))
        logo = symbol((15, 13))
        image.paste(logo, (4, 14), logo)
        if number in (146, 147, 150, 151):
            left = {146: 127, 147: 146, 150: 80, 151: 89}[number]
            draw.rectangle((left, 14, left + 88, 26), fill=(212, 208, 200))
            draw.text((left, 14), "ReliefWin Explorer", font=font(11), fill="black")
        save(path, image)
    for number in (170, 171):
        path = folder + str(number) + ".bmp"
        palette = Image.open(ROOT / path)
        image = palette.convert("RGB")
        draw = ImageDraw.Draw(image)
        draw.rectangle((4, 171, 11, 177), fill=(212, 208, 200))
        logo = symbol((7, 7))
        image.paste(logo, (4, 171), logo)
        if number == 171:
            draw.rectangle((2, 69, 10, 169), fill=(138, 179, 219))
            panel = Image.new("RGBA", (100, 9))
            ImageDraw.Draw(panel).text((0, -1), "ReliefWin", font=font(9, True), fill="white")
            panel = panel.transpose(TRANSPOSE.ROTATE_90)
            image.paste(panel, (2, 69), panel)
        save(path, preserve_palette(palette, image))


def main():
    save("media/graphics/branding/reliefwin-mark.png", MARK)
    logo = wordmark((2000, 667))
    save("media/graphics/branding/reliefwin-logo.png", logo)
    save("media/graphics/branding/reliefwin-banner.png", flatten(logo, "white"))
    save("media/graphics/branding/reliefwin.ico", symbol((256, 256), 12), "ICO",
         sizes=[(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)], bitmap_format="bmp")
    image = symbol((48, 42))
    raw = bytes(c if channel == 3 else c * pixel[3] // 255
                for pixel in image.getdata() for channel, c in enumerate(pixel))
    write("bootloader/gui/assets/logo.rgba", raw)
    bitmap32("ntos/inbv/resources/reliefwin_logo.bmp", wordmark((652, 120), "white"))
    bitmap4("ntos/inbv/resources/logo.bmp", flatten(wordmark((190, 224), "white", True), "black"))
    copyright = Image.new("RGB", (260, 26), "black")
    draw = ImageDraw.Draw(copyright)
    draw.text((0, 0), "ReliefWin", font=font(10, True), fill="white")
    draw.text((0, 13), "© 2026 LiberNT; © 1996–2026 ReactOS contributors", font=font(10), fill="white")
    bitmap4("ntos/inbv/resources/copyright.bmp", copyright)
    bitmap4("ntos/inbv/resources/shutdown.bmp", load_bitmap4("ntos/inbv/resources/shutdown.bmp"))
    for path in ("base/system/userinit/res/rosbitmap", "dll/cpl/sysdm/resources/rosbitmap"):
        alpha_pair(path, flatten(wordmark((320, 159)), (238, 244, 250)).convert("RGBA"))
    for path in ("dll/win32/shell32/res/bitmaps/aboutbanner.bmp", "dll/win32/syssetup/resources/reactos.bmp"):
        save(path, flatten(wordmark((413, 72)), (238, 244, 250)))
    save("base/system/logonui/res/123.bmp", flatten(wordmark((137, 86), "white", True), (108, 148, 227)))
    save("base/applications/screensavers/logon/res/0.bmp", flatten(wordmark((205, 164), "white", True), "black"))
    watermark = Image.new("RGB", (164, 400), "black")
    scaled = symbol((96, 96))
    watermark.paste(scaled, (34, 75), scaled)
    save("base/setup/reliefwin/res/watermark.bmp", watermark)
    for path, sizes in {
        "base/setup/reliefwin/res/reliefwin.ico": [16, 24, 32, 48, 64, 128, 256],
        "image/data/icon.ico": [16, 24, 32, 48, 64, 128, 256],
        "base/shell/explorer/res/ico/258.ico": [16, 24, 32, 48, 64],
        "base/shell/progman/res/roslogo.ico": [16, 32],
        "base/applications/osk/res/reactos.ico": [16],
        "subsystems/win/win32k/user/user32/resources/oic_reactos.ico": [16, 32, 48],
    }.items():
        save(path, symbol((max(sizes), max(sizes)), max(1, max(sizes) // 16)), "ICO",
             sizes=[(value, value) for value in sizes], bitmap_format="bmp")
    for state in ("default", "hover", "pressed"):
        save("media/graphics/start-button/" + state + ".png", symbol((640, 640), 64))
    save("base/shell/explorer/res/bmp/143.bmp", flatten(symbol((25, 20)), "black").convert("RGBA"))
    panel = Image.new("RGBA", (233, 21), (138, 179, 219))
    panel.alpha_composite(wordmark((112, 21), "white"), (0, 0))
    save("base/shell/explorer/res/bmp/158.bmp", panel.transpose(TRANSPOSE.ROTATE_90).convert("RGB"))
    legacy_previews()
    from generate_brand_mesh import main as generate_mesh
    generate_mesh()
    mesh = "base/applications/dxdiag/d3dmark_mesh.h"
    OUTPUTS[mesh] = hashlib.sha256((ROOT / mesh).read_bytes()).hexdigest()
    manifest = {"source": "media/graphics/branding/reliefwin-source.jpg",
                "source_sha256": hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
                "outputs": OUTPUTS}
    (BRANDING / "assets.json").write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")


MARK = mark_source()
dark, light = MARK.getpixel((400, 300))[:3], MARK.getpixel((100, 100))[:3]
VGA_PALETTE = [channel for color in (
    (0, 0, 0), (24, 24, 24), dark, light,
    tuple(c // 2 for c in dark), tuple(c // 2 for c in light),
    *((gray, gray, gray) for gray in range(48, 241, 24)), (255, 255, 255))
    for channel in color]

if __name__ == "__main__":
    main()
