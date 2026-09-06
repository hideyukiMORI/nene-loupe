"""Photograph the running loupe for the README (Issue #21 / ADR 0007).

The product excludes its own windows from screen capture (ADR 0005), so no screen
capture route can see them; this script starts the product with the documented
diagnostic argument --allow-screen-capture, lays a controlled backdrop *over* the
loupe so the lens still samples real screen pixels instead of itself, and takes the
picture with PrintWindow. Every shot is only kept when the window's own caption
proves it sampled the colour this script painted under the lens.

Requires an unlocked interactive Windows session. Generates no pointer or keyboard
input; the settings modal is opened by posting ordinary window messages.
"""

import argparse
import colorsys
import ctypes as c
from ctypes import wintypes as w
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time

from PIL import Image

user = c.WinDLL("user32", use_last_error=True)
gdi = c.WinDLL("gdi32", use_last_error=True)
shcore = c.WinDLL("shcore", use_last_error=True)


def api(dll, name, result, *arguments):
    function = getattr(dll, name)
    function.restype, function.argtypes = result, arguments
    return function


api(user, "SetProcessDpiAwarenessContext", w.BOOL, w.HANDLE)
api(user, "CreateWindowExW", w.HWND, w.DWORD, w.LPCWSTR, w.LPCWSTR, w.DWORD,
    c.c_int, c.c_int, c.c_int, c.c_int, w.HWND, w.HMENU, w.HINSTANCE, w.LPVOID)
