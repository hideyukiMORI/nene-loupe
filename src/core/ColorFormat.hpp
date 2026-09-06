#pragma once

namespace neneloupe
{
enum class ColorFormat
{
    rgb_decimal,
    rgb_hex,
    cmyk,
    hsl,
    hsv
};

ColorFormat next_format(ColorFormat format);
} // namespace neneloupe
