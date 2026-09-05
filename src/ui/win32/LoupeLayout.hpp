#pragma once

#include "LoupeHitArea.hpp"

#include <Windows.h>

namespace neneloupe
{
// ルーペ窓の寸法の正本（DIP、96 DPI 基準）。描画もヒット判定もここだけを見る。
// 実座標は MulDiv(値, dpi, 96)。表は docs/design/loupe-look.md 第 2 節と同じ。
class LoupeLayout final
{
  public:
    static constexpr int width = 240;
    static constexpr int height = 64;
    static constexpr int lens_cell = 8;

    static int scale(int value, UINT dpi);
    static RECT scaled(RECT bounds, UINT dpi);
    static POINT lens_center(UINT dpi);

    static RECT window(UINT dpi);
    static RECT lens_frame(UINT dpi);
    static RECT lens_pixels(UINT dpi);
    static RECT marker_outer(UINT dpi);
    static RECT marker_inner(UINT dpi);
    static RECT swatch_frame(UINT dpi);
    static RECT swatch_fill(UINT dpi);
    static RECT chip(UINT dpi);
    static RECT chip_text(UINT dpi);
    static RECT caret(UINT dpi);
    static RECT toast_icon(UINT dpi);
    static RECT toast_text(UINT dpi);
    static RECT gear_glyph(UINT dpi);
    static RECT value_text(UINT dpi);
    static RECT copy_rule(UINT dpi);
    static RECT failure_bar(UINT dpi);
    static RECT failure_text(UINT dpi);

    static RECT hit_chip(UINT dpi);
    static RECT hit_gear(UINT dpi);
    static RECT hit_value(UINT dpi);
    static LoupeHitArea hit_test(POINT point, UINT dpi);
};
} // namespace neneloupe
