"""Interactive Windows verification, separate from unit tests (Issue #3 / QLT-013).

Owns and closes its test windows without moving the pointer or generating keyboard input.
Requires an unlocked interactive Windows session; writes numeric evidence only.
"""

import ctypes as c
from ctypes import wintypes as w
import json
from pathlib import Path
import subprocess
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
api(gdi, "CreateSolidBrush", w.HBRUSH, w.DWORD)
api(gdi, "DeleteObject", w.BOOL, w.HGDIOBJ)
api(gdi, "GdiFlush", w.BOOL)
api(gdi, "GetPixel", w.DWORD, w.HDC, c.c_int, c.c_int)
api(gdi, "SetPixel", w.DWORD, w.HDC, c.c_int, c.c_int, w.DWORD)
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
        assert rectangle[2] - rectangle[0] == 160 * dpi // 96, (rectangle, dpi)
        assert rectangle[3] - rectangle[1] == 64 * dpi // 96, (rectangle, dpi)
        assert not user.GetWindowLongPtrW(window, -16) & 0x00C40000
        assert user.GetWindowLongPtrW(window, -20) & 8
        verify_palette(window, helper)
        verify_grid_center(window, helper)
        results.append({"bounds": [left, top, right, bottom], "dpi": dpi,
                        "palettePassed": True, "onePixelMovementPassed": True})
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
    assert user.PostMessageW(window, 0x0100, 0x1B, 0)
    assert process.wait(timeout=5) == 0
    return {"monitors": results, "gdiObjects": [before, after], "escapeExit": 0,
            "captureExclusion": affinity.value, "automaticPointerOrKeyboardInput": False}


def main():
    root = Path(__file__).resolve().parents[1]
    output = root / "out/window-verification"
    output.mkdir(parents=True, exist_ok=True)
    if user.FindWindowW("NeNeLoupe.Window", None):
        raise SystemExit("Close the existing loupe before this verification.")
    assert user.SetProcessDpiAwarenessContext(c.c_void_p(-4))
    helper = user.CreateWindowExW(8, "STATIC", "NeNe Loupe controlled backdrop", 0x80000000,
                                  400, 200, 180, 160, None, None, None, None)
    assert helper
    user.SetWindowLongPtrW(helper, -4, c.cast(fixture_procedure, c.c_void_p).value)
    process = None
    try:
        user.ShowWindow(helper, 4)
        process, window = start(root / "build/NeNeLoupe.exe")
        result = verify(window, helper, process)
        (output / "lens-results.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(result, indent=2))
    finally:
        if process and process.poll() is None:
            user.PostMessageW(window, 0x0010, 0, 0)
            process.wait(timeout=5)
        user.DestroyWindow(helper)


if __name__ == "__main__":
    main()
