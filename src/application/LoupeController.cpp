#include "LoupeController.hpp"

#include <utility>

namespace neneloupe
{
namespace
{
ThemeAppearance resolve(Theme theme, SystemAppearancePort &port)
{
    switch (theme)
    {
    case Theme::dark:
        return ThemeAppearance::dark;
    case Theme::light:
        return ThemeAppearance::light;
    case Theme::system:
        return port.current();
    }
    std::unreachable();
}
} // namespace

LoupeController::LoupeController(ScreenSamplerPort &sampler, ClipboardPort &clipboard,
                                 SettingsStorePort &store, SystemAppearancePort &appearance)
    : sampler_(sampler), clipboard_(clipboard), store_(store), system_appearance_(appearance),
      sample_(std::unexpected(SamplingFailure::position_unavailable)),
      settings_(LoupeSettings::defaults()), appearance_(ThemeAppearance::light),
      status_(SettingsStatus::ok), copy_(CopyState::idle)
{
    const auto stored = store_.load();
    if (stored)
    {
        settings_ = *stored;
    }
    else
    {
        status_ = SettingsStatus::load_failed;
    }
    refresh_appearance();
}

void LoupeController::refresh(const std::expected<ScreenPosition, SamplingFailure> &position)
{
    if (!position)
    {
        sample_ = std::unexpected(position.error());
        return;
    }
    sample_ = sampler_.sample(*position);
}

void LoupeController::refresh_appearance()
{
    appearance_ = resolve(settings_.theme(), system_appearance_);
}

void LoupeController::apply(const LoupeSettings &settings)
{
    settings_ = settings;
    refresh_appearance();
    const auto saved = store_.save(settings_);
    status_ = saved ? SettingsStatus::ok : SettingsStatus::save_failed;
}

void LoupeController::copy_current_value()
{
    const auto current = frame();
    if (!current.has_color())
    {
        copy_ = CopyState::failed;
        return;
    }
    const auto written = clipboard_.write(current.caption());
    copy_ = written ? CopyState::copied : CopyState::failed;
}

void LoupeController::clear_copy_state()
{
    copy_ = CopyState::idle;
}

void LoupeController::cycle_format()
{
    select_format(next_format(settings_.format()));
}

void LoupeController::select_format(ColorFormat format)
{
    apply(settings_.with_format(format));
}

void LoupeController::select_theme(Theme theme)
{
    apply(settings_.with_theme(theme));
}

void LoupeController::select_layer(WindowLayer layer)
{
    apply(settings_.with_layer(layer));
}

LoupeFrame LoupeController::frame() const
{
    return LoupeFrame::of(sample_, settings_, appearance_, copy_);
}

SettingsFrame LoupeController::settings_frame() const
{
    return SettingsFrame::of(settings_, appearance_, status_);
}

ColorFormat LoupeController::format() const noexcept
{
    return settings_.format();
}

WindowLayer LoupeController::layer() const noexcept
{
    return settings_.layer();
}
} // namespace neneloupe
