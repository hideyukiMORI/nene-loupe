#pragma once
#include "ScreenSamplerPort.hpp"

namespace neneloupe
{
class Win32ScreenSamplerAdapter final : public ScreenSamplerPort
{
  public:
    std::expected<ScreenSample, SamplingFailure> sample(ScreenPosition position) override;
};
} // namespace neneloupe
