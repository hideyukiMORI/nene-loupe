#include "CaptureExclusion.hpp"
#include "ColorText.hpp"
#include "LoupeController.hpp"
#include "PaletteRole.hpp"
#include "RgbColor.hpp"
#include "ScreenSample.hpp"
#include "ThemePalette.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
void require(bool condition, const char *description)
{
    if (!condition)
    {
        std::fprintf(stderr, "FAIL: %s\n", description);
        std::exit(1);
    }
}

neneloupe::ScreenSample sample_with(neneloupe::RgbColor color)
{
    const std::vector pixels(49, color);
    auto sample = neneloupe::ScreenSample::create(pixels);
    require(sample.has_value(), "valid sample construction");
    return *sample;
}

class SequenceSampler final : public neneloupe::ScreenSamplerPort
{
  public:
    explicit SequenceSampler(
        std::vector<std::expected<neneloupe::ScreenSample, neneloupe::SamplingFailure>> sequence)
        : sequence_(std::move(sequence))
    {
    }

    std::expected<neneloupe::ScreenSample, neneloupe::SamplingFailure>
    sample(neneloupe::ScreenPosition position) override
    {
        positions_.push_back(position);
        return sequence_.at(position_++);
    }

    std::size_t calls() const
    {
        return position_;
    }

    const std::vector<neneloupe::ScreenPosition> &positions() const
    {
        return positions_;
    }

  private:
    std::vector<neneloupe::ScreenPosition> positions_;
    std::vector<std::expected<neneloupe::ScreenSample, neneloupe::SamplingFailure>> sequence_;
    std::size_t position_ = 0;
};

class RecordingClipboard final : public neneloupe::ClipboardPort
{
  public:
    std::expected<void, neneloupe::ClipboardFailure> write(const std::wstring &text) override
    {
        ++calls_;
        if (broken_)
        {
            return std::unexpected(neneloupe::ClipboardFailure::unavailable);
        }
        written_ = text;
        return {};
    }

    void breaks()
    {
        broken_ = true;
    }

    const std::wstring &written() const
    {
        return written_;
    }

    int calls() const
    {
        return calls_;
    }

  private:
    std::wstring written_;
    int calls_ = 0;
    bool broken_ = false;
};

class MemorySettingsStore final : public neneloupe::SettingsStorePort
{
  public:
    std::expected<neneloupe::LoupeSettings, neneloupe::SettingsFailure> load() override
    {
        ++loads_;
        if (unreadable_)
        {
            return std::unexpected(neneloupe::SettingsFailure::unreadable);
        }
        return stored_;
    }

    std::expected<void, neneloupe::SettingsFailure>
    save(const neneloupe::LoupeSettings &settings) override
    {
        ++saves_;
        if (unwritable_)
        {
            return std::unexpected(neneloupe::SettingsFailure::unwritable);
        }
        stored_ = settings;
        return {};
    }

    void cannot_read()
    {
        unreadable_ = true;
    }

    void cannot_write()
    {
        unwritable_ = true;
    }

    void can_write()
    {
        unwritable_ = false;
    }

    const neneloupe::LoupeSettings &stored() const
    {
        return stored_;
    }

    int saves() const
    {
        return saves_;
    }

    int loads() const
    {
        return loads_;
    }

  private:
    neneloupe::LoupeSettings stored_ = neneloupe::LoupeSettings::defaults();
    int loads_ = 0;
    int saves_ = 0;
    bool unreadable_ = false;
    bool unwritable_ = false;
};

class FixedAppearance final : public neneloupe::SystemAppearancePort
{
  public:
    neneloupe::ThemeAppearance current() override
    {
        ++calls_;
        return value_;
    }

    void becomes(neneloupe::ThemeAppearance value)
    {
        value_ = value;
    }

    int calls() const
    {
        return calls_;
    }

  private:
    neneloupe::ThemeAppearance value_ = neneloupe::ThemeAppearance::dark;
    int calls_ = 0;
};

