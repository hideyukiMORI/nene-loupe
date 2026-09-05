#pragma once

#include "ColorFormat.hpp"
#include "CopyState.hpp"
#include "LoupeSettings.hpp"
#include "SamplingFailure.hpp"
#include "ScreenSample.hpp"
#include "ThemeAppearance.hpp"
#include "ThemePalette.hpp"

#include <expected>
#include <optional>
#include <string>

namespace neneloupe
{
class LoupeFrame final
{
  public:
    static LoupeFrame of(const std::expected<ScreenSample, SamplingFailure> &sample,
                         const LoupeSettings &settings, ThemeAppearance appearance, CopyState copy);
    const std::optional<ScreenSample> &sample() const noexcept;
    const std::wstring &caption() const noexcept;
    const std::wstring &format_label() const noexcept;
    ColorFormat format() const noexcept;
    CopyState copy() const noexcept;
    ThemePalette palette() const;
    bool has_color() const noexcept;

  private:
    LoupeFrame(std::optional<ScreenSample> sample, std::wstring caption,
               const LoupeSettings &settings, ThemeAppearance appearance);
    std::optional<ScreenSample> sample_;
    std::wstring caption_;
    std::wstring format_label_;
    ColorFormat format_;
    ThemeAppearance appearance_;
    CopyState copy_ = CopyState::idle;
};
} // namespace neneloupe
