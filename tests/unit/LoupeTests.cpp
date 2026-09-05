#include "LoupeController.hpp"
#include "RgbColor.hpp"
#include "ScreenSample.hpp"

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

void verify_colors()
{
    using neneloupe::RgbColor;
    require(RgbColor::from_channels(0, 0, 0).hex() == "#000000", "black");
    require(RgbColor::from_channels(255, 255, 255).hex() == "#FFFFFF", "white");
    require(RgbColor::from_channels(255, 128, 0).hex() == "#FF8000", "mixed color");
    require(RgbColor::from_channels(0, 1, 15).hex() == "#00010F", "leading zeros");
    for (unsigned int channel = 0; channel < 256; ++channel)
    {
        const auto byte = static_cast<std::uint8_t>(channel);
        const auto color = RgbColor::from_channels(byte, byte, byte);
        require(color.red() == byte && color.green() == byte && color.blue() == byte,
                "all channel values");
        require(color.hex().size() == 7, "fixed HEX width");
    }
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
    LoupeController controller(sampler);
    require(sampler.calls() == 0, "construction does not sample an unknown position");
    const auto point = ScreenPosition::from_physical_pixels(-1200, -300);
    require(point.x() == -1200 && point.y() == -300, "negative physical coordinates");
    controller.refresh(point);
    const auto original = controller.frame();
    require(original.caption() == L"#FF8000", "initial sample formatted by application");
    require(original.sample().has_value(), "initial pixels available");
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
    require(controller.frame().caption() == L"位置取得不可", "invalid position is explicit");
    require(sampler.calls() == 4, "invalid position never calls the capture port");
}
} // namespace

void verify_position_changes()
{
    using namespace neneloupe;
    const auto first = ScreenPosition::from_physical_pixels(120, 80);
    const auto second = ScreenPosition::from_physical_pixels(-900, -50);
    SequenceSampler sampler({sample_with(RgbColor::from_channels(255, 255, 255)),
                             sample_with(RgbColor::from_channels(255, 128, 0))});
    LoupeController controller(sampler);
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
    verify_sample_rejection();
    verify_updates();
    verify_position_changes();
    std::puts("Loupe unit tests passed: color channels, sample ownership, rejection, failure and "
              "recovery.");
    return 0;
}
