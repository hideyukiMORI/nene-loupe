#pragma once

#include "SettingsHitArea.hpp"
#include "WindowLayer.hpp"

#include <Windows.h>

namespace neneloupe
{
// 設定モーダルの寸法の正本（DIP、96 DPI 基準）。docs/design/loupe-look.md 第 9 節と同じ。
class SettingsLayout final
{
  public:
    static constexpr int width = 320;
    static constexpr int height = 392;
    static constexpr int theme_choices = 3;

    static RECT window(UINT dpi);
    static RECT title(UINT dpi);
    static RECT close_glyph(UINT dpi);
    static RECT divider(int top, UINT dpi);
    static RECT theme_label(UINT dpi);
    static RECT theme_radio(int index, UINT dpi);
    static RECT theme_text(int index, UINT dpi);
    static RECT window_label(UINT dpi);
    static RECT topmost_text(UINT dpi);
    static RECT switch_track(UINT dpi);
    static RECT switch_knob(WindowLayer layer, UINT dpi);
    static RECT topmost_help(UINT dpi);
    static RECT about_label(UINT dpi);
    static RECT about_version(UINT dpi);
    static RECT about_copyright(UINT dpi);
    static RECT about_cmyk(UINT dpi);
    static RECT status_text(UINT dpi);

    static RECT hit_close(UINT dpi);
    static RECT hit_theme(int index, UINT dpi);
    static RECT hit_topmost(UINT dpi);
    static RECT drag_band(UINT dpi);
    static SettingsHitArea hit_test(POINT point, UINT dpi);
};
} // namespace neneloupe
