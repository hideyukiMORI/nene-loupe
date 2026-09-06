#include "WindowLayerStyle.hpp"

#include <utility>

namespace neneloupe
{
DWORD WindowLayerStyle::extended(WindowLayer layer)
{
    switch (layer)
    {
    case WindowLayer::topmost:
        return WS_EX_TOPMOST;
    case WindowLayer::normal:
        return 0;
    }
    std::unreachable();
}

HWND WindowLayerStyle::insert_after(WindowLayer layer)
{
    switch (layer)
    {
    case WindowLayer::topmost:
        return HWND_TOPMOST;
    case WindowLayer::normal:
        return HWND_NOTOPMOST;
    }
    std::unreachable();
}

void WindowLayerStyle::apply(HWND owner, HWND modal, WindowLayer layer)
{
    const HWND after = insert_after(layer);
    const UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
    SetWindowPos(owner, after, 0, 0, 0, 0, flags);
    if (modal)
    {
        SetWindowPos(modal, after, 0, 0, 0, 0, flags);
    }
}
} // namespace neneloupe
