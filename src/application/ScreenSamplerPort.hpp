#pragma once

#include "SamplingFailure.hpp"
#include "ScreenPosition.hpp"
#include "ScreenSample.hpp"

#include <expected>

namespace neneloupe
{
class ScreenSamplerPort
{
  public:
    virtual ~ScreenSamplerPort() = default;
    virtual std::expected<ScreenSample, SamplingFailure> sample(ScreenPosition position) = 0;
};
} // namespace neneloupe
