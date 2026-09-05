#include "ThemePalette.hpp"

#include <utility>

namespace neneloupe
{
namespace
{
// 役割から表の位置への対応。default を書かないので、役割を足すと /we4062 と
// covered-switch-default でここが落ちる。末尾の位置には依存しない。
std::size_t index_of(PaletteRole role)
{
    switch (role)
    {
    case PaletteRole::background:
        return 0;
    case PaletteRole::border:
        return 1;
    case PaletteRole::surface:
        return 2;
    case PaletteRole::surface_hover:
        return 3;
    case PaletteRole::line:
        return 4;
    case PaletteRole::text:
        return 5;
    case PaletteRole::muted_text:
        return 6;
    case PaletteRole::accent:
        return 7;
    case PaletteRole::on_accent:
        return 8;
    case PaletteRole::warning:
        return 9;
    }
    std::unreachable();
}

using Table = std::array<RgbColor, ThemePalette::role_count>;

Table dark_table()
{
    return Table{RgbColor::from_channels(24, 26, 30),    RgbColor::from_channels(14, 16, 19),
                 RgbColor::from_channels(34, 37, 43),    RgbColor::from_channels(44, 48, 55),
                 RgbColor::from_channels(51, 55, 63),    RgbColor::from_channels(245, 246, 250),
                 RgbColor::from_channels(150, 156, 169), RgbColor::from_channels(76, 194, 255),
                 RgbColor::from_channels(6, 18, 27),     RgbColor::from_channels(221, 162, 75)};
}

Table light_table()
{
    return Table{RgbColor::from_channels(243, 244, 247), RgbColor::from_channels(185, 190, 200),
                 RgbColor::from_channels(255, 255, 255), RgbColor::from_channels(232, 234, 239),
                 RgbColor::from_channels(211, 215, 223), RgbColor::from_channels(20, 22, 26),
                 RgbColor::from_channels(92, 98, 112),   RgbColor::from_channels(0, 103, 192),
                 RgbColor::from_channels(255, 255, 255), RgbColor::from_channels(138, 90, 18)};
}
} // namespace

ThemePalette::ThemePalette(const std::array<RgbColor, role_count> &colors) : colors_(colors) {}

ThemePalette ThemePalette::of(ThemeAppearance appearance)
{
    switch (appearance)
    {
    case ThemeAppearance::dark:
        return ThemePalette(dark_table());
    case ThemeAppearance::light:
        return ThemePalette(light_table());
    }
    std::unreachable();
}

RgbColor ThemePalette::color(PaletteRole role) const noexcept
{
    return colors_[index_of(role)];
}
} // namespace neneloupe
