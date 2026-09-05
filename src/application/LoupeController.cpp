#include "LoupeController.hpp"

namespace neneloupe
{
LoupeController::LoupeController(ScreenSamplerPort &sampler)
    : sampler_(sampler), sample_(std::unexpected(SamplingFailure::position_unavailable))
{
}

void LoupeController::refresh(const std::expected<ScreenPosition, SamplingFailure> &position)
{
    if (!position)
    {
        sample_ = std::unexpected(position.error());
        return;
    }
    sample_ = sampler_.sample(*position);
}

LoupeFrame LoupeController::frame() const
{
    return LoupeFrame::from_sample(sample_);
}
} // namespace neneloupe
