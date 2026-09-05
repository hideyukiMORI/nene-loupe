#pragma once

#include "LoupeFrame.hpp"
#include "ScreenSamplerPort.hpp"

namespace neneloupe
{
class LoupeController final
{
  public:
    explicit LoupeController(ScreenSamplerPort &sampler);
    LoupeController(const LoupeController &) = delete;
    LoupeController &operator=(const LoupeController &) = delete;
    void refresh(const std::expected<ScreenPosition, SamplingFailure> &position);
    LoupeFrame frame() const;

  private:
    ScreenSamplerPort &sampler_;
    std::expected<ScreenSample, SamplingFailure> sample_;
};
} // namespace neneloupe
