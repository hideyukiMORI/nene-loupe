#include "PopupCaptureExclusion.hpp"

#include "CaptureExclusionStyle.hpp"

#include <array>
#include <exception>

namespace neneloupe
{
namespace
{
constexpr wchar_t popup_menu_class[] = L"#32768";
}

std::expected<std::unique_ptr<PopupCaptureExclusion>, PopupCaptureExclusion::Failure>
PopupCaptureExclusion::create(HWND owner, CaptureExclusion exclusion)
{
    auto guard = std::unique_ptr<PopupCaptureExclusion>(new PopupCaptureExclusion());
    // 除外しないと決めた起動では、メニューにも掛けない（ADR 0007）。
    if (!CaptureExclusionStyle::excludes(exclusion))
    {
        return guard;
    }
    const DWORD thread = GetWindowThreadProcessId(owner, nullptr);
    if (thread == 0)
    {
        return std::unexpected(Failure::installation_failed);
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, callback, nullptr, thread);
    if (!hook)
    {
        return std::unexpected(Failure::installation_failed);
    }
    guard->hook_ = hook;
    return guard;
}

PopupCaptureExclusion::PopupCaptureExclusion() noexcept = default;

PopupCaptureExclusion::~PopupCaptureExclusion()
{
    if (hook_)
    {
        UnhookWindowsHookEx(hook_);
    }
}

bool PopupCaptureExclusion::is_popup_menu(HWND window) noexcept
{
    std::array<wchar_t, std::size(popup_menu_class)> name{};
    return GetClassNameW(window, name.data(), static_cast<int>(name.size())) > 0 &&
           name == std::to_array(popup_menu_class);
}

void PopupCaptureExclusion::exclude_or_close(HWND window) noexcept
{
    DWORD affinity = WDA_NONE;
    if (GetWindowDisplayAffinity(window, &affinity) && affinity == WDA_EXCLUDEFROMCAPTURE)
    {
        return;
    }
    if (SetWindowDisplayAffinity(window, WDA_EXCLUDEFROMCAPTURE))
    {
        return;
    }
    if (!DestroyWindow(window) && IsWindow(window))
    {
        std::terminate();
    }
}

LRESULT CALLBACK PopupCaptureExclusion::callback(int code, WPARAM word, LPARAM data) noexcept
{
    if (code == HC_ACTION)
    {
        const auto &message = *reinterpret_cast<const CWPSTRUCT *>(data);
        const bool window_position_shows =
            message.message == WM_WINDOWPOSCHANGING &&
            (reinterpret_cast<const WINDOWPOS *>(message.lParam)->flags & SWP_SHOWWINDOW) != 0;
        const bool show_message_shows = message.message == WM_SHOWWINDOW && message.wParam != FALSE;
        if ((window_position_shows || show_message_shows) && is_popup_menu(message.hwnd))
        {
            exclude_or_close(message.hwnd);
        }
    }
    return CallNextHookEx(nullptr, code, word, data);
}
} // namespace neneloupe
