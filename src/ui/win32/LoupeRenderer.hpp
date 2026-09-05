#pragma once

#include "LoupeFrame.hpp"
#include "LoupeHitArea.hpp"
#include "PaletteBrush.hpp"

#include <Windows.h>

namespace neneloupe
{
class LoupeRenderer final
{
  public:
    static void render(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover);
    // 記号は書体に頼らず GDI の図形で描く。設定モーダルからも使う。
    static void draw_gear(HDC dc, RECT bounds, COLORREF mark, COLORREF hole);
    static void draw_check(HDC dc, RECT bounds, COLORREF mark, UINT dpi);
    static void draw_cross(HDC dc, RECT bounds, COLORREF mark, UINT dpi);
    static LOGFONTW font_of(const wchar_t *face, int height, int weight, UINT dpi);
    static void write_text(HDC dc, RECT bounds, const std::wstring &text, const LOGFONTW &font);

  private:
    static void render_content(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover);
    static void render_lens(HDC dc, const LoupeFrame &frame, UINT dpi);
    static void render_swatch(HDC dc, const LoupeFrame &frame, UINT dpi);
    static void render_chip(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover);
    static void render_toast(HDC dc, const LoupeFrame &frame, UINT dpi);
    static void render_value(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover);
};
} // namespace neneloupe
