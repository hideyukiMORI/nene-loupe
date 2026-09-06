#include "LoupeWindow.hpp"

#include "ColorText.hpp"
#include "LoupeLayout.hpp"
#include "LoupeRenderer.hpp"
#include "PopupCaptureExclusion.hpp"
#include "WindowLayerStyle.hpp"

#include <algorithm>
#include <array>
#include <windowsx.h>

namespace neneloupe
{
namespace
{
constexpr wchar_t class_name[] = L"NeNeLoupe.Window";
constexpr UINT_PTR refresh_timer = 1;
constexpr UINT_PTR copy_timer = 2;
constexpr UINT copy_notice_milliseconds = 900;
constexpr std::array<ColorFormat, 5> menu_formats{ColorFormat::rgb_decimal, ColorFormat::rgb_hex,
                                                  ColorFormat::cmyk, ColorFormat::hsl,
                                                  ColorFormat::hsv};

std::wstring widen(const std::string &text)
{
    return std::wstring(text.begin(), text.end());
}

bool moved_past_drag(POINT anchor, POINT current)
{
    return std::abs(current.x - anchor.x) > GetSystemMetrics(SM_CXDRAG) ||
           std::abs(current.y - anchor.y) > GetSystemMetrics(SM_CYDRAG);
}
} // namespace

LoupeWindow::LoupeWindow(HINSTANCE instance, LoupeController &controller)
    : instance_(instance), controller_(controller)
{
}

std::expected<std::unique_ptr<LoupeWindow>, WindowFailure>
LoupeWindow::create(HINSTANCE instance, LoupeController &controller)
{
    auto window = std::unique_ptr<LoupeWindow>(new LoupeWindow(instance, controller));
    auto initialized = window->initialize();
    if (!initialized)
    {
        return std::unexpected(initialized.error());
    }
    return window;
}

std::expected<void, WindowFailure> LoupeWindow::initialize()
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
    const auto topmost = WindowLayerStyle::extended(controller_.layer());
    if (!CreateWindowExW(topmost | WS_EX_APPWINDOW, class_name, L"NeNe Loupe", WS_POPUP, 100, 100,
                         LoupeLayout::width, LoupeLayout::height, nullptr, nullptr, instance_,
                         this))
    {
        return std::unexpected(WindowFailure::creation_failed);
    }
    if (!SetWindowDisplayAffinity(window_, WDA_EXCLUDEFROMCAPTURE))
    {
        return std::unexpected(WindowFailure::capture_exclusion_failed);
    }
    dpi_ = GetDpiForWindow(window_);
    const auto bounds = LoupeLayout::window(dpi_);
    SetWindowPos(window_, nullptr, 0, 0, bounds.right, bounds.bottom,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (!SetTimer(window_, refresh_timer, 30, nullptr))
    {
        return std::unexpected(WindowFailure::timer_failed);
    }
    refresh();
    ShowWindow(window_, SW_SHOWNORMAL);
    return {};
}

LoupeWindow::~LoupeWindow()
{
    settings_.reset();
    if (window_)
    {
        DestroyWindow(window_);
    }
    if (class_)
    {
        UnregisterClassW(class_name, instance_);
    }
}

bool LoupeWindow::is_open() const noexcept
{
    return window_ != nullptr;
}

HWND LoupeWindow::handle() const noexcept
{
    return window_;
}

LRESULT CALLBACK LoupeWindow::procedure(HWND window, UINT message, WPARAM word,
                                        LPARAM data) noexcept
{
    auto *self = reinterpret_cast<LoupeWindow *>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE)
    {
        self = static_cast<LoupeWindow *>(reinterpret_cast<CREATESTRUCTW *>(data)->lpCreateParams);
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

bool LoupeWindow::controls_enabled() const
{
    return controller_.frame().has_color();
}

// スクリーン座標で来る WM_NCHITTEST をクライアント座標へ直してから判定する。
// 3 つの操作部だけ HTCLIENT を返し、残りは HTCAPTION のまま（ADR 0006 第 1 節）。
LRESULT LoupeWindow::hit_result(LPARAM data) const
{
    POINT point{GET_X_LPARAM(data), GET_Y_LPARAM(data)};
    if (!ScreenToClient(window_, &point))
    {
        return HTCAPTION;
    }
    switch (LoupeLayout::hit_test(point, dpi_))
    {
    case LoupeHitArea::none:
        return HTCAPTION;
    case LoupeHitArea::gear:
        return HTCLIENT;
    case LoupeHitArea::format_chip:
    case LoupeHitArea::value:
        return controls_enabled() ? HTCLIENT : HTCAPTION;
    }
    std::unreachable();
}

void LoupeWindow::track_hover(POINT point)
{
    if (!tracking_)
    {
        TRACKMOUSEEVENT tracking{sizeof(TRACKMOUSEEVENT), TME_LEAVE, window_, 0};
        tracking_ = TrackMouseEvent(&tracking) != FALSE;
    }
    const auto area = LoupeLayout::hit_test(point, dpi_);
    const auto next = controls_enabled() ? area : LoupeHitArea::none;
    if (next != hover_)
    {
        hover_ = next;
        InvalidateRect(window_, nullptr, FALSE);
    }
}

LRESULT LoupeWindow::on_mouse(UINT message, LPARAM data)
{
    const POINT point{GET_X_LPARAM(data), GET_Y_LPARAM(data)};
    if (message == WM_LBUTTONDOWN)
    {
        pressed_ = LoupeLayout::hit_test(point, dpi_);
        anchor_ = point;
        SetCapture(window_);
        return 0;
    }
    if (message == WM_MOUSEMOVE)
    {
        // 掴んだまま動かしたらクリックを取り消し、窓の移動へ渡す。
        if (pressed_ != LoupeHitArea::none && moved_past_drag(anchor_, point))
        {
            pressed_ = LoupeHitArea::none;
            ReleaseCapture();
            begin_caption_drag(point);
            return 0;
        }
        track_hover(point);
        return 0;
    }
    ReleaseCapture();
    if (pressed_ != LoupeHitArea::none && pressed_ == LoupeLayout::hit_test(point, dpi_))
    {
        activate(pressed_);
    }
    pressed_ = LoupeHitArea::none;
    return 0;
}

void LoupeWindow::begin_caption_drag(POINT point)
{
    if (!ClientToScreen(window_, &point))
    {
        return;
    }
    SendMessageW(window_, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
}

LRESULT LoupeWindow::dispatch(UINT message, WPARAM word, LPARAM data)
{
    switch (message)
    {
    case WM_PAINT:
        render();
        return 0;
    case WM_TIMER:
        if (word == refresh_timer)
        {
            refresh();
        }
        if (word == copy_timer)
        {
            KillTimer(window_, copy_timer);
            controller_.clear_copy_state();
            InvalidateRect(window_, nullptr, FALSE);
        }
        return 0;
    case WM_NCHITTEST:
        return hit_result(data);
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
        return on_mouse(message, data);
    case WM_MOUSELEAVE:
        tracking_ = false;
        hover_ = LoupeHitArea::none;
        InvalidateRect(window_, nullptr, FALSE);
        return 0;
    case WM_RBUTTONUP:
        show_format_menu(POINT{GET_X_LPARAM(data), GET_Y_LPARAM(data)});
        return 0;
    case WM_SETTINGCHANGE:
        controller_.refresh_appearance();
        InvalidateRect(window_, nullptr, FALSE);
        if (settings_ && settings_->is_open())
        {
            InvalidateRect(settings_->handle(), nullptr, FALSE);
        }
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

void LoupeWindow::render()
{
    const auto frame = controller_.frame();
    PAINTSTRUCT painting{};
    HDC dc = BeginPaint(window_, &painting);
    if (dc)
    {
        LoupeRenderer::render(dc, frame, dpi_, hover_);
    }
    EndPaint(window_, &painting);
}

void LoupeWindow::refresh()
{
    if (settings_ && !settings_->is_open())
    {
        settings_.reset();
    }
    controller_.refresh(sampling_position());
    const auto frame = controller_.frame();
    // 変わったときだけ書き換える。30 ms ごとにタスクバーを更新しない。
    auto title = std::wstring(L"NeNe Loupe — ") + frame.caption();
    if (title != title_)
    {
        title_ = std::move(title);
        SetWindowTextW(window_, title_.c_str());
    }
    InvalidateRect(window_, nullptr, FALSE);
}

void LoupeWindow::activate(LoupeHitArea area)
{
    switch (area)
    {
    case LoupeHitArea::none:
        return;
    case LoupeHitArea::format_chip:
        if (!controls_enabled())
        {
            return;
        }
        controller_.cycle_format();
        break;
    case LoupeHitArea::gear:
        open_settings();
        return;
    case LoupeHitArea::value:
        if (!controls_enabled())
        {
            return;
        }
        controller_.copy_current_value();
        SetTimer(window_, copy_timer, copy_notice_milliseconds, nullptr);
        break;
    }
    InvalidateRect(window_, nullptr, FALSE);
}

void LoupeWindow::open_settings()
{
    if (settings_ && settings_->is_open())
    {
        SetForegroundWindow(settings_->handle());
        return;
    }
    auto opened = SettingsWindow::create(instance_, window_, controller_);
    if (!opened)
    {
        return;
    }
    settings_ = std::move(*opened);
}

void LoupeWindow::show_format_menu(POINT client)
{
    if (LoupeLayout::hit_test(client, dpi_) != LoupeHitArea::format_chip || !controls_enabled())
    {
        return;
    }
    HMENU menu = CreatePopupMenu();
    if (!menu)
    {
        return;
    }
    for (std::size_t index = 0; index < menu_formats.size(); ++index)
    {
        AppendMenuW(menu, MF_STRING, index + 1,
                    widen(ColorText::label(menu_formats[index])).c_str());
    }
    const auto selected = std::find(menu_formats.begin(), menu_formats.end(), controller_.format());
    CheckMenuRadioItem(menu, 1, static_cast<UINT>(menu_formats.size()),
                       static_cast<UINT>(selected - menu_formats.begin() + 1), MF_BYCOMMAND);
    POINT screen = client;
    ClientToScreen(window_, &screen);
    int chosen = 0;
    {
        auto capture_exclusion = PopupCaptureExclusion::create(window_);
        if (!capture_exclusion)
        {
            DestroyMenu(menu);
            return;
        }
        chosen = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screen.x, screen.y, 0,
                                window_, nullptr);
    }
    DestroyMenu(menu);
    if (chosen >= 1 && chosen <= static_cast<int>(menu_formats.size()))
    {
        controller_.select_format(menu_formats[static_cast<std::size_t>(chosen - 1)]);
        InvalidateRect(window_, nullptr, FALSE);
    }
}

std::expected<ScreenPosition, SamplingFailure> LoupeWindow::sampling_position() const
{
    auto center = LoupeLayout::lens_center(dpi_);
    if (!ClientToScreen(window_, &center))
    {
        return std::unexpected(SamplingFailure::position_unavailable);
    }
    return ScreenPosition::from_physical_pixels(static_cast<int>(center.x),
                                                static_cast<int>(center.y));
}

void LoupeWindow::change_dpi(WPARAM word, LPARAM data)
{
    dpi_ = LOWORD(word);
    const auto &suggested = *reinterpret_cast<const RECT *>(data);
    SetWindowPos(window_, nullptr, suggested.left, suggested.top, suggested.right - suggested.left,
                 suggested.bottom - suggested.top, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window_, nullptr, FALSE);
}
} // namespace neneloupe
