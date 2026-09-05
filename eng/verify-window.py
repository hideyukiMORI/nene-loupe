"""Interactive Windows verification, separate from unit tests (Issue #3 / QLT-013).

Moves the pointer temporarily; owns and closes its test windows. Saves only the loupe's
client area, never the desktop. Requires an unlocked interactive Windows session.
"""

import ctypes as c
from ctypes import wintypes as w
import json
from pathlib import Path
import struct
import subprocess
import time
import zlib

user = c.WinDLL("user32", use_last_error=True)
gdi = c.WinDLL("gdi32", use_last_error=True)
kernel = c.WinDLL("kernel32", use_last_error=True)


def api(dll, name, result, *arguments):
    function = getattr(dll, name)
    function.restype, function.argtypes = result, arguments
    return function


api(user, "SetProcessDpiAwarenessContext", w.BOOL, w.HANDLE)
api(user, "GetCursorPos", w.BOOL, c.POINTER(w.POINT))
api(user, "SetCursorPos", w.BOOL, c.c_int, c.c_int)
api(user, "GetForegroundWindow", w.HWND)
api(user, "SetForegroundWindow", w.BOOL, w.HWND)
api(user, "CreateWindowExW", w.HWND, w.DWORD, w.LPCWSTR, w.LPCWSTR, w.DWORD,
    c.c_int, c.c_int, c.c_int, c.c_int, w.HWND, w.HMENU, w.HINSTANCE, w.LPVOID)
api(user, "DestroyWindow", w.BOOL, w.HWND)
api(user, "ShowWindow", w.BOOL, w.HWND, c.c_int)
api(user, "IsWindowVisible", w.BOOL, w.HWND)
api(user, "FindWindowW", w.HWND, w.LPCWSTR, w.LPCWSTR)
api(user, "GetWindowThreadProcessId", w.DWORD, w.HWND, c.POINTER(w.DWORD))
api(user, "GetGUIThreadInfo", w.BOOL, w.DWORD, w.LPVOID)
api(user, "GetWindowRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "GetClientRect", w.BOOL, w.HWND, c.POINTER(w.RECT))
api(user, "GetWindowTextW", c.c_int, w.HWND, w.LPWSTR, c.c_int)
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
api(user, "mouse_event", None, w.DWORD, w.DWORD, w.DWORD, w.DWORD, c.c_size_t)
api(user, "keybd_event", None, w.BYTE, w.BYTE, w.DWORD, c.c_size_t)
api(user, "GetGuiResources", w.DWORD, w.HANDLE, w.DWORD)
api(gdi, "CreateSolidBrush", w.HBRUSH, w.DWORD)
api(gdi, "DeleteObject", w.BOOL, w.HGDIOBJ)
api(gdi, "GdiFlush", w.BOOL)
api(gdi, "GetPixel", w.DWORD, w.HDC, c.c_int, c.c_int)
api(gdi, "SetPixel", w.DWORD, w.HDC, c.c_int, c.c_int, w.DWORD)
api(gdi, "CreateCompatibleDC", w.HDC, w.HDC)
api(gdi, "CreateDIBSection", w.HBITMAP, w.HDC, w.LPVOID, w.UINT, c.POINTER(w.LPVOID), w.HANDLE, w.DWORD)
api(gdi, "SelectObject", w.HGDIOBJ, w.HDC, w.HGDIOBJ)
api(gdi, "DeleteDC", w.BOOL, w.HDC)
api(gdi, "BitBlt", w.BOOL, w.HDC, c.c_int, c.c_int, c.c_int, c.c_int, w.HDC, c.c_int, c.c_int, w.DWORD)
api(kernel, "OpenProcess", w.HANDLE, w.DWORD, w.BOOL, w.DWORD)
api(kernel, "CloseHandle", w.BOOL, w.HANDLE)
monitor_callback = c.WINFUNCTYPE(w.BOOL, w.HANDLE, w.HDC, c.POINTER(w.RECT), w.LPARAM)
api(user, "EnumDisplayMonitors", w.BOOL, w.HDC, c.POINTER(w.RECT), monitor_callback, w.LPARAM)



def pause(seconds):
    deadline = time.monotonic() + seconds
    message = w.MSG()
    while time.monotonic() < deadline:
        while user.PeekMessageW(c.byref(message), None, 0, 0, 1):
            user.TranslateMessage(c.byref(message))
            user.DispatchMessageW(c.byref(message))
        time.sleep(0.005)


window_callback = c.WINFUNCTYPE(c.c_ssize_t, w.HWND, w.UINT, w.WPARAM, w.LPARAM)


class GuiThreadInfo(c.Structure):
    _fields_ = [("size", w.DWORD), ("flags", w.DWORD), ("active", w.HWND),
                ("focus", w.HWND), ("capture", w.HWND), ("menu", w.HWND),
                ("moving", w.HWND), ("caret", w.HWND), ("caretRect", w.RECT)]


def await_drag(window):
    thread = user.GetWindowThreadProcessId(window, None)
    for _ in range(100):
        info = GuiThreadInfo()
        info.size = c.sizeof(info)
        assert user.GetGUIThreadInfo(thread, c.byref(info))
        if info.moving == window:
            return
        pause(0.03)
    raise AssertionError("Window did not enter the native move loop")


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


def await_caption(window, expected):
    for _ in range(100):
        if caption(window).endswith(expected):
            return
        pause(0.03)
    cursor = w.POINT()
    user.GetCursorPos(c.byref(cursor))
    raise AssertionError(f"Expected {expected}; actual {caption(window)}; cursor={cursor.x},{cursor.y}")


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


def screenshot(window, output):
    rect = w.RECT()
    assert user.GetClientRect(window, c.byref(rect))
    dc = user.GetDC(window)
    memory = gdi.CreateCompatibleDC(dc)
    header = c.create_string_buffer(struct.pack("<IiiHHIIiiII", 40, rect.right, -rect.bottom, 1, 32, 0, 0, 0, 0, 0, 0))
    pixels = w.LPVOID()
    bitmap = gdi.CreateDIBSection(dc, header, 0, c.byref(pixels), None, 0)
    assert memory and bitmap
    previous = gdi.SelectObject(memory, bitmap)
    try:
        assert gdi.BitBlt(memory, 0, 0, rect.right, rect.bottom, dc, 0, 0, 0x00CC0020)
        assert gdi.GdiFlush()
        raw = c.string_at(pixels, rect.right * rect.bottom * 4)
        rows = bytearray()
        for y in range(rect.bottom):
            rows.append(0)
            for x in range(rect.right):
                offset = (y * rect.right + x) * 4
                rows.extend((raw[offset + 2], raw[offset + 1], raw[offset]))
    finally:
        gdi.SelectObject(memory, previous)
        gdi.DeleteObject(bitmap)
        gdi.DeleteDC(memory)
        user.ReleaseDC(window, dc)
    def chunk(kind, data):
        return struct.pack("!I", len(data)) + kind + data + struct.pack("!I", zlib.crc32(kind + data))
    output.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack("!2I5B", rect.right, rect.bottom, 8, 2, 0, 0, 0))
                       + chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b""))


