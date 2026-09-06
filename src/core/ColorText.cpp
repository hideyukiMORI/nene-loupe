#include "ColorText.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace neneloupe
{
namespace
{
constexpr int channel_maximum = 255;

int high_channel(const RgbColor &color)
{
    return std::max({color.red(), color.green(), color.blue()});
}

int low_channel(const RgbColor &color)
{
    return std::min({color.red(), color.green(), color.blue()});
}
} // namespace

std::string ColorText::of(const RgbColor &color, ColorFormat format)
{
    switch (format)
    {
    case ColorFormat::rgb_decimal:
        return decimal_text(color);
    case ColorFormat::rgb_hex:
        return hex_text(color);
    case ColorFormat::cmyk:
        return cmyk_text(color);
    case ColorFormat::hsl:
        return hsl_text(color);
    case ColorFormat::hsv:
        return hsv_text(color);
    }
    std::unreachable();
}

std::string ColorText::label(ColorFormat format)
{
    switch (format)
    {
    case ColorFormat::rgb_decimal:
        return "RGB";
    case ColorFormat::rgb_hex:
        return "HEX";
    case ColorFormat::cmyk:
        return "CMYK";
    case ColorFormat::hsl:
        return "HSL";
    case ColorFormat::hsv:
        return "HSV";
    }
    std::unreachable();
}

int ColorText::percent(int numerator, int denominator)
{
    return (numerator * 100 + denominator / 2) / denominator;
}

int ColorText::hue_degrees(const RgbColor &color)
{
    const int red = color.red();
    const int green = color.green();
    const int blue = color.blue();
    const int delta = high_channel(color) - low_channel(color);
    if (delta == 0)
    {
        return 0;
    }
    double sextant = 4.0 + static_cast<double>(red - green) / delta;
    if (high_channel(color) == red)
    {
        sextant = static_cast<double>(green - blue) / delta;
    }
    else if (high_channel(color) == green)
    {
        sextant = 2.0 + static_cast<double>(blue - red) / delta;
    }
    // 丸めの契約: 先に [0,360) へ正規化し、そのうえで四捨五入する。360 は 0 へ畳む。
    // 負の値のまま丸めると赤の手前で丸めが非対称になる（例 RGB(240,0,2) の 359.5 度）。
    double degrees = std::fmod(sextant * 60.0, 360.0);
    if (degrees < 0.0)
    {
        degrees += 360.0;
    }
    return static_cast<int>(std::lround(degrees) % 360);
}

std::string ColorText::decimal_text(const RgbColor &color)
{
    return std::to_string(color.red()) + ", " + std::to_string(color.green()) + ", " +
           std::to_string(color.blue());
}

std::string ColorText::hex_text(const RgbColor &color)
{
    constexpr char digits[] = "0123456789ABCDEF";
    const auto red = color.red();
    const auto green = color.green();
    const auto blue = color.blue();
    return {'#',
            digits[red >> 4],
            digits[red & 15],
            digits[green >> 4],
            digits[green & 15],
            digits[blue >> 4],
            digits[blue & 15]};
}

std::string ColorText::cmyk_text(const RgbColor &color)
{
    // ICC を使わない素朴換算（SPECIFICATION 第 3 節）。
    const int high = high_channel(color);
    if (high == 0)
    {
        return "0, 0, 0, 100";
    }
    const int cyan = percent(high - color.red(), high);
    const int magenta = percent(high - color.green(), high);
    const int yellow = percent(high - color.blue(), high);
    const int black = 100 - percent(high, channel_maximum);
    return std::to_string(cyan) + ", " + std::to_string(magenta) + ", " + std::to_string(yellow) +
           ", " + std::to_string(black);
}

std::string ColorText::hsl_text(const RgbColor &color)
{
    const int high = high_channel(color);
    const int low = low_channel(color);
    const int delta = high - low;
    const int lightness = percent(high + low, 2 * channel_maximum);
    const int span = channel_maximum - std::abs(high + low - channel_maximum);
    const int saturation = (delta == 0) ? 0 : percent(delta, span);
    return std::to_string(hue_degrees(color)) + ", " + std::to_string(saturation) + "%, " +
           std::to_string(lightness) + "%";
}

std::string ColorText::hsv_text(const RgbColor &color)
{
    const int high = high_channel(color);
    const int delta = high - low_channel(color);
    const int saturation = (high == 0) ? 0 : percent(delta, high);
    const int value = percent(high, channel_maximum);
    return std::to_string(hue_degrees(color)) + ", " + std::to_string(saturation) + "%, " +
           std::to_string(value) + "%";
}
} // namespace neneloupe
