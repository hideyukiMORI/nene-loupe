#include "LoupeSettings.hpp"

namespace neneloupe
{
LoupeSettings::LoupeSettings(Theme theme, ColorFormat format, WindowLayer layer)
    : theme_(theme), format_(format), layer_(layer)
{
}

LoupeSettings LoupeSettings::defaults()
{
    // 既定は現状互換。最前面は入（ADR 0006）。
    return LoupeSettings(Theme::system, ColorFormat::rgb_hex, WindowLayer::topmost);
}

LoupeSettings LoupeSettings::of(Theme theme, ColorFormat format, WindowLayer layer)
{
    return LoupeSettings(theme, format, layer);
}

Theme LoupeSettings::theme() const noexcept
{
    return theme_;
}

ColorFormat LoupeSettings::format() const noexcept
{
    return format_;
}

WindowLayer LoupeSettings::layer() const noexcept
{
    return layer_;
}

LoupeSettings LoupeSettings::with_theme(Theme theme) const
{
    return LoupeSettings(theme, format_, layer_);
}

LoupeSettings LoupeSettings::with_format(ColorFormat format) const
{
    return LoupeSettings(theme_, format, layer_);
}

LoupeSettings LoupeSettings::with_layer(WindowLayer layer) const
{
    return LoupeSettings(theme_, format_, layer);
}
} // namespace neneloupe