void verify_colors()
{
    using neneloupe::ColorFormat;
    using neneloupe::ColorText;
    using neneloupe::RgbColor;
    const auto orange = RgbColor::from_channels(255, 128, 0);
    require(ColorText::of(orange, ColorFormat::rgb_hex) == "#FF8000", "hex mixed");
    require(ColorText::of(RgbColor::from_channels(0, 0, 0), ColorFormat::rgb_hex) == "#000000",
            "hex black");
    require(ColorText::of(RgbColor::from_channels(255, 255, 255), ColorFormat::rgb_hex) ==
                "#FFFFFF",
            "hex white");
    require(ColorText::of(RgbColor::from_channels(0, 1, 15), ColorFormat::rgb_hex) == "#00010F",
            "hex leading zeros");
    require(ColorText::of(orange, ColorFormat::rgb_decimal) == "255, 128, 0", "decimal separator");
    require(ColorText::of(orange, ColorFormat::cmyk) == "0, 50, 100, 0", "cmyk mixed");
    require(ColorText::of(RgbColor::from_channels(0, 0, 0), ColorFormat::cmyk) == "0, 0, 0, 100",
            "cmyk black is the only special case");
    require(ColorText::of(orange, ColorFormat::hsl) == "30, 100%, 50%", "hsl mixed");
    require(ColorText::of(orange, ColorFormat::hsv) == "30, 100%, 100%", "hsv mixed");
    require(ColorText::of(RgbColor::from_channels(0, 0, 0), ColorFormat::hsv) == "0, 0%, 0%",
            "hsv black");
    require(ColorText::of(RgbColor::from_channels(128, 128, 128), ColorFormat::hsl) == "0, 0%, 50%",
            "grey has no hue and no saturation");
}

void verify_hue_rounding()
{
    using neneloupe::ColorFormat;
    using neneloupe::ColorText;
    using neneloupe::RgbColor;
    // 角度を [0,360) へ正規化してから四捨五入する（ADR 0006 第 2 節）。
    // 負のまま丸めると 359.5 度が 359 になり、赤の手前で丸めが非対称になる。
    require(ColorText::of(RgbColor::from_channels(240, 0, 2), ColorFormat::hsl) == "0, 100%, 47%",
            "359.5 degrees folds to 0");
    require(ColorText::of(RgbColor::from_channels(240, 0, 2), ColorFormat::hsv) == "0, 100%, 94%",
            "359.5 degrees folds to 0 in hsv");
    require(ColorText::of(RgbColor::from_channels(136, 0, 17), ColorFormat::hsl) ==
                "353, 100%, 27%",
            "352.5 degrees rounds up to 353");
    // 三つの成分それぞれが最大になる経路を通す。
    require(ColorText::of(RgbColor::from_channels(0, 255, 0), ColorFormat::hsl) == "120, 100%, 50%",
            "green hue");
    require(ColorText::of(RgbColor::from_channels(0, 0, 255), ColorFormat::hsl) == "240, 100%, 50%",
            "blue hue");
}

void verify_capture_exclusion_argument()
{
    using neneloupe::capture_exclusion_of;
    using neneloupe::CaptureExclusion;
    require(capture_exclusion_of(L"") == CaptureExclusion::enabled, "no argument excludes");
    require(capture_exclusion_of(L"   ") == CaptureExclusion::enabled, "blank argument excludes");
    require(capture_exclusion_of(L"--allow-screen-capture") == CaptureExclusion::disabled,
            "diagnostic argument alone");
    require(capture_exclusion_of(L"  --allow-screen-capture  ") == CaptureExclusion::disabled,
            "diagnostic argument with padding");
    require(capture_exclusion_of(L"--other --allow-screen-capture") == CaptureExclusion::disabled,
            "diagnostic argument after another");
    require(capture_exclusion_of(L"--allow-screen-capture	--other") ==
                CaptureExclusion::disabled,
            "tab separated arguments");
    require(capture_exclusion_of(L"--allow-screen-captured") == CaptureExclusion::enabled,
            "longer word is not the argument");
    require(capture_exclusion_of(L"allow-screen-capture") == CaptureExclusion::enabled,
            "shorter word is not the argument");
}

