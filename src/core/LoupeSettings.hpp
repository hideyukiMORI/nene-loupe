#pragma once

#include "ColorFormat.hpp"
#include "Theme.hpp"
#include "WindowLayer.hpp"

namespace neneloupe
{
class LoupeSettings final
{
  public:
    static LoupeSettings defaults();
    static LoupeSettings of(Theme theme, ColorFormat format, WindowLayer layer);
    Theme theme() const noexcept;
    ColorFormat format() const noexcept;
    WindowLayer layer() const noexcept;
    LoupeSettings with_theme(Theme theme) const;
    LoupeSettings with_format(ColorFormat format) const;
    LoupeSettings with_layer(WindowLayer layer) const;
    bool operator==(const LoupeSettings &) const = default;

  private:
    LoupeSettings(Theme theme, ColorFormat format, WindowLayer layer);
    Theme theme_;
    ColorFormat format_;
    WindowLayer layer_;
};
} // namespace neneloupe
