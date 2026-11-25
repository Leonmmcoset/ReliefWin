import argparse
import io
from pathlib import Path
from PIL import Image

PALETTES = {
    "NORMAL": {0: (28, 32, 40), 1: (39, 43, 51)},
    "LIGHT": {0: (238, 238, 238), 1: (243, 243, 243)},
}


def frame(w, h, states, palette):
    im = Image.new("RGB", (w, h * states))
    for s in range(states):
        im.paste(palette[s], (0, s * h, w, (s + 1) * h))
    return im


def main():
    parser = argparse.ArgumentParser(description="Generate uniform caption and resize-frame surfaces without outlines")
    parser.add_argument("output", nargs="?", type=Path,
                        default=Path(__file__).resolve().parent.parent / "bitmaps")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    failures = []
    for prefix, palette in PALETTES.items():
        def save(im, name):
            path = args.output / (prefix + "_" + name + ".bmp")
            buffer = io.BytesIO()
            im.save(buffer, format="BMP")
            data = buffer.getvalue()
            if path.is_file() and path.read_bytes() == data:
                return
            if args.check:
                failures.append(str(path))
            else:
                path.write_bytes(data)

        save(frame(8, 30, 2, palette), "FRAMECAPTION")
        save(frame(8, 22, 2, palette), "SMALLFRAMECAPTION")
        save(frame(8, 30, 2, palette), "FRAMECAPTIONMIN")
        save(frame(6, 8, 2, palette), "FRAMELEFT")
        save(frame(6, 8, 2, palette), "FRAMERIGHT")
        save(frame(8, 6, 2, palette), "FRAMEBOTTOM")
        save(frame(40, 20, 2, palette), "FRAMECAPTIONSIZING")
        save(frame(40, 20, 2, palette), "SMALLFRAMECAPTIONSIZING")
        save(frame(8, 30, 2, palette), "FRAMEMAXIMIZED")
    if failures:
        parser.exit(1, "Frame bitmap differs or is missing:\n" + "\n".join(failures) + "\n")
    print("18 frame bitmaps " + ("verified" if args.check else "generated"))


if __name__ == "__main__":
    main()