void verify_format_cycle()
{
    using neneloupe::ColorFormat;
    using neneloupe::ColorText;
    require(neneloupe::next_format(ColorFormat::rgb_hex) == ColorFormat::cmyk, "hex then cmyk");
    require(neneloupe::next_format(ColorFormat::cmyk) == ColorFormat::hsl, "cmyk then hsl");
    require(neneloupe::next_format(ColorFormat::hsl) == ColorFormat::hsv, "hsl then hsv");
    require(neneloupe::next_format(ColorFormat::hsv) == ColorFormat::rgb_decimal, "hsv then rgb");
    require(neneloupe::next_format(ColorFormat::rgb_decimal) == ColorFormat::rgb_hex,
            "rgb then hex");
    require(ColorText::label(ColorFormat::rgb_decimal) == "RGB", "rgb label");
    require(ColorText::label(ColorFormat::rgb_hex) == "HEX", "hex label");
    require(ColorText::label(ColorFormat::cmyk) == "CMYK", "cmyk label");
    require(ColorText::label(ColorFormat::hsl) == "HSL", "hsl label");
    require(ColorText::label(ColorFormat::hsv) == "HSV", "hsv label");
}

void verify_palette()
{
    using neneloupe::PaletteRole;
    using neneloupe::ThemeAppearance;
    using neneloupe::ThemePalette;
    static constexpr std::array<PaletteRole, ThemePalette::role_count> roles{
        PaletteRole::background,    PaletteRole::border, PaletteRole::surface,
        PaletteRole::surface_hover, PaletteRole::line,   PaletteRole::text,
        PaletteRole::muted_text,    PaletteRole::accent, PaletteRole::on_accent,
        PaletteRole::warning};
    const auto dark = ThemePalette::of(ThemeAppearance::dark);
    const auto light = ThemePalette::of(ThemeAppearance::light);
    for (const auto role : roles)
    {
        require(!(dark.color(role) == light.color(role)) || role == PaletteRole::on_accent,
                "every role differs between the two appearances");
    }
    require(dark.color(PaletteRole::background) == neneloupe::RgbColor::from_channels(24, 26, 30),
            "dark background keeps the shipped colour");
    require(light.color(PaletteRole::text) == neneloupe::RgbColor::from_channels(20, 22, 26),
            "light text");
}

void verify_settings_value()
{
    using namespace neneloupe;
    const auto defaults = LoupeSettings::defaults();
    require(defaults.theme() == Theme::system, "default theme follows the system");
    require(defaults.format() == ColorFormat::rgb_hex, "default format is hex");
    require(defaults.layer() == WindowLayer::topmost, "default layer is topmost");
    // 反転の意図は core にひとつ。設定モーダルのトグルはこれを呼ぶ。
    require(next_layer(WindowLayer::topmost) == WindowLayer::normal, "topmost turns into normal");
    require(next_layer(WindowLayer::normal) == WindowLayer::topmost, "normal turns back");
    require(next_layer(next_layer(WindowLayer::topmost)) == WindowLayer::topmost,
            "two toggles return to the start");
    const auto changed = defaults.with_theme(Theme::dark)
                             .with_format(ColorFormat::cmyk)
                             .with_layer(WindowLayer::normal);
    require(changed.theme() == Theme::dark, "theme replaced");
    require(changed.format() == ColorFormat::cmyk, "format replaced");
    require(changed.layer() == WindowLayer::normal, "layer replaced");
    require(defaults.theme() == Theme::system, "the original value is untouched");
    require(LoupeSettings::of(Theme::light, ColorFormat::hsl, WindowLayer::normal) ==
                defaults.with_theme(Theme::light)
                    .with_format(ColorFormat::hsl)
                    .with_layer(WindowLayer::normal),
            "equal settings compare equal");
}

void verify_sample_ownership()
{
    using namespace neneloupe;
    const auto orange = RgbColor::from_channels(255, 128, 0);
    std::vector pixels(49, RgbColor::from_channels(0, 0, 0));
    pixels[24] = orange;
    const auto sample = ScreenSample::create(pixels);
    require(sample.has_value(), "7 by 7 grid");
    pixels[24] = RgbColor::from_channels(255, 255, 255);
    require(sample->center() == orange, "sample owns an independent copy");
    require(sample->pixels().size() == 49, "all pixels preserved");
    require(sample->pixels()[0] == RgbColor::from_channels(0, 0, 0), "top-left pixel");
    static_assert(std::is_same_v<decltype(sample->pixels()[0]), const RgbColor &>);
}