def start(executable):
    process = subprocess.Popen([str(executable)])
    for _ in range(100):
        window = user.FindWindowW("NeNeLoupe.Window", None)
        owner = w.DWORD()
        if window:
            user.GetWindowThreadProcessId(window, c.byref(owner))
            if owner.value == process.pid and user.IsWindowVisible(window):
                return process, window
        if process.poll() is not None:
            raise AssertionError(f"Loupe exited early: {process.returncode}")
        pause(0.03)
    process.terminate()
    process.wait()
    raise AssertionError("Loupe did not create its window")


def verify(window, helper, process, output):
    dpi = user.GetDpiForWindow(window)
    initial = bounds(window)
    assert initial[2] - initial[0] == 160 * dpi // 96
    assert initial[3] - initial[1] == 64 * dpi // 96
    assert not user.GetWindowLongPtrW(window, -16) & 0x00C40000  # caption / sizing frame
    assert user.GetWindowLongPtrW(window, -20) & 8  # topmost
    assert user.SendMessageW(window, 0x0084, 0, (initial[1] + 10) << 16 | (initial[0] + 10)) == 2
    user.SetCursorPos(480, 270)
    for color, text in ((0x000000, "#000000"), (0xFFFFFF, "#FFFFFF"),
                        (0x0080FF, "#FF8000"), (0x0F0100, "#00010F")):
        paint_color(helper, color)
        await_caption(window, text)
        pause(0.08)
        dc = user.GetDC(window)
        try:
            assert gdi.GetPixel(dc, 32 * dpi // 96, 32 * dpi // 96) == color, (hex(color), hex(gdi.GetPixel(dc, 32 * dpi // 96, 32 * dpi // 96)), caption(window))
            assert gdi.GetPixel(dc, 8 * dpi // 96, 8 * dpi // 96) == color
        finally:
            user.ReleaseDC(window, dc)
    paint_color(helper, 0x0080FF)
    await_caption(window, "#FF8000")
    pause(0.1)
    screenshot(window, output / "loupe.png")
    verify_grid(window, helper, dpi, output)
    handle = kernel.OpenProcess(0x0400, False, process.pid)
    assert handle
    try:
        def resource_floor():
            counts = []
            for _ in range(20):
                counts.append(user.GetGuiResources(handle, 0))
                pause(0.017)
            return min(counts)
        resources_before = resource_floor()
        pause(3)
        resources_after = resource_floor()
        assert resources_after == resources_before, (resources_before, resources_after)
    finally:
        kernel.CloseHandle(handle)
    user.SetForegroundWindow(window)
    user.SetCursorPos(initial[0] + 20, initial[1] + 20)
    drag_start = w.POINT()
    user.GetCursorPos(c.byref(drag_start))
    user.mouse_event(2, 0, 0, 0, 0)
    try:
        await_drag(window)
        user.SetCursorPos(initial[0] + 100, initial[1] + 70)
        drag_end = w.POINT()
        user.GetCursorPos(c.byref(drag_end))
        for _ in range(100):
            moved = bounds(window)
            if moved[0] == initial[0] + 80 and moved[1] == initial[1] + 50:
                break
            pause(0.03)
    finally:
        user.mouse_event(4, 0, 0, 0, 0)
    pause(0.15)
    moved = bounds(window)
    assert moved[0] - initial[0] == 80 and moved[1] - initial[1] == 50, (initial, moved, [drag_start.x, drag_start.y], [drag_end.x, drag_end.y])
    monitors = verify_monitors(window, helper)
    user.PostMessageW(window, 0x0100, 0x1B, 0)  # WM_KEYDOWN / Escape
    assert process.wait(timeout=5) == 0
    return {"dpi": dpi, "initialBounds": initial, "draggedBounds": moved,
            "palette": ["#000000", "#FFFFFF", "#FF8000", "#00010F"],
            "gdiObjects": [resources_before, resources_after], "escapeExit": 0,
            "monitors": monitors, "gridPixelsVerified": 49}


def verify_grid(window, helper, dpi, output):
    colors = [index * 4 | (255 - index * 3) << 8 | 128 << 16 for index in range(49)]
    dc = user.GetDC(helper)
    try:
        for index, color in enumerate(colors):
            assert gdi.SetPixel(dc, 77 + index % 7, 67 + index // 7, color) == color
        gdi.GdiFlush()
    finally:
        user.ReleaseDC(helper, dc)
    await_caption(window, "#60B780")
    pause(0.1)
    dc = user.GetDC(window)
    try:
        for index, color in enumerate(colors):
            x = (8 + index % 7 * 8) * dpi // 96
            y = (8 + index // 7 * 8) * dpi // 96
            assert gdi.GetPixel(dc, x, y) == color, index
    finally:
        user.ReleaseDC(window, dc)
    screenshot(window, output / "grid.png")


def verify_monitors(window, helper):
    monitors = []
    @monitor_callback
    def collect(handle, dc, rectangle, data):
        rect = rectangle.contents
        monitors.append([rect.left, rect.top, rect.right, rect.bottom])
        return True
    assert user.EnumDisplayMonitors(None, None, collect, 0)
    results = []
    for left, top, right, bottom in monitors:
        fixture_x, fixture_y = left + (right - left) // 2, top + (bottom - top) // 2
        assert user.SetWindowPos(helper, None, fixture_x, fixture_y, 0, 0, 0x15)
        assert user.SetWindowPos(window, None, left + 100, top + 100, 0, 0, 0x15)
        pause(0.2)
        paint_color(helper, 0x0080FF)
        user.SetCursorPos(fixture_x + 80, fixture_y + 70)
        await_caption(window, "#FF8000")
        dpi = user.GetDpiForWindow(window)
        rectangle = bounds(window)
        assert rectangle[2] - rectangle[0] == 160 * dpi // 96, (rectangle, dpi)
        assert rectangle[3] - rectangle[1] == 64 * dpi // 96, (rectangle, dpi)
        dc = user.GetDC(window)
        try:
            pause(0.1)
            assert gdi.GetPixel(dc, 32 * dpi // 96, 32 * dpi // 96) == 0x0080FF
        finally:
            user.ReleaseDC(window, dc)
        results.append({"bounds": [left, top, right, bottom], "dpi": dpi, "caption": caption(window)})
    return results


def main():
    root = Path(__file__).resolve().parents[1]
    output = root / "out/window-verification"
    output.mkdir(parents=True, exist_ok=True)
    if user.FindWindowW("NeNeLoupe.Window", None):
        raise SystemExit("Close the existing loupe before this verification.")
    user.SetProcessDpiAwarenessContext(c.c_void_p(-4))
    original = w.POINT()
    assert user.GetCursorPos(c.byref(original))
    foreground = user.GetForegroundWindow()
    helper = user.CreateWindowExW(8, "STATIC", "NeNe Loupe controlled color fixture", 0x80000000,
                                  400, 200, 180, 160, None, None, None, None)
    assert helper
    user.SetWindowLongPtrW(helper, -4, c.cast(fixture_procedure, c.c_void_p).value)
    process = None
    try:
        user.ShowWindow(helper, 5)
        process, window = start(root / "build/NeNeLoupe.exe")
        result = verify(window, helper, process, output)
        process, window = start(root / "build/NeNeLoupe.exe")
        user.SetForegroundWindow(window)
        user.keybd_event(0x12, 0, 0, 0)
        user.keybd_event(0x73, 0, 0, 0)
        user.keybd_event(0x73, 0, 2, 0)
        user.keybd_event(0x12, 0, 2, 0)
        result["altF4Exit"] = process.wait(timeout=5)
        assert result["altF4Exit"] == 0
        result["monitorCount"] = user.GetSystemMetrics(80)
        result["virtualScreen"] = [user.GetSystemMetrics(index) for index in (76, 77, 78, 79)]
        (output / "results.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(result, indent=2))
    finally:
        if process and process.poll() is None:
            process.terminate()
            process.wait()
        user.DestroyWindow(helper)
        user.SetCursorPos(original.x, original.y)
        if foreground:
            user.SetForegroundWindow(foreground)


if __name__ == "__main__":
    main()
