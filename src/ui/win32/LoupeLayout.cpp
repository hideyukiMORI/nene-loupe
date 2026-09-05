#include "LoupeLayout.hpp"

namespace neneloupe
{
int LoupeLayout::scale(int value, UINT dpi)
{
    return MulDiv(value, static_cast<int>(dpi), 96);
}

RECT LoupeLayout::scaled(RECT bounds, UINT dpi)
{
    return RECT{scale(bounds.left, dpi), scale(bounds.top, dpi), scale(bounds.right, dpi),
                scale(bounds.bottom, dpi)};
}

POINT LoupeLayout::lens_center(UINT dpi)
{
    // 4 + 7 * 8 / 2 = 32。ADR 0004 / 0005 の採取契約はここで固定されている。
    const auto center = scale(4 + 7 * lens_cell / 2, dpi);
    return POINT{center, center};
}

RECT LoupeLayout::window(UINT dpi)
{
    return scaled(RECT{0, 0, width, height}, dpi);
}

RECT LoupeLayout::lens_frame(UINT dpi)
{
    return scaled(RECT{3, 3, 61, 61}, dpi);
}

RECT LoupeLayout::lens_pixels(UINT dpi)
{
    return scaled(RECT{4, 4, 60, 60}, dpi);
}

RECT LoupeLayout::marker_outer(UINT dpi)
{
    return scaled(RECT{27, 27, 37, 37}, dpi);
}

RECT LoupeLayout::marker_inner(UINT dpi)
{
    return scaled(RECT{28, 28, 36, 36}, dpi);
}

RECT LoupeLayout::swatch_frame(UINT dpi)
{
    return scaled(RECT{65, 3, 81, 61}, dpi);
}

RECT LoupeLayout::swatch_fill(UINT dpi)
{
    return scaled(RECT{66, 4, 80, 60}, dpi);
}

RECT LoupeLayout::chip(UINT dpi)
{
    return scaled(RECT{88, 6, 148, 26}, dpi);
}

RECT LoupeLayout::chip_text(UINT dpi)
{
    return scaled(RECT{95, 7, 137, 25}, dpi);
}

RECT LoupeLayout::caret(UINT dpi)
{
    return scaled(RECT{135, 14, 141, 18}, dpi);
}

RECT LoupeLayout::toast_icon(UINT dpi)
{
    return scaled(RECT{154, 10, 166, 22}, dpi);
}

RECT LoupeLayout::toast_text(UINT dpi)
{
    return scaled(RECT{169, 7, 208, 25}, dpi);
}

RECT LoupeLayout::gear_glyph(UINT dpi)
{
    return scaled(RECT{218, 8, 234, 24}, dpi);
}

RECT LoupeLayout::value_text(UINT dpi)
{
    return scaled(RECT{88, 31, 236, 58}, dpi);
}

RECT LoupeLayout::copy_rule(UINT dpi)
{
    return scaled(RECT{88, 59, 236, 61}, dpi);
}

RECT LoupeLayout::failure_bar(UINT dpi)
{
    return scaled(RECT{88, 33, 90, 55}, dpi);
}

RECT LoupeLayout::failure_text(UINT dpi)
{
    return scaled(RECT{98, 31, 236, 58}, dpi);
}

RECT LoupeLayout::hit_chip(UINT dpi)
{
    return scaled(RECT{86, 3, 150, 27}, dpi);
}

RECT LoupeLayout::hit_gear(UINT dpi)
{
    return scaled(RECT{211, 3, 238, 27}, dpi);
}

RECT LoupeLayout::hit_value(UINT dpi)
{
    return scaled(RECT{86, 29, 238, 61}, dpi);
}

LoupeHitArea LoupeLayout::hit_test(POINT point, UINT dpi)
{
    const RECT chip_area = hit_chip(dpi);
    if (PtInRect(&chip_area, point))
    {
        return LoupeHitArea::format_chip;
    }
    const RECT gear_area = hit_gear(dpi);
    if (PtInRect(&gear_area, point))
    {
        return LoupeHitArea::gear;
    }
    const RECT value_area = hit_value(dpi);
    if (PtInRect(&value_area, point))
    {
        return LoupeHitArea::value;
    }
    return LoupeHitArea::none;
}
} // namespace neneloupe