void verify_sample_rejection()
{
    using namespace neneloupe;
    for (const auto count : {0, 48, 50})
    {
        const std::vector pixels(count, RgbColor::from_channels(0, 0, 0));
        const auto result = ScreenSample::create(pixels);
        require(!result, "wrong sample size rejected");
        require(result.error() == SampleRejection::wrong_pixel_count, "typed size rejection");
    }
}

void verify_updates()
{
    using namespace neneloupe;
    SequenceSampler sampler({sample_with(RgbColor::from_channels(255, 128, 0)),
                             std::unexpected(SamplingFailure::capture_unavailable),
                             std::unexpected(SamplingFailure::position_unavailable),
                             sample_with(RgbColor::from_channels(0, 1, 15))});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    require(sampler.calls() == 0, "construction does not sample an unknown position");
    const auto point = ScreenPosition::from_physical_pixels(-1200, -300);
    require(point.x() == -1200 && point.y() == -300, "negative physical coordinates");
    controller.refresh(point);
    const auto original = controller.frame();
    require(original.caption() == L"#FF8000", "initial sample formatted by application");
    require(original.sample().has_value(), "initial pixels available");
    require(original.format_label() == L"HEX", "format label follows the settings");
    controller.refresh(point);
    require(!controller.frame().sample(), "capture failure removes stale pixels");
    require(controller.frame().caption() == L"画面取得不可", "capture failure caption");
    controller.refresh(point);
    require(controller.frame().caption() == L"位置取得不可", "position failure caption");
    controller.refresh(point);
    require(controller.frame().caption() == L"#00010F", "recovery uses fresh data");
    require(original.caption() == L"#FF8000", "previous immutable frame is unchanged");
    require(sampler.calls() == 4, "one port call per update");
    require(sampler.positions() == std::vector(4, point),
            "explicit lens position reaches the port");
    controller.refresh(std::unexpected(SamplingFailure::position_unavailable));
    require(!controller.frame().sample(), "invalid position clears the old pixels");
    require(sampler.calls() == 4, "invalid position never calls the capture port");
}

void verify_copy()
{
    using namespace neneloupe;
    SequenceSampler sampler({sample_with(RgbColor::from_channels(255, 128, 0)),
                             std::unexpected(SamplingFailure::capture_unavailable)});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    const auto point = ScreenPosition::from_physical_pixels(10, 20);
    controller.refresh(point);
    require(controller.frame().copy() == CopyState::idle, "no notice before a copy");
    controller.copy_current_value();
    require(clipboard.written() == L"#FF8000", "the shown string is the copied string");
    require(controller.frame().copy() == CopyState::copied, "success is visible");
    controller.clear_copy_state();
    require(controller.frame().copy() == CopyState::idle, "the notice goes away");
    controller.select_format(ColorFormat::rgb_decimal);
    controller.copy_current_value();
    require(clipboard.written() == L"255, 128, 0", "copying follows the current format");
    controller.refresh(point);
    require(!controller.frame().has_color(), "capture failed");
    controller.copy_current_value();
    require(controller.frame().copy() == CopyState::failed, "cannot copy what is not there");
    require(clipboard.calls() == 2, "a failed sample never reaches the clipboard");
}

void verify_copy_failure()
{
    using namespace neneloupe;
    SequenceSampler sampler({sample_with(neneloupe::RgbColor::from_channels(1, 2, 3))});
    RecordingClipboard clipboard;
    clipboard.breaks();
    MemorySettingsStore store;
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    controller.refresh(ScreenPosition::from_physical_pixels(0, 0));
    controller.copy_current_value();
    require(controller.frame().copy() == CopyState::failed, "clipboard failure is visible");
}

