#!/usr/bin/env python3
from pathlib import Path
import io
import cairosvg
from PIL import Image

root = Path(__file__).resolve().parent
for source in sorted(root.glob('*.svg')):
    png = cairosvg.svg2png(bytestring=source.read_bytes())
    image = Image.open(io.BytesIO(png))
    image.save(source.with_suffix('.png'))
    image.save(source.with_suffix('.ico'), sizes=[image.size])
