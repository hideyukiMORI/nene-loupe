#pragma once

#include "ClipboardPort.hpp"
#include "LoupeFrame.hpp"
#include "ScreenSamplerPort.hpp"
#include "SettingsFrame.hpp"
#include "SettingsStorePort.hpp"
#include "SystemAppearancePort.hpp"

namespace neneloupe
{
// 最新のサンプル・設定・コピー状態・設定の状態を 1 つずつ持つ唯一の場所（ARC-004）。
class LoupeController final
{
  public:
    LoupeController(ScreenSamplerPort &sampler, ClipboardPort &clipboard, SettingsStorePort &store,
                    SystemAppearancePort &appearance);
    LoupeController(const LoupeController &) = delete;
    LoupeController &operator=(const LoupeController &) = delete;
    void refresh(const std::expected<ScreenPosition, SamplingFailure> &position);
    void copy_current_value();
    void clear_copy_state();
    void cycle_format();
    void select_format(ColorFormat format);
    void select_theme(Theme theme);
    void select_layer(WindowLayer layer);
    void refresh_appearance();
    LoupeFrame frame() const;
    SettingsFrame settings_frame() const;
    ColorFormat format() const noexcept;
    WindowLayer layer() const noexcept;

  private:
    void apply(const LoupeSettings &settings);
    ScreenSamplerPort &sampler_;
    ClipboardPort &clipboard_;
    SettingsStorePort &store_;
    SystemAppearancePort &system_appearance_;
    std::expected<ScreenSample, SamplingFailure> sample_;
    LoupeSettings settings_;
    ThemeAppearance appearance_;
    SettingsStatus status_;
    CopyState copy_;
};
} // namespace neneloupe
