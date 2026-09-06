#pragma once

#include "ThemePalette.hpp"

#include <Windows.h>

namespace neneloupe
{
// 配色表から GDI の色への唯一の変換。両方の窓がこれを通す（ARC-001）。
class PaletteBrush final
{
  public:
    explicit PaletteBrush(const ThemePalette &palette);
    COLORREF color(PaletteRole role) const;
    void fill(HDC dc, RECT bounds, PaletteRole role) const;
    void outline(HDC dc, RECT bounds, PaletteRole role) const;
    static void fill_color(HDC dc, RECT bounds, COLORREF color);

  private:
    ThemePalette palette_;
};
} // namespace neneloupe
