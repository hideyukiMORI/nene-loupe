#pragma once

#include "RgbColor.hpp"
#include "SampleRejection.hpp"

#include <expected>
#include <span>
#include <vector>

namespace neneloupe
{
class ScreenSample final
{
  public:
    static constexpr int side() noexcept
    {
        return 7;
    }
    static std::expected<ScreenSample, SampleRejection> create(std::span<const RgbColor> pixels);
    std::span<const RgbColor> pixels() const noexcept;
    RgbColor center() const noexcept;

  private:
    explicit ScreenSample(std::span<const RgbColor> pixels);
    std::vector<RgbColor> pixels_;
};
} // namespace neneloupe
