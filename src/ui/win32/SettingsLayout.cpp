#include "SettingsLayout.hpp"

#include "LoupeLayout.hpp"

namespace neneloupe
{
namespace
{
constexpr int theme_step = 32;
constexpr int theme_first = 84;

RECT dip(RECT bounds, UINT dpi)
{
    return LoupeLayout::scaled(bounds, dpi);
}
} // namespace

RECT SettingsLayout::window(UINT dpi)
{
    return dip(RECT{0, 0, width, height}, dpi);
}

RECT SettingsLayout::title(UINT dpi)
{
    return dip(RECT{16, 12, 216, 36}, dpi);
}

RECT SettingsLayout::close_glyph(UINT dpi)
{
    return dip(RECT{286, 12, 306, 32}, dpi);
}

RECT SettingsLayout::divider(int top, UINT dpi)
{
    return dip(RECT{0, top, width, top + 1}, dpi);
}

RECT SettingsLayout::theme_label(UINT dpi)
{
    return dip(RECT{16, 60, 216, 76}, dpi);
}

RECT SettingsLayout::theme_radio(int index, UINT dpi)
{
    const int top = theme_first + 4 + index * theme_step;
    return dip(RECT{18, top, 32, top + 14}, dpi);
}

RECT SettingsLayout::theme_text(int index, UINT dpi)
{
    const int top = theme_first + index * theme_step;
    return dip(RECT{42, top, 282, top + 22}, dpi);
}

RECT SettingsLayout::window_label(UINT dpi)
{
    return dip(RECT{16, 192, 216, 208}, dpi);
}

RECT SettingsLayout::topmost_text(UINT dpi)
{
    return dip(RECT{16, 213, 216, 233}, dpi);
}

RECT SettingsLayout::switch_track(UINT dpi)
{
    return dip(RECT{270, 213, 306, 233}, dpi);
}

RECT SettingsLayout::switch_knob(WindowLayer layer, UINT dpi)
{
    const int left = layer == WindowLayer::topmost ? 288 : 272;
    return dip(RECT{left, 215, left + 16, 231}, dpi);
}

RECT SettingsLayout::topmost_help(UINT dpi)
{
    return dip(RECT{16, 239, 304, 257}, dpi);
}

RECT SettingsLayout::about_label(UINT dpi)
{
    return dip(RECT{16, 278, 256, 294}, dpi);
}

RECT SettingsLayout::about_version(UINT dpi)
{
    return dip(RECT{16, 298, 304, 316}, dpi);
}

RECT SettingsLayout::about_copyright(UINT dpi)
{
    return dip(RECT{16, 318, 304, 336}, dpi);
}

RECT SettingsLayout::about_cmyk(UINT dpi)
{
    return dip(RECT{16, 338, 304, 356}, dpi);
}

RECT SettingsLayout::status_text(UINT dpi)
{
    return dip(RECT{16, 364, 304, 382}, dpi);
}

RECT SettingsLayout::hit_close(UINT dpi)
{
    return dip(RECT{280, 6, 314, 38}, dpi);
}

RECT SettingsLayout::hit_theme(int index, UINT dpi)
{
    const int top = 80 + index * theme_step;
    return dip(RECT{8, top, 312, top + 30}, dpi);
}

RECT SettingsLayout::hit_topmost(UINT dpi)
{
    return dip(RECT{8, 207, 312, 239}, dpi);
}

RECT SettingsLayout::drag_band(UINT dpi)
{
    return dip(RECT{0, 0, 280, 48}, dpi);
}

SettingsHitArea SettingsLayout::hit_test(POINT point, UINT dpi)
{
    const RECT close_area = hit_close(dpi);
    if (PtInRect(&close_area, point))
    {
        return SettingsHitArea::close;
    }
    const RECT dark = hit_theme(0, dpi);
    if (PtInRect(&dark, point))
    {
        return SettingsHitArea::theme_dark;
    }
    const RECT light = hit_theme(1, dpi);
    if (PtInRect(&light, point))
    {
        return SettingsHitArea::theme_light;
    }
    const RECT system = hit_theme(2, dpi);
    if (PtInRect(&system, point))
    {
        return SettingsHitArea::theme_system;
    }
    const RECT layer = hit_topmost(dpi);
    if (PtInRect(&layer, point))
    {
        return SettingsHitArea::topmost;
    }
    return SettingsHitArea::none;
}
} // namespace neneloupe
