#pragma once

#include "PaletteRole.hpp"
#include "RgbColor.hpp"
#include "ThemeAppearance.hpp"

#include <array>
#include <cstddef>

namespace neneloupe
{
class ThemePalette final
{
  public:
    static constexpr std::size_t role_count = 10;
    static ThemePalette of(ThemeAppearance appearance);
    RgbColor color(PaletteRole role) const noexcept;

  private:
    explicit ThemePalette(const std::array<RgbColor, role_count> &colors);
    std::array<RgbColor, role_count> colors_;
};
} // namespace neneloupe
