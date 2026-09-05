#include "LoupeWindow.hpp"
#include "LoupeRenderer.hpp"

namespace neneloupe
{
namespace
{
constexpr wchar_t class_name[] = L"NeNeLoupe.Window";
constexpr UINT_PTR refresh_timer = 1;
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
        return std::unexpected(initialized.error());
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
        return std::unexpected(WindowFailure::registration_failed);
    if (!CreateWindowExW(WS_EX_TOPMOST | WS_EX_APPWINDOW, class_name, L"NeNe Loupe", WS_POPUP, 100,
                         100, 160, 64, nullptr, nullptr, instance_, this))
        return std::unexpected(WindowFailure::creation_failed);
    if (!SetWindowDisplayAffinity(window_, WDA_EXCLUDEFROMCAPTURE))
        return std::unexpected(WindowFailure::capture_exclusion_failed);
    dpi_ = GetDpiForWindow(window_);
    SetWindowPos(window_, nullptr, 0, 0, MulDiv(160, static_cast<int>(dpi_), 96),
                 MulDiv(64, static_cast<int>(dpi_), 96), SWP_NOMOVE | SWP_NOZORDER);
    if (!SetTimer(window_, refresh_timer, 30, nullptr))
        return std::unexpected(WindowFailure::timer_failed);
    refresh();
    ShowWindow(window_, SW_SHOWNORMAL);
    return {};
}

LoupeWindow::~LoupeWindow()
{
    if (window_)
        DestroyWindow(window_);
    if (class_)
        UnregisterClassW(class_name, instance_);
}

bool LoupeWindow::is_open() const noexcept
{
    return window_ != nullptr;
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
        return DefWindowProcW(window, message, word, data);
    const auto result = self->dispatch(message, word, data);
    if (message == WM_NCDESTROY)
    {
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        self->window_ = nullptr;
    }
    return result;
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
            refresh();
        return 0;
    case WM_NCHITTEST:
        return HTCAPTION;
    case WM_KEYDOWN:
        if (word == VK_ESCAPE)
            DestroyWindow(window_);
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
        LoupeRenderer::render(dc, frame, dpi_);
    EndPaint(window_, &painting);
}

void LoupeWindow::refresh()
{
    controller_.refresh(sampling_position());
    const auto frame = controller_.frame();
    const auto title = std::wstring(L"NeNe Loupe — ") + frame.caption();
    SetWindowTextW(window_, title.c_str());
    InvalidateRect(window_, nullptr, FALSE);
}

std::expected<ScreenPosition, SamplingFailure> LoupeWindow::sampling_position() const
{
    auto center = LoupeRenderer::lens_center(dpi_);
    if (!ClientToScreen(window_, &center))
        return std::unexpected(SamplingFailure::position_unavailable);
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
