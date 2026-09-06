#include "PaletteBrush.hpp"

namespace neneloupe
{
PaletteBrush::PaletteBrush(const ThemePalette &palette) : palette_(palette) {}

COLORREF PaletteBrush::color(PaletteRole role) const
{
    const auto value = palette_.color(role);
    return RGB(value.red(), value.green(), value.blue());
}

void PaletteBrush::fill_color(HDC dc, RECT bounds, COLORREF value)
{
    SetDCBrushColor(dc, value);
    FillRect(dc, &bounds, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

void PaletteBrush::fill(HDC dc, RECT bounds, PaletteRole role) const
{
    fill_color(dc, bounds, color(role));
}

void PaletteBrush::outline(HDC dc, RECT bounds, PaletteRole role) const
{
    SetDCBrushColor(dc, color(role));
    FrameRect(dc, &bounds, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}
} // namespace neneloupe
