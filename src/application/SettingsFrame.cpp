#include "SettingsFrame.hpp"

#include <utility>

namespace neneloupe
{
namespace
{
std::wstring status_caption(SettingsStatus status)
{
    switch (status)
    {
    case SettingsStatus::ok:
        return std::wstring();
    case SettingsStatus::load_failed:
        return L"設定を読み込めませんでした。既定値で動いています。";
    case SettingsStatus::save_failed:
        return L"設定を保存できませんでした。";
    }
    std::unreachable();
}
} // namespace

SettingsFrame::SettingsFrame(Theme theme, WindowLayer layer, ThemeAppearance appearance,
                             SettingsStatus status)
    : theme_(theme), layer_(layer), appearance_(appearance), status_(status),
      status_text_(status_caption(status))
{
}

SettingsFrame SettingsFrame::of(const LoupeSettings &settings, ThemeAppearance appearance,
                                SettingsStatus status)
{
    return SettingsFrame(settings.theme(), settings.layer(), appearance, status);
}

Theme SettingsFrame::theme() const noexcept
{
    return theme_;
}

WindowLayer SettingsFrame::layer() const noexcept
{
    return layer_;
}

SettingsStatus SettingsFrame::status() const noexcept
{
    return status_;
}

const std::wstring &SettingsFrame::status_text() const noexcept
{
    return status_text_;
}

ThemePalette SettingsFrame::palette() const
{
    return ThemePalette::of(appearance_);
}
} // namespace neneloupe
