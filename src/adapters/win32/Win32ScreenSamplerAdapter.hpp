#pragma once
#include "ScreenSamplerPort.hpp"

namespace neneloupe
{
class Win32ScreenSamplerAdapter final : public ScreenSamplerPort
{
  public:
    std::expected<ScreenSample, SamplingFailure> sample() override;
};
} // namespace neneloupe
