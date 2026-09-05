"""Interactive Windows verification, separate from unit tests (Issue #6 / QLT-013).

Owns and closes its test windows without moving the pointer or generating keyboard input.
Requires an unlocked interactive Windows session; writes numeric evidence only.
"""

import ctypes as c
from ctypes import wintypes as w
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time

user = c.WinDLL("user32", use_last_error=True)
gdi = c.WinDLL("gdi32", use_last_error=True)
kernel = c.WinDLL("kernel32", use_last_error=True)


def api(dll, name, result, *arguments):
    function = getattr(dll, name)
    function.restype, function.argtypes = result, arguments
    return function


api(user, "SetProcessDpiAwarenessContext", w.BOOL, w.HANDLE)
api(user, "GetWindowDisplayAffinity", w.BOOL, w.HWND, c.POINTER(w.DWORD))
api(user, "CreateWindowExW", w.HWND, w.DWORD, w.LPCWSTR, w.LPCWSTR, w.DWORD,
    c.c_int, c.c_int, c.c_int, c.c_int, w.HWND, w.HMENU, w.HINSTANCE, w.LPVOID)
api(user, "DestroyWindow", w.BOOL, w.HWND)
api(user, "ShowWindow", w.BOOL, w.HWND, c.c_int)
api(user, "IsWindowVisible", w.BOOL, w.HWND)
api(user, "FindWindowW", w.HWND, w.LPCWSTR, w.LPCWSTR)
api(user, "GetWindowThreadProcessId", w.DWORD, w.HWND, c.POINTER(w.DWORD))
api(user, "GetWindowRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "GetClientRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "GetWindowTextW", c.c_int, w.HWND, w.LPWSTR, c.c_int)
api(user, "GetClassNameW", c.c_int, w.HWND, w.LPWSTR, c.c_int)
api(user, "GetWindow", w.HWND, w.HWND, w.UINT)
api(user, "GetWindowLongPtrW", c.c_ssize_t, w.HWND, c.c_int)
api(user, "SetWindowLongPtrW", c.c_ssize_t, w.HWND, c.c_int, c.c_ssize_t)
api(user, "DefWindowProcW", c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
api(user, "ValidateRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "PeekMessageW", w.BOOL, c.POINTER(w.MSG), w.HWND, w.UINT, w.UINT, w.UINT)
api(user, "TranslateMessage", w.BOOL, c.POINTER(w.MSG))
api(user, "DispatchMessageW", c.c_ssize_t, c.POINTER(w.MSG))
api(user, "GetDpiForWindow", w.UINT, w.HWND)
api(user, "SetWindowPos", w.BOOL, w.HWND, w.HWND, c.c_int, c.c_int, c.c_int, c.c_int, w.UINT)
api(user, "SendMessageW", c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
api(user, "PostMessageW", w.BOOL, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
api(user, "GetDC", w.HDC, w.HWND)
api(user, "ReleaseDC", c.c_int, w.HWND, w.HDC)
api(user, "FillRect", c.c_int, w.HDC, c.POINTER(w.RECT), w.HBRUSH)
api(user, "GetSystemMetrics", c.c_int, c.c_int)
api(user, "GetGuiResources", w.DWORD, w.HANDLE, w.DWORD)
api(user, "OpenClipboard", w.BOOL, w.HWND)
api(user, "CloseClipboard", w.BOOL)
api(user, "EmptyClipboard", w.BOOL)
api(user, "EnumClipboardFormats", w.UINT, w.UINT)
api(user, "GetClipboardData", w.HANDLE, w.UINT)
api(user, "SetClipboardData", w.HANDLE, w.UINT, w.HANDLE)
api(user, "GetClipboardOwner", w.HWND)
api(user, "IsWindowEnabled", w.BOOL, w.HWND)
api(user, "IsZoomed", w.BOOL, w.HWND)
api(gdi, "CreateSolidBrush", w.HBRUSH, w.DWORD)
api(gdi, "DeleteObject", w.BOOL, w.HGDIOBJ)
api(gdi, "GdiFlush", w.BOOL)
api(gdi, "GetPixel", w.DWORD, w.HDC, c.c_int, c.c_int)
api(gdi, "SetPixel", w.DWORD, w.HDC, c.c_int, c.c_int, w.DWORD)
api(kernel, "OpenProcess", w.HANDLE, w.DWORD, w.BOOL, w.DWORD)
api(kernel, "CloseHandle", w.BOOL, w.HANDLE)
api(kernel, "GlobalAlloc", w.HGLOBAL, w.UINT, c.c_size_t)
api(kernel, "GlobalLock", w.LPVOID, w.HGLOBAL)
api(kernel, "GlobalUnlock", w.BOOL, w.HGLOBAL)
api(kernel, "GlobalSize", c.c_size_t, w.HGLOBAL)
api(kernel, "GlobalFree", w.HGLOBAL, w.HGLOBAL)
monitor_callback = c.WINFUNCTYPE(w.BOOL, w.HANDLE, w.HDC, c.POINTER(w.RECT), w.LPARAM)
api(user, "EnumDisplayMonitors", w.BOOL, w.HDC, c.POINTER(w.RECT), monitor_callback, w.LPARAM)
enum_window_callback = c.WINFUNCTYPE(w.BOOL, w.HWND, w.LPARAM)
api(user, "EnumWindows", w.BOOL, enum_window_callback, w.LPARAM)



def pause(seconds):
    deadline = time.monotonic() + seconds
    message = w.MSG()
    while time.monotonic() < deadline:
        while user.PeekMessageW(c.byref(message), None, 0, 0, 1):
            user.TranslateMessage(c.byref(message))
            user.DispatchMessageW(c.byref(message))
        time.sleep(0.005)


window_callback = c.WINFUNCTYPE(c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)


@window_callback
def fixture_procedure(window, message, word, data):
    if message == 0x000F:
        user.ValidateRect(window, None)
        return 0
    if message == 0x0014:
        return 1
    return user.DefWindowProcW(window, message, word, data)

def bounds(window):
    rect = w.RECT()
    assert user.GetWindowRect(window, c.byref(rect))
    return [rect.left, rect.top, rect.right, rect.bottom]


def caption(window):
    buffer = c.create_unicode_buffer(128)
    user.GetWindowTextW(window, buffer, len(buffer))
    return buffer.value


def open_clipboard(owner):
    for _ in range(50):
        if user.OpenClipboard(owner):
            return
        pause(0.02)
    raise AssertionError("Clipboard stayed locked")


def clipboard_text():
    open_clipboard(None)
    try:
        handle = user.GetClipboardData(13)
        assert handle, "No CF_UNICODETEXT after clicking the value"
        pointer = kernel.GlobalLock(handle)
        assert pointer
        try:
            return c.wstring_at(pointer)
        finally:
            kernel.GlobalUnlock(handle)
    finally:
        user.CloseClipboard()


class ClipboardSnapshot:
    """Preserve supported clipboard formats in memory; never write them to evidence."""

    def __init__(self, owner):
        self.owner = owner
        self.formats = []
        open_clipboard(owner)
        try:
            current = user.EnumClipboardFormats(0)
            while current:
                # GDI/private handles need distinct ownership rules. Refuse before changing anything.
                assert current not in (2, 3, 9, 14) and not 0x200 <= current <= 0x3FF, (
                    "Clipboard has a non-memory format; run this verification with a text clipboard")
                handle = user.GetClipboardData(current)
                size = kernel.GlobalSize(handle)
                assert size, "Clipboard format cannot be safely preserved"
                pointer = kernel.GlobalLock(handle)
                assert pointer
                try:
                    self.formats.append((current, c.string_at(pointer, size)))
                finally:
                    kernel.GlobalUnlock(handle)
                current = user.EnumClipboardFormats(current)
        finally:
            user.CloseClipboard()

    def restore(self, test_window):
        # A copy in another app during verification belongs to the user and must win.
        if user.GetClipboardOwner() != test_window:
            return
        open_clipboard(self.owner)
        try:
            assert user.EmptyClipboard()
            for kind, data in self.formats:
                handle = kernel.GlobalAlloc(0x2, len(data))
                assert handle
                transferred = False
                try:
                    pointer = kernel.GlobalLock(handle)
                    assert pointer
                    c.memmove(pointer, data, len(data))
                    kernel.GlobalUnlock(handle)
                    transferred = bool(user.SetClipboardData(kind, handle))
                    assert transferred
                finally:
                    if not transferred:
                        kernel.GlobalFree(handle)
        finally:
            user.CloseClipboard()


def point_parameter(x, y):
    return (x & 0xFFFF) | ((y & 0xFFFF) << 16)


def click(window, x, y):
    dpi = user.GetDpiForWindow(window)
    point = point_parameter(round(x * dpi / 96), round(y * dpi / 96))
    assert user.PostMessageW(window, 0x0201, 1, point)
    assert user.PostMessageW(window, 0x0202, 0, point)
    pause(0.15)


def verify_hit_regions(window):
    left, top, _, _ = bounds(window)
    dpi = user.GetDpiForWindow(window)
    for x, y, expected in ((32, 32, 2), (100, 16, 1), (224, 16, 1), (150, 44, 1)):
        point = point_parameter(left + round(x * dpi / 96), top + round(y * dpi / 96))
        assert user.SendMessageW(window, 0x0084, 0, point) == expected, (x, y)


def verify_double_click(window):
    original = bounds(window)
    dpi = user.GetDpiForWindow(window)
    point = point_parameter(original[0] + round(32 * dpi / 96),
                            original[1] + round(32 * dpi / 96))
    assert user.PostMessageW(window, 0x00A3, 2, point)
    pause(0.1)
    assert not user.IsZoomed(window)
    assert bounds(window) == original, "Caption double click changed loupe geometry"


def verify_copy_formats(window, helper):
    paint_color(helper, 0x0080FF)
    await_caption(window, "#FF8000")
    snapshot = ClipboardSnapshot(helper)
    copied = []
    try:
        # HEX -> CMYK -> HSL -> HSV -> RGB -> HEX follows the declared closed cycle.
        for expected in ("#FF8000", "0, 50, 100, 0", "30, 100%, 50%", "30, 100%, 100%",
                         "255, 128, 0", "#FF8000"):
            await_caption(window, expected)
            click(window, 150, 44)
            actual = clipboard_text()
            assert actual == expected, (expected, actual)
            copied.append(actual)
            if len(copied) < 6:
                click(window, 112, 16)
        return copied
    finally:
        snapshot.restore(window)


def await_caption(window, expected):
    for _ in range(100):
        if caption(window).endswith(expected):
            return
        pause(0.03)
    raise AssertionError(f"Expected {expected}; actual {caption(window)}")


def paint_color(window, color):
    dc = user.GetDC(window)
    brush = gdi.CreateSolidBrush(color)
    try:
        rectangle = w.RECT(0, 0, 180, 160)
        assert user.FillRect(dc, c.byref(rectangle), brush)
        assert gdi.GdiFlush()
    finally:
        gdi.DeleteObject(brush)
        user.ReleaseDC(window, dc)


def find_owned_window(process_id, class_name="NeNeLoupe.Window"):
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


def find_modal(process_id, parent):
    found = []
    @enum_window_callback
    def collect(window, data):
        owner = w.DWORD()
        user.GetWindowThreadProcessId(window, c.byref(owner))
        if (owner.value == process_id and user.GetWindow(window, 4) == parent
                and user.IsWindowVisible(window)):
            found.append(window)
        return True
    assert user.EnumWindows(collect, 0)
    return found[0] if found else None


def await_modal(process_id, parent):
    for _ in range(100):
        window = find_modal(process_id, parent)
        if window:
            return window
        pause(0.03)
    raise AssertionError("Settings did not open")


def verify_settings(window, process):
    click(window, 224, 16)
    modal = await_modal(process.pid, window)
    assert not user.IsWindowEnabled(window), "Settings must disable interaction with its owner"
    affinity = w.DWORD()
    assert user.GetWindowDisplayAffinity(modal, c.byref(affinity)) and affinity.value == 0x11
    layers = []
    for expected in (False, True, False):
        click(modal, 120, 222)
        for item in (window, modal):
            assert bool(user.GetWindowLongPtrW(item, -20) & 8) == expected
        layers.append(expected)
    # Select all three themes through their ordinary hit regions; persistence is checked after restart.
    for y in (94, 126, 158, 94):
        click(modal, 100, y)
    assert user.PostMessageW(modal, 0x0100, 0x1B, 0)
    for _ in range(100):
        if find_modal(process.pid, window) is None and user.IsWindowEnabled(window):
            break
        pause(0.03)
    assert find_modal(process.pid, window) is None
    assert user.IsWindowEnabled(window), "Closing the modal must restore owner interaction"
    return {"captureExclusion": affinity.value, "topmostTransitions": layers,
            "ownerReenabled": True, "themesClicked": ["dark", "light", "system", "dark"]}


def verify_rejected_settings(executable, environment, settings_file, helper):
    malformed = {
        "unknownVersion": "schema=2\ntheme=dark\nformat=hex\nlayer=normal\n",
        "unknownTheme": "schema=1\ntheme=bogus\nformat=hex\nlayer=normal\n",
        "unknownFormat": "schema=1\ntheme=dark\nformat=bogus\nlayer=normal\n",
        "missingField": "schema=1\nformat=hex\nlayer=normal\n",
        "trailingData": "schema=1\ntheme=dark\nformat=hex\nlayer=normal\nextra=true\n",
    }
    results = []
    for name, payload in malformed.items():
        settings_file.write_bytes(payload.encode("utf-8"))
        process, window = start(executable, environment)
        try:
            pause(0.1)
            assert user.GetWindowLongPtrW(window, -20) & 8, (name, "Malformed settings were accepted")
            assert settings_file.read_bytes() == payload.encode("utf-8"), (name, "Input was overwritten")
            results.append(name)
        finally:
            assert user.PostMessageW(window, 0x0010, 0, 0)
            assert process.wait(timeout=5) == 0
    return results


def start(executable, environment=None):
    process = subprocess.Popen([str(executable)], env=environment)
    for _ in range(100):
        window = find_owned_window(process.pid)
        if window and user.IsWindowVisible(window):
            return process, window
        if process.poll() is not None:
            raise AssertionError(f"Loupe exited early: {process.returncode}")
        pause(0.03)
    process.terminate()
    process.wait()
    raise AssertionError("Loupe did not create its window")



def move_over_fixture(window, helper, x, y):
    assert user.SetWindowPos(helper, c.c_void_p(-1), x, y, 0, 0, 0x11)
    assert user.SetWindowPos(window, c.c_void_p(-1), x, y, 0, 0, 0x11)
    pause(0.2)


def verify_palette(window, helper):
    for color, text in ((0, "#000000"), (0xFFFFFF, "#FFFFFF"),
                        (0x0080FF, "#FF8000"), (0x0F0100, "#00010F")):
        paint_color(helper, color)
        await_caption(window, text)
    pause(0.3)
    assert caption(window).endswith("#00010F")


def verify_grid_center(window, helper):
    dpi = user.GetDpiForWindow(window)
    center = 32 * dpi // 96
    colors = [index * 4 | (255 - index * 3) << 8 | 128 << 16 for index in range(49)]
    dc = user.GetDC(helper)
    try:
        for index, color in enumerate(colors):
            assert gdi.SetPixel(dc, center - 3 + index % 7, center - 3 + index // 7, color) == color
        gdi.GdiFlush()
    finally:
        user.ReleaseDC(helper, dc)
    await_caption(window, "#60B780")
    left, top, _, _ = bounds(window)
    # Moving the lens by exactly one physical pixel must sample the neighbouring source pixel.
    assert user.SetWindowPos(window, None, left + 1, top, 0, 0, 0x15)
    await_caption(window, "#64B480")
    assert user.SetWindowPos(window, None, left, top + 1, 0, 0, 0x15)
    await_caption(window, "#7CA280")


def verify(window, helper, process):
    results = []
    monitors = []
    @monitor_callback
    def collect(handle, dc, rectangle, data):
        rect = rectangle.contents
        monitors.append([rect.left, rect.top, rect.right, rect.bottom])
        return True
    assert user.EnumDisplayMonitors(None, None, collect, 0)
    affinity = w.DWORD()
    assert user.GetWindowDisplayAffinity(window, c.byref(affinity)) and affinity.value == 0x11
    for left, top, right, bottom in monitors:
        x, y = left + (right - left) // 2, top + (bottom - top) // 2
        move_over_fixture(window, helper, x, y)
        dpi = user.GetDpiForWindow(window)
        rectangle = bounds(window)
        assert rectangle[2] - rectangle[0] == 240 * dpi // 96, (rectangle, dpi)
        assert rectangle[3] - rectangle[1] == 64 * dpi // 96, (rectangle, dpi)
        assert not user.GetWindowLongPtrW(window, -16) & 0x00C40000
        assert user.GetWindowLongPtrW(window, -20) & 8
        verify_palette(window, helper)
        verify_grid_center(window, helper)
        verify_hit_regions(window)
        verify_double_click(window)
        results.append({"bounds": [left, top, right, bottom], "dpi": dpi,
                        "palettePassed": True, "onePixelMovementPassed": True,
                        "hitRegionsPassed": True, "doubleClickGeometryPreserved": True})
    handle = kernel.OpenProcess(0x0400, False, process.pid)
    assert handle
    try:
        def resource_floor():
            counts = []
            for _ in range(20):
                counts.append(user.GetGuiResources(handle, 0))
                pause(0.017)
            return min(counts)
        before = resource_floor()
        pause(3)
        after = resource_floor()
        assert before == after, (before, after)
    finally:
        kernel.CloseHandle(handle)
    copied = verify_copy_formats(window, helper)
    return {"monitors": results, "gdiObjects": [before, after], "copiedValues": copied,
            "captureExclusion": affinity.value, "automaticPointerOrKeyboardInput": False}


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, default=root / "build/NeNeLoupe.exe")
    arguments = parser.parse_args()
    output = root / "out/window-verification"
    output.mkdir(parents=True, exist_ok=True)
    executable = output / "NeNeLoupe.exe"
    shutil.copy2(arguments.executable, executable)
    settings_root = Path(tempfile.mkdtemp(prefix="settings-", dir=output)).resolve()
    assert settings_root.is_relative_to(output.resolve())
    environment = dict(os.environ, LOCALAPPDATA=str(settings_root))
    settings_file = settings_root / "NeNeLoupe/settings.v1.txt"
    assert user.SetProcessDpiAwarenessContext(c.c_void_p(-4))
    helper = user.CreateWindowExW(8, "STATIC", "NeNe Loupe controlled backdrop", 0x80000000,
                                  400, 200, 180, 160, None, None, None, None)
    assert helper
    user.SetWindowLongPtrW(helper, -4, c.cast(fixture_procedure, c.c_void_p).value)
    process = None
    try:
        user.ShowWindow(helper, 4)
        process, window = start(executable, environment)
        result = verify(window, helper, process)
        result["settings"] = verify_settings(window, process)
        assert user.PostMessageW(window, 0x0100, 0x1B, 0)
        assert process.wait(timeout=5) == 0
        result["escapeExit"] = 0
        expected_settings = "schema=1\ntheme=dark\nformat=hex\nlayer=normal\n"
        assert settings_file.read_bytes() == expected_settings.encode("utf-8")
        process, window = start(executable, environment)
        assert not user.GetWindowLongPtrW(window, -20) & 8, "Topmost must remain off after restart"
        assert user.PostMessageW(window, 0x0010, 0, 0)
        assert process.wait(timeout=5) == 0
        result["settings"]["restartPreserved"] = True
        result["settings"]["saved"] = expected_settings.splitlines()
        result["settings"]["rejectedWithoutOverwrite"] = verify_rejected_settings(
            executable, environment, settings_file, helper)
        (output / "lens-results.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(result, indent=2))
    finally:
        if process and process.poll() is None:
            user.PostMessageW(window, 0x0010, 0, 0)
            process.wait(timeout=5)
        user.DestroyWindow(helper)


if __name__ == "__main__":
    main()
