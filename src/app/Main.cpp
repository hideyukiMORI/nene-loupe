#include "LoupeWindow.hpp"
#include "Win32ScreenSamplerAdapter.hpp"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    neneloupe::Win32ScreenSamplerAdapter sampler;
    neneloupe::LoupeController controller(sampler);
    auto window = neneloupe::LoupeWindow::create(instance, controller);
    if (!window)
    {
        MessageBoxW(nullptr, L"ルーペのウィンドウを作成できませんでした。", L"NeNe Loupe",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
    MSG message{};
    while ((*window)->is_open())
    {
        const auto result = GetMessageW(&message, nullptr, 0, 0);
        if (result == -1)
            return 1;
        if (result == 0)
            break;
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 0;
}
