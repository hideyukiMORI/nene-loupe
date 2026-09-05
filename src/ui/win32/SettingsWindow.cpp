#include "SettingsWindow.hpp"

#include "SettingsLayout.hpp"
#include "SettingsRenderer.hpp"
#include "WindowLayerStyle.hpp"

#include <windowsx.h>

namespace neneloupe
{
namespace
{
constexpr wchar_t class_name[] = L"NeNeLoupe.Settings";
}

SettingsWindow::SettingsWindow(HINSTANCE instance, HWND owner, LoupeController &controller)
    : instance_(instance), owner_(owner), controller_(controller)
{
}

std::expected<std::unique_ptr<SettingsWindow>, WindowFailure>
SettingsWindow::create(HINSTANCE instance, HWND owner, LoupeController &controller)
{
    auto window = std::unique_ptr<SettingsWindow>(new SettingsWindow(instance, owner, controller));
    auto initialized = window->initialize();
    if (!initialized)
    {
        return std::unexpected(initialized.error());
    }
    return window;
}

std::expected<void, WindowFailure> SettingsWindow::initialize()
{
    WNDCLASSW registration{};
    registration.hInstance = instance_;
    registration.lpfnWndProc = procedure;
    registration.lpszClassName = class_name;
    registration.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    class_ = RegisterClassW(&registration);
    if (!class_)
    {
        return std::unexpected(WindowFailure::registration_failed);
    }
    if (!CreateWindowExW(WindowLayerStyle::extended(controller_.layer()), class_name,
                         L"NeNe Loupe 設定", WS_POPUP, 0, 0, SettingsLayout::width,
                         SettingsLayout::height, owner_, nullptr, instance_, this))
    {
        return std::unexpected(WindowFailure::creation_failed);
    }
    // ルーペ窓と同じ規則を 2 枚目にも通す。掛けないとレンズが自分自身を採る。
    if (!SetWindowDisplayAffinity(window_, WDA_EXCLUDEFROMCAPTURE))
    {
        return std::unexpected(WindowFailure::capture_exclusion_failed);
    }
    dpi_ = GetDpiForWindow(window_);
    place();
    EnableWindow(owner_, FALSE);
    ShowWindow(window_, SW_SHOWNORMAL);
    SetForegroundWindow(window_);
    return {};
}

void SettingsWindow::place()
{
    const auto size = SettingsLayout::window(dpi_);
    RECT owner{};
    GetWindowRect(owner_, &owner);
    MONITORINFO monitor{};
    monitor.cbSize = sizeof(monitor);
    GetMonitorInfoW(MonitorFromWindow(owner_, MONITOR_DEFAULTTONEAREST), &monitor);
    const LONG width = size.right - size.left;
    const LONG height = size.bottom - size.top;
    LONG left = (owner.left + owner.right) / 2 - width / 2;
    LONG top = (owner.top + owner.bottom) / 2 - height / 2;
    left = std::max(monitor.rcWork.left, std::min(left, monitor.rcWork.right - width));
    top = std::max(monitor.rcWork.top, std::min(top, monitor.rcWork.bottom - height));
    SetWindowPos(window_, nullptr, left, top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

SettingsWindow::~SettingsWindow()
{
    // 先に入力を戻してから前面へ返す。順を逆にすると別のアプリが前へ出る。
    if (owner_)
    {
        EnableWindow(owner_, TRUE);
    }
    if (window_)
    {
        DestroyWindow(window_);
    }
    if (owner_)
    {
        SetForegroundWindow(owner_);
    }
    if (class_)
    {
        UnregisterClassW(class_name, instance_);
    }
}

bool SettingsWindow::is_open() const noexcept
{
    return window_ != nullptr;
}

HWND SettingsWindow::handle() const noexcept
{
    return window_;
}

LRESULT CALLBACK SettingsWindow::procedure(HWND window, UINT message, WPARAM word,
                                           LPARAM data) noexcept
{
    auto *self = reinterpret_cast<SettingsWindow *>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE)
    {
        self =
            static_cast<SettingsWindow *>(reinterpret_cast<CREATESTRUCTW *>(data)->lpCreateParams);
        self->window_ = window;
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (!self)
    {
        return DefWindowProcW(window, message, word, data);
    }
    const auto result = self->dispatch(message, word, data);
    if (message == WM_NCDESTROY)
    {
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        self->window_ = nullptr;
    }
    return result;
}

LRESULT SettingsWindow::dispatch(UINT message, WPARAM word, LPARAM data)
{
    switch (message)
    {
    case WM_PAINT:
        render();
        return 0;
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_LBUTTONDOWN:
        pressed_ = SettingsLayout::hit_test(POINT{GET_X_LPARAM(data), GET_Y_LPARAM(data)}, dpi_);
        if (pressed_ == SettingsHitArea::none)
        {
            SendMessageW(window_, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        }
        return 0;
    case WM_LBUTTONUP:
        if (pressed_ != SettingsHitArea::none &&
            pressed_ ==
                SettingsLayout::hit_test(POINT{GET_X_LPARAM(data), GET_Y_LPARAM(data)}, dpi_))
        {
            activate(pressed_);
        }
        pressed_ = SettingsHitArea::none;
        return 0;
    case WM_KEYDOWN:
        if (word == VK_ESCAPE)
        {
            DestroyWindow(window_);
        }
        return 0;
    case WM_CLOSE:
        DestroyWindow(window_);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_DPICHANGED:
        change_dpi(word, data);
        return 0;
    default:
        return DefWindowProcW(window_, message, word, data);
    }
}

void SettingsWindow::render()
{
    const auto frame = controller_.settings_frame();
    PAINTSTRUCT painting{};
    HDC dc = BeginPaint(window_, &painting);
    if (dc)
    {
        SettingsRenderer::render(dc, frame, dpi_);
    }
    EndPaint(window_, &painting);
}

void SettingsWindow::activate(SettingsHitArea area)
{
    switch (area)
    {
    case SettingsHitArea::none:
        return;
    case SettingsHitArea::close:
        DestroyWindow(window_);
        return;
    case SettingsHitArea::theme_dark:
        controller_.select_theme(Theme::dark);
        break;
    case SettingsHitArea::theme_light:
        controller_.select_theme(Theme::light);
        break;
    case SettingsHitArea::theme_system:
        controller_.select_theme(Theme::system);
        break;
    case SettingsHitArea::topmost:
        controller_.select_layer(next_layer(controller_.layer()));
        WindowLayerStyle::apply(owner_, window_, controller_.layer());
        break;
    }
    InvalidateRect(window_, nullptr, FALSE);
    InvalidateRect(owner_, nullptr, FALSE);
}

void SettingsWindow::change_dpi(WPARAM word, LPARAM data)
{
    dpi_ = LOWORD(word);
    const auto &suggested = *reinterpret_cast<const RECT *>(data);
    SetWindowPos(window_, nullptr, suggested.left, suggested.top, suggested.right - suggested.left,
                 suggested.bottom - suggested.top, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window_, nullptr, FALSE);
}
} // namespace neneloupe