api(user, "DestroyWindow", w.BOOL, w.HWND)
api(user, "ShowWindow", w.BOOL, w.HWND, c.c_int)
api(user, "IsWindowVisible", w.BOOL, w.HWND)
api(user, "EnumWindows", w.BOOL, c.c_void_p, w.LPARAM)
api(user, "EnumDisplayMonitors", w.BOOL, w.HDC, c.c_void_p, c.c_void_p, w.LPARAM)
api(user, "GetWindowThreadProcessId", w.DWORD, w.HWND, c.POINTER(w.DWORD))
api(user, "GetClassNameW", c.c_int, w.HWND, w.LPWSTR, c.c_int)
api(user, "GetWindowTextW", c.c_int, w.HWND, w.LPWSTR, c.c_int)
api(user, "GetWindowRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "GetWindowDisplayAffinity", w.BOOL, w.HWND, c.POINTER(w.DWORD))
api(user, "GetWindow", w.HWND, w.HWND, w.UINT)
api(user, "SetWindowLongPtrW", c.c_ssize_t, w.HWND, c.c_int, c.c_ssize_t)
api(user, "DefWindowProcW", c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
api(user, "ValidateRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "PeekMessageW", w.BOOL, c.POINTER(w.MSG), w.HWND, w.UINT, w.UINT, w.UINT)
api(user, "TranslateMessage", w.BOOL, c.POINTER(w.MSG))
api(user, "DispatchMessageW", c.c_ssize_t, c.POINTER(w.MSG))
api(user, "PostMessageW", w.BOOL, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
api(user, "SetWindowPos", w.BOOL, w.HWND, w.HWND, c.c_int, c.c_int, c.c_int, c.c_int, w.UINT)
api(user, "GetDC", w.HDC, w.HWND)
api(user, "ReleaseDC", c.c_int, w.HWND, w.HDC)
api(user, "FillRect", c.c_int, w.HDC, c.POINTER(w.RECT), w.HBRUSH)
api(user, "PrintWindow", w.BOOL, w.HWND, w.HDC, w.UINT)
api(user, "GetDpiForWindow", w.UINT, w.HWND)
api(gdi, "CreateSolidBrush", w.HBRUSH, w.DWORD)
api(gdi, "DeleteObject", w.BOOL, w.HGDIOBJ)
api(gdi, "GdiFlush", w.BOOL)
api(gdi, "SetPixel", w.DWORD, w.HDC, c.c_int, c.c_int, w.DWORD)
api(gdi, "GetPixel", w.DWORD, w.HDC, c.c_int, c.c_int)
api(gdi, "CreateCompatibleDC", w.HDC, w.HDC)
api(gdi, "DeleteDC", w.BOOL, w.HDC)
api(gdi, "SelectObject", w.HGDIOBJ, w.HDC, w.HGDIOBJ)

enum_window_callback = c.WINFUNCTYPE(w.BOOL, w.HWND, w.LPARAM)
window_callback = c.WINFUNCTYPE(c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
monitor_callback = c.WINFUNCTYPE(w.BOOL, w.HANDLE, w.HDC, c.POINTER(w.RECT), w.LPARAM)

PRINT_FULL_CONTENT = 2
LENS_SIDE = 7          # ADR 0004 / 0005: the loupe magnifies 7x7 screen pixels
LENS_CENTRE_DIP = 32   # LoupeLayout::lens_center
GEAR_DIP = (224, 15)   # inside LoupeLayout::hit_gear


class BITMAPINFOHEADER(c.Structure):
    _fields_ = [("biSize", w.DWORD), ("biWidth", c.c_long), ("biHeight", c.c_long),
                ("biPlanes", w.WORD), ("biBitCount", w.WORD), ("biCompression", w.DWORD),
                ("biSizeImage", w.DWORD), ("biXPelsPerMeter", c.c_long),
                ("biYPelsPerMeter", c.c_long), ("biClrUsed", w.DWORD),
                ("biClrImportant", w.DWORD)]


class BITMAPINFO(c.Structure):
    _fields_ = [("bmiHeader", BITMAPINFOHEADER), ("bmiColors", w.DWORD * 3)]


class MONITORINFO(c.Structure):
    _fields_ = [("cbSize", w.DWORD), ("rcMonitor", w.RECT), ("rcWork", w.RECT),
                ("dwFlags", w.DWORD)]


api(gdi, "CreateDIBSection", w.HBITMAP, w.HDC, c.POINTER(BITMAPINFO), w.UINT,
    c.POINTER(c.c_void_p), w.HANDLE, w.DWORD)
api(user, "GetMonitorInfoW", w.BOOL, w.HANDLE, c.POINTER(MONITORINFO))
api(shcore, "GetDpiForMonitor", c.c_long, w.HANDLE, c.c_int, c.POINTER(w.UINT),
    c.POINTER(w.UINT))


@window_callback
def backdrop_procedure(window, message, word, data):
    # Keep the painted pixels: never erase, never repaint.
    if message == 0x000F:  # WM_PAINT
        user.ValidateRect(window, None)
        return 0
    if message == 0x0014:  # WM_ERASEBKGND
        return 1
    return user.DefWindowProcW(window, message, word, data)


def pause(seconds):
    deadline = time.monotonic() + seconds
    message = w.MSG()
    while time.monotonic() < deadline:
        while user.PeekMessageW(c.byref(message), None, 0, 0, 1):
            user.TranslateMessage(c.byref(message))
            user.DispatchMessageW(c.byref(message))
        time.sleep(0.005)


def bounds(window):
    rect = w.RECT()
    assert user.GetWindowRect(window, c.byref(rect))
    return rect


def caption(window):
    buffer = c.create_unicode_buffer(160)
    user.GetWindowTextW(window, buffer, len(buffer))
    return buffer.value


def scaled(value, dpi):
    return round(value * dpi / 96)


def point_parameter(x, y):
    return (x & 0xFFFF) | ((y & 0xFFFF) << 16)


def monitors():
    found = []

    @monitor_callback
    def collect(handle, dc, rectangle, data):
        horizontal, vertical = w.UINT(), w.UINT()
        shcore.GetDpiForMonitor(handle, 0, c.byref(horizontal), c.byref(vertical))
        info = MONITORINFO()
        info.cbSize = c.sizeof(MONITORINFO)
        assert user.GetMonitorInfoW(handle, c.byref(info))
        found.append({"dpi": horizontal.value,
                      "work": [info.rcWork.left, info.rcWork.top,
                               info.rcWork.right, info.rcWork.bottom],
                      "primary": bool(info.dwFlags & 1)})
        return True

    assert user.EnumDisplayMonitors(None, None, collect, 0)
    return found


def find_owned_window(process_id, class_name):
    found = []

    @enum_window_callback
    def collect(window, data):
        owner = w.DWORD()
        user.GetWindowThreadProcessId(window, c.byref(owner))
        name = c.create_unicode_buffer(128)
        user.GetClassNameW(window, name, len(name))
        if owner.value == process_id and name.value == class_name:
            found.append(window)
        return True

    assert user.EnumWindows(collect, 0)
    return found[0] if found else None


def start(executable, environment):
    process = subprocess.Popen([str(executable), "--allow-screen-capture"], env=environment)
    for _ in range(200):
        window = find_owned_window(process.pid, "NeNeLoupe.Window")
        if window and user.IsWindowVisible(window):
            return process, window
        if process.poll() is not None:
            raise AssertionError(f"Loupe exited early: {process.returncode}")
        pause(0.03)
    process.terminate()
    process.wait()
    raise AssertionError("Loupe did not create its window")


def stop(process, window):
    if process.poll() is None:
        user.PostMessageW(window, 0x0010, 0, 0)  # WM_CLOSE
        process.wait(timeout=5)


def await_stable_geometry(window):
    previous, stable = None, 0
    for _ in range(200):
        rect = bounds(window)
        current = (user.GetDpiForWindow(window),
                   (rect.left, rect.top, rect.right, rect.bottom))
        if current == previous:
            stable += 1
            if stable == 3:
                return current
        else:
            stable, previous = 0, current
        pause(0.03)
    raise AssertionError("Window geometry did not settle")


def lens_pixels():
    """A 7x7 patch of distinct colours: hue across the columns, lightness down the rows."""
    grid = []
    for row in range(LENS_SIDE):
        line = []
        for column in range(LENS_SIDE):
            hue = column / LENS_SIDE
            lightness = 0.34 + 0.07 * row
            red, green, blue = colorsys.hls_to_rgb(hue, lightness, 0.78)
            line.append((round(red * 255), round(green * 255), round(blue * 255)))
        grid.append(line)
    return grid


def paint_backdrop(window, width, height, centre, grid):
    """Fill the cover, then place the loupe's 7x7 sample exactly under the lens."""
    dc = user.GetDC(window)
    try:
        brush = gdi.CreateSolidBrush(0x1A1512)
        rectangle = w.RECT(0, 0, width, height)
        assert user.FillRect(dc, c.byref(rectangle), brush)
        gdi.DeleteObject(brush)
        half = LENS_SIDE // 2
        for row in range(LENS_SIDE):
            for column in range(LENS_SIDE):
                red, green, blue = grid[row][column]
                gdi.SetPixel(dc, centre[0] - half + column, centre[1] - half + row,
                             red | (green << 8) | (blue << 16))
        assert gdi.GdiFlush()
    finally:
        user.ReleaseDC(window, dc)


def cover(loupe_rect, margin):
    window = user.CreateWindowExW(
        8, "STATIC", "NeNe Loupe capture backdrop", 0x80000000,
        loupe_rect.left - margin, loupe_rect.top - margin,
        loupe_rect.right - loupe_rect.left + 2 * margin,
        loupe_rect.bottom - loupe_rect.top + 2 * margin, None, None, None, None)
    assert window
    user.SetWindowLongPtrW(window, -4, c.cast(backdrop_procedure, c.c_void_p).value)
    user.ShowWindow(window, 4)
    # HWND_TOPMOST last wins, so this lands above the loupe even when the loupe is topmost.
    assert user.SetWindowPos(window, c.c_void_p(-1), loupe_rect.left - margin,
                             loupe_rect.top - margin, 0, 0, 0x11)
    return window


def await_caption(window, expected):
    for _ in range(200):
        if caption(window).endswith(expected):
            return caption(window)
        pause(0.03)
    raise AssertionError(f"Expected caption ending {expected}; actual {caption(window)}")


def shoot(window):
    rect = bounds(window)
    width, height = rect.right - rect.left, rect.bottom - rect.top
    screen = user.GetDC(None)
    memory = gdi.CreateCompatibleDC(screen)
    info = BITMAPINFO()
    info.bmiHeader.biSize = c.sizeof(BITMAPINFOHEADER)
    info.bmiHeader.biWidth = width
    info.bmiHeader.biHeight = -height
    info.bmiHeader.biPlanes = 1
    info.bmiHeader.biBitCount = 32
    bits = c.c_void_p()
    bitmap = gdi.CreateDIBSection(screen, c.byref(info), 0, c.byref(bits), None, 0)
    user.ReleaseDC(None, screen)
    assert bitmap and bits
    previous = gdi.SelectObject(memory, bitmap)
    try:
        assert user.PrintWindow(window, memory, PRINT_FULL_CONTENT), "PrintWindow refused"
        gdi.GdiFlush()
        raw = bytes((c.c_char * (width * height * 4)).from_address(bits.value))
        image = Image.frombuffer("RGBA", (width, height), raw, "raw", "BGRA", 0, 1).convert("RGB")
        assert image.getextrema()[0] != (0, 0) or image.convert("L").getextrema() != (0, 0), \
            "PrintWindow returned an empty image"
        return image, [rect.left, rect.top, rect.right, rect.bottom]
    finally:
        gdi.SelectObject(memory, previous)
        gdi.DeleteObject(bitmap)
        gdi.DeleteDC(memory)


def save(image, path):
    """Keep it lossless: index the palette only when the picture really has few colours."""
    colours = image.getcolors(256)
    if colours:
        indexed = image.convert("P", palette=Image.ADAPTIVE, colors=len(colours))
        if list(indexed.convert("RGB").getdata()) == list(image.getdata()):
            image = indexed
    image.save(path, optimize=True)
    return path.stat().st_size


def place(window, monitor, dpi):
    left, top, right, bottom = monitor["work"]
    x = left + (right - left) // 3
    y = top + (bottom - top) // 3
    assert user.SetWindowPos(window, c.c_void_p(-1), x, y, 0, 0, 0x11)
    settled_dpi, rectangle = await_stable_geometry(window)
    assert settled_dpi == dpi, (settled_dpi, dpi)
    return rectangle


def capture(executable, output, monitor, shot, results):
    settings_root = Path(tempfile.mkdtemp(prefix="capture-", dir=output)).resolve()
    settings_file = settings_root / "NeNeLoupe/settings.v1.txt"
    settings_file.parent.mkdir(parents=True, exist_ok=True)
    settings_file.write_bytes(shot["settings"].encode("utf-8"))
    environment = dict(os.environ, LOCALAPPDATA=str(settings_root))
    process, window = start(executable, environment)
    backdrop = None
    try:
        place(window, monitor, shot["dpi"])
        dpi = user.GetDpiForWindow(window)
        rect = bounds(window)
        backdrop = cover(rect, scaled(40, dpi))
        centre_screen = (rect.left + scaled(LENS_CENTRE_DIP, dpi),
                         rect.top + scaled(LENS_CENTRE_DIP, dpi))
        backdrop_rect = bounds(backdrop)
        grid = lens_pixels()
        paint_backdrop(backdrop, backdrop_rect.right - backdrop_rect.left,
                       backdrop_rect.bottom - backdrop_rect.top,
                       (centre_screen[0] - backdrop_rect.left,
                        centre_screen[1] - backdrop_rect.top), grid)
        middle = grid[LENS_SIDE // 2][LENS_SIDE // 2]
        expected = "#{:02X}{:02X}{:02X}".format(*middle)
        # The window's own caption is the proof that the lens sampled the painted patch.
        title = await_caption(window, expected)
        affinity = w.DWORD()
        assert user.GetWindowDisplayAffinity(window, c.byref(affinity))

        target = window
        if shot.get("settings_modal"):
            user.PostMessageW(window, 0x0201, 1,
                              point_parameter(scaled(GEAR_DIP[0], dpi), scaled(GEAR_DIP[1], dpi)))
            user.PostMessageW(window, 0x0202, 0,
                              point_parameter(scaled(GEAR_DIP[0], dpi), scaled(GEAR_DIP[1], dpi)))
            for _ in range(200):
                modal = find_owned_window(process.pid, "NeNeLoupe.Settings")
                if modal and user.IsWindowVisible(modal):
                    target = modal
                    break
                pause(0.03)
            assert target is not window, "Settings did not open"
            pause(0.4)

        image, rectangle = shoot(target)
        path = Path(shot["path"])
        path.parent.mkdir(parents=True, exist_ok=True)
        size = save(image, path)
        results.append({"name": shot["name"], "file": str(path).replace("\\", "/"),
                        "dpi": dpi, "pixels": list(image.size),
                        "dips": [round(image.size[0] * 96 / dpi), round(image.size[1] * 96 / dpi)],
                        "bytes": size, "windowRect": rectangle, "caption": title,
                        "sampledColour": expected, "displayAffinity": hex(affinity.value),
                        "settings": shot["settings"].splitlines()})
        return image
    finally:
        if backdrop:
            user.DestroyWindow(backdrop)
        stop(process, window)


def social_preview(shot_image, path):
    """The one composed picture: a real capture, doubled, on a plain card for GitHub."""
    from PIL import ImageDraw, ImageFont
    card = Image.new("RGB", (1280, 640), (18, 23, 29))
    doubled = shot_image.resize((shot_image.width * 2, shot_image.height * 2), Image.NEAREST)
    card.paste(doubled, ((1280 - doubled.width) // 2, 300))
    draw = ImageDraw.Draw(card)
    fonts = Path(os.environ["WINDIR"]) / "Fonts"
    title = ImageFont.truetype(str(fonts / "segoeuib.ttf"), 76)
    body = ImageFont.truetype(str(fonts / "segoeui.ttf"), 30)
    draw.text((640, 176), "NeNe Loupe", font=title, fill=(240, 244, 248), anchor="mm")
    draw.text((640, 236), "A tiny frameless screen loupe and colour picker for Windows",
              font=body, fill=(150, 162, 175), anchor="mm")
    draw.text((640, 520), "C++23  ·  plain Win32  ·  no UI library", font=body,
              fill=(110, 122, 136), anchor="mm")
    path.parent.mkdir(parents=True, exist_ok=True)
    card.save(path, optimize=True)
    return {"name": "socialPreview", "file": str(path).replace("\\", "/"),
            "pixels": list(card.size), "bytes": path.stat().st_size,
            "composed": "real capture of the dark shot, scaled 2x, on a plain card"}


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, default=root / "build/NeNeLoupe.exe")
    parser.add_argument("--images", type=Path, default=root / "docs/images")
    arguments = parser.parse_args()
    output = root / "out/readme-capture"
    output.mkdir(parents=True, exist_ok=True)
    executable = output / "NeNeLoupe.exe"
    shutil.copy2(arguments.executable, executable)

    assert user.SetProcessDpiAwarenessContext(c.c_void_p(-4))
    available = monitors()
    primary = next(monitor for monitor in available if monitor["primary"])
    tall = max(available, key=lambda monitor: monitor["dpi"] if monitor["dpi"] == 144 else 0)
    assert tall["dpi"] == 144, "no 150% monitor to photograph"

    shots = [
        {"name": "dark", "path": arguments.images / "loupe-dark.png", "dpi": primary["dpi"],
         "monitor": primary, "settings": "schema=1\ntheme=dark\nformat=hex\nlayer=topmost\n"},
        {"name": "light", "path": arguments.images / "loupe-light.png", "dpi": primary["dpi"],
         "monitor": primary, "settings": "schema=1\ntheme=light\nformat=hex\nlayer=topmost\n"},
        {"name": "settings", "path": arguments.images / "loupe-settings.png",
         "dpi": primary["dpi"], "monitor": primary, "settings_modal": True,
         "settings": "schema=1\ntheme=dark\nformat=hex\nlayer=topmost\n"},
        {"name": "dark150", "path": arguments.images / "loupe-dark-150.png", "dpi": tall["dpi"],
         "monitor": tall, "settings": "schema=1\ntheme=dark\nformat=hex\nlayer=topmost\n"},
    ]
    results = []
    images = {}
    for shot in shots:
        images[shot["name"]] = capture(executable, output, shot["monitor"], shot, results)
    results.append(social_preview(images["dark"], arguments.images / "social-preview.png"))
    (output / "capture-results.json").write_text(
        json.dumps({"shots": results, "monitors": available}, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(results, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
