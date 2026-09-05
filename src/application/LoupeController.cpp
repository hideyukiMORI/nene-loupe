#include "LoupeController.hpp"

namespace neneloupe
{
LoupeController::LoupeController(ScreenSamplerPort &sampler)
    : sampler_(sampler), sample_(sampler.sample())
{
}

void LoupeController::refresh()
{
    sample_ = sampler_.sample();
}

LoupeFrame LoupeController::frame() const
{
    return LoupeFrame::from_sample(sample_);
}
} // namespace neneloupe
