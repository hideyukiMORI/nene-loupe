#pragma once

#include "SamplingFailure.hpp"
#include "ScreenSample.hpp"

#include <expected>
#include <optional>
#include <string>

namespace neneloupe
{
class LoupeFrame final
{
  public:
    static LoupeFrame from_sample(const std::expected<ScreenSample, SamplingFailure> &sample);
    const std::optional<ScreenSample> &sample() const noexcept;
    const std::wstring &caption() const noexcept;

  private:
    LoupeFrame(std::optional<ScreenSample> sample, std::wstring caption);
    std::optional<ScreenSample> sample_;
    std::wstring caption_;
};
} // namespace neneloupe
