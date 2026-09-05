#include "ScreenSample.hpp"

namespace neneloupe
{
ScreenSample::ScreenSample(std::span<const RgbColor> pixels) : pixels_(pixels.begin(), pixels.end())
{
}

std::expected<ScreenSample, SampleRejection> ScreenSample::create(std::span<const RgbColor> pixels)
{
    if (pixels.size() != side() * side())
    {
        return std::unexpected(SampleRejection::wrong_pixel_count);
    }
    return ScreenSample(pixels);
}

std::span<const RgbColor> ScreenSample::pixels() const noexcept
{
    return pixels_;
}

RgbColor ScreenSample::center() const noexcept
{
    return pixels_[pixels_.size() / 2];
}
} // namespace neneloupe
