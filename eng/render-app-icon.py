"""Render the accepted SVG into the checked-in Windows icon asset."""

import argparse
import struct
import subprocess
import tempfile
from pathlib import Path

from PIL import Image


SIZES = (16, 20, 24, 32, 48, 64, 128, 256)


def dib_frame(image: Image.Image) -> bytes:
    image = image.convert("RGBA")
    width, height = image.size
    rgba = image.load()
    pixels = bytearray()
    for y in range(height - 1, -1, -1):
        for x in range(width):
            red, green, blue, alpha = rgba[x, y]
            pixels.extend((blue, green, red, alpha))
    mask_stride = ((width + 31) // 32) * 4
    mask = bytearray()
    for y in range(height - 1, -1, -1):
        row = bytearray(mask_stride)
        for x in range(width):
            if rgba[x, y][3] == 0:
                row[x // 8] |= 0x80 >> (x % 8)
        mask.extend(row)
    header = struct.pack(
        "<IiiHHIIiiII", 40, width, height * 2, 1, 32, 0, len(pixels), 0, 0, 0, 0
    )
    return header + pixels + mask


def render(chrome: Path, source: Path, directory: Path, size: int) -> Path:
    svg = source.read_text(encoding="utf-8")
    dimensions = 'width="256" height="256"'
    root_start = svg.find("<svg")
    root_end = svg.find(">", root_start)
    if root_start < 0 or root_end < 0 or svg[root_start:root_end].count(dimensions) != 1:
        raise ValueError("accepted SVG must have one 256 by 256 root dimension")
    root = svg[root_start:root_end].replace(dimensions, f'width="{size}" height="{size}"')
    svg = svg[:root_start] + root + svg[root_end:]
    sized_source = directory / f"app-icon-{size}.svg"
    image = directory / f"app-icon-{size}.png"
    sized_source.write_text(svg, encoding="utf-8")
    subprocess.run(
        [
            str(chrome),
            "--headless=new",
            "--disable-gpu",
            "--no-first-run",
            "--no-default-browser-check",
            f"--user-data-dir={directory / 'chrome-profile'}",
            "--default-background-color=00000000",
            "--force-device-scale-factor=1",
            f"--window-size={size},{size}",
            f"--screenshot={image}",
            sized_source.as_uri(),
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    return image


def make_icon(images: list[Path]) -> bytes:
    frames = []
    for size, path in zip(SIZES, images, strict=True):
        with Image.open(path) as image:
            if image.size != (size, size) or image.mode != "RGBA":
                raise ValueError(f"unexpected render: {path} {image.size} {image.mode}")
            minimum_alpha, maximum_alpha = image.getchannel("A").getextrema()
            if minimum_alpha >= 255 or maximum_alpha != 255:
                raise ValueError(f"render lacks translucent and opaque pixels: {path}")
            frames.append(path.read_bytes() if size == 256 else dib_frame(image))
    offset = 6 + 16 * len(frames)
    entries = []
    for size, frame in zip(SIZES, frames, strict=True):
        encoded_size = 0 if size == 256 else size
        entries.append(
            struct.pack("<BBBBHHII", encoded_size, encoded_size, 0, 0, 1, 32, len(frame), offset)
        )
        offset += len(frame)
    return struct.pack("<HHH", 0, 1, len(frames)) + b"".join(entries + frames)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--chrome", type=Path, required=True)
    parser.add_argument("--source", type=Path, default=Path("docs/design/app-icon.svg"))
    parser.add_argument("--output", type=Path, default=Path("src/app/NeNeLoupe.ico"))
    arguments = parser.parse_args()
    if not arguments.chrome.is_file() or not arguments.source.is_file():
        raise FileNotFoundError("Chrome or the accepted SVG was not found")
    with tempfile.TemporaryDirectory(prefix="neneloupe-icon-") as temporary:
        directory = Path(temporary).resolve()
        images = [render(arguments.chrome.resolve(), arguments.source.resolve(), directory, size) for size in SIZES]
        icon = make_icon(images)
    arguments.output.write_bytes(icon)
    print(f"Rendered {arguments.output} ({len(icon)} bytes, {len(SIZES)} sizes).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
