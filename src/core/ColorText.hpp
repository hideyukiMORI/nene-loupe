#pragma once

#include "ColorFormat.hpp"
#include "RgbColor.hpp"

#include <string>

namespace neneloupe
{
class ColorText final
{
  public:
    // 表示とクリップボードは同じ文字列を使う（FR-006 / FR-007）。
    static std::string of(const RgbColor &color, ColorFormat format);
    static std::string label(ColorFormat format);

  private:
    static std::string decimal_text(const RgbColor &color);
    static std::string hex_text(const RgbColor &color);
    static std::string cmyk_text(const RgbColor &color);
    static std::string hsl_text(const RgbColor &color);
    static std::string hsv_text(const RgbColor &color);
    static int hue_degrees(const RgbColor &color);
    static int percent(int numerator, int denominator);
};
} // namespace neneloupe
