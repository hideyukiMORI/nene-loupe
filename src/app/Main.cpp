#include "CaptureExclusion.hpp"
#include "LoupeWindow.hpp"
#include "Win32ClipboardAdapter.hpp"
#include "Win32ScreenSamplerAdapter.hpp"
#include "Win32SettingsStoreAdapter.hpp"
#include "Win32SystemAppearanceAdapter.hpp"

#include <string_view>
#include <utility>

namespace
{
const wchar_t *reason_of(neneloupe::WindowFailure failure)
{
    switch (failure)
    {
    case neneloupe::WindowFailure::icon_loading_failed:
        return L"Could not load the application icon.";
    case neneloupe::WindowFailure::registration_failed:
        return L"Could not register the window class.";
    case neneloupe::WindowFailure::creation_failed:
        return L"Could not create the window.";
    case neneloupe::WindowFailure::capture_exclusion_failed:
        return L"Could not exclude the loupe from screen capture. "
               L"Windows 10 version 2004 or later is required.";
    case neneloupe::WindowFailure::timer_failed:
        return L"Could not create the refresh timer.";
    }
    std::unreachable();
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR command_line, int)
{
    // 起動引数の解釈は core の 1 か所だけ。ここは受け取って渡すだけにする。
    const std::wstring_view arguments = command_line ? command_line : L"";
    const auto exclusion = neneloupe::capture_exclusion_of(arguments);
    neneloupe::Win32ScreenSamplerAdapter sampler;
    neneloupe::Win32ClipboardAdapter clipboard;
    neneloupe::Win32SettingsStoreAdapter store;
    neneloupe::Win32SystemAppearanceAdapter appearance;
    neneloupe::LoupeController controller(sampler, clipboard, store, appearance);
    auto window = neneloupe::LoupeWindow::create(instance, controller, exclusion);
    if (!window)
    {
        MessageBoxW(nullptr, reason_of(window.error()), L"NeNe Loupe", MB_OK | MB_ICONERROR);
        return 1;
    }
    // OpenClipboard に NULL を渡すと所有者が NULL になり SetClipboardData が落ちる。
    // 実在する窓を合成ルートで結ぶ（ui から adapters への依存は作らない）。
    clipboard.bind((*window)->handle());
    MSG message{};
    while ((*window)->is_open())
    {
        const auto result = GetMessageW(&message, nullptr, 0, 0);
        if (result == -1)
        {
            return 1;
        }
        if (result == 0)
        {
            break;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 0;
}
