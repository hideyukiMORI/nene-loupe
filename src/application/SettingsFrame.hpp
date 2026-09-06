#pragma once

#include "LoupeSettings.hpp"
#include "SettingsStatus.hpp"
#include "ThemeAppearance.hpp"
#include "ThemePalette.hpp"

#include <string>

namespace neneloupe
{
class SettingsFrame final
{
  public:
    static SettingsFrame of(const LoupeSettings &settings, ThemeAppearance appearance,
                            SettingsStatus status);
    Theme theme() const noexcept;
    WindowLayer layer() const noexcept;
    SettingsStatus status() const noexcept;
    const std::wstring &status_text() const noexcept;
    ThemePalette palette() const;

  private:
    SettingsFrame(Theme theme, WindowLayer layer, ThemeAppearance appearance,
                  SettingsStatus status);
    Theme theme_;
    WindowLayer layer_;
    ThemeAppearance appearance_;
    SettingsStatus status_;
    std::wstring status_text_;
};
} // namespace neneloupe