void verify_theme_resolution()
{
    using namespace neneloupe;
    SequenceSampler sampler({});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    FixedAppearance appearance;
    appearance.becomes(ThemeAppearance::dark);
    LoupeController controller(sampler, clipboard, store, appearance);
    const auto initial = appearance.calls();
    require(initial == 1, "the system is read once at construction");
    controller.select_theme(Theme::light);
    require(controller.settings_frame().theme() == Theme::light, "explicit light");
    require(appearance.calls() == initial, "an explicit theme never reads the system");
    controller.select_theme(Theme::dark);
    require(appearance.calls() == initial, "an explicit dark theme never reads the system");
    controller.select_theme(Theme::system);
    require(appearance.calls() == initial + 1, "following the system reads it once");
    controller.refresh_appearance();
    require(appearance.calls() == initial + 2, "the system is re-read only when asked");
    require(store.saves() == 3, "each choice is saved once");
    require(store.stored().theme() == Theme::system, "the choice reaches the store");
}

void verify_settings_status()
{
    using namespace neneloupe;
    SequenceSampler sampler({});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    store.cannot_read();
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    require(controller.settings_frame().status() == SettingsStatus::load_failed,
            "an unreadable store is visible");
    require(!controller.settings_frame().status_text().empty(), "the failure has words");
    require(controller.settings_frame().theme() == Theme::system,
            "defaults are used but the failure is not hidden");
    store.cannot_write();
    controller.select_layer(WindowLayer::normal);
    require(controller.settings_frame().status() == SettingsStatus::save_failed,
            "an unwritable store is visible");
    require(controller.layer() == WindowLayer::normal,
            "the choice still applies to the running window");
    store.can_write();
    controller.select_layer(WindowLayer::topmost);
    require(controller.settings_frame().status() == SettingsStatus::ok,
            "a later success clears the notice");
    require(controller.settings_frame().status_text().empty(), "no words when nothing is wrong");
    require(store.loads() == 1, "the store is read once");
}

void verify_format_cycling_through_controller()
{
    using namespace neneloupe;
    SequenceSampler sampler({sample_with(RgbColor::from_channels(255, 128, 0))});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    controller.refresh(ScreenPosition::from_physical_pixels(5, 5));
    require(controller.format() == ColorFormat::rgb_hex, "starts at hex");
    controller.cycle_format();
    require(controller.frame().caption() == L"0, 50, 100, 0", "hex then cmyk");
    controller.cycle_format();
    require(controller.frame().caption() == L"30, 100%, 50%", "cmyk then hsl");
    controller.cycle_format();
    require(controller.frame().caption() == L"30, 100%, 100%", "hsl then hsv");
    controller.cycle_format();
    require(controller.frame().caption() == L"255, 128, 0", "hsv then rgb");
    controller.cycle_format();
    require(controller.frame().caption() == L"#FF8000", "rgb then hex");
    require(store.saves() == 5, "every switch is saved");
}
} // namespace

void verify_position_changes()
{
    using namespace neneloupe;
    const auto first = ScreenPosition::from_physical_pixels(120, 80);
    const auto second = ScreenPosition::from_physical_pixels(-900, -50);
    SequenceSampler sampler({sample_with(RgbColor::from_channels(255, 255, 255)),
                             sample_with(RgbColor::from_channels(255, 128, 0))});
    RecordingClipboard clipboard;
    MemorySettingsStore store;
    FixedAppearance appearance;
    LoupeController controller(sampler, clipboard, store, appearance);
    controller.refresh(first);
    controller.refresh(std::unexpected(SamplingFailure::position_unavailable));
    controller.refresh(second);
    require(sampler.positions() == std::vector{first, second},
            "moving the lens changes its target");
    require(controller.frame().caption() == L"#FF8000", "position recovery samples the new target");
}

int main(int argc, char **argv)
{
    verify_colors();
    verify_sample_ownership();
    if (argc == 2 && std::string_view(argv[1]) == "--coverage-negative")
    {
        return 0;
    }
    verify_hue_rounding();
    verify_format_cycle();
    verify_capture_exclusion_argument();
    verify_palette();
    verify_settings_value();
    verify_sample_rejection();
    verify_updates();
    verify_copy();
    verify_copy_failure();
    verify_theme_resolution();
    verify_settings_status();
    verify_format_cycling_through_controller();
    verify_position_changes();
    std::puts("Loupe unit tests passed: colour text, hue rounding, formats, palette, settings, "
              "sampling, copy and failure reporting.");
    return 0;
}
