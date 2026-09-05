#include "ColorFormat.hpp"

#include <utility>

namespace neneloupe
{
ColorFormat next_format(ColorFormat format)
{
    switch (format)
    {
    case ColorFormat::rgb_decimal:
        return ColorFormat::rgb_hex;
    case ColorFormat::rgb_hex:
        return ColorFormat::cmyk;
    case ColorFormat::cmyk:
        return ColorFormat::hsl;
    case ColorFormat::hsl:
        return ColorFormat::hsv;
    case ColorFormat::hsv:
        return ColorFormat::rgb_decimal;
    }
    std::unreachable();
}
} // namespace neneloupe
