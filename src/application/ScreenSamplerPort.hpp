#pragma once

#include "SamplingFailure.hpp"
#include "ScreenSample.hpp"

#include <expected>

namespace neneloupe
{
class ScreenSamplerPort
{
  public:
    virtual ~ScreenSamplerPort() = default;
    virtual std::expected<ScreenSample, SamplingFailure> sample() = 0;
};
} // namespace neneloupe
