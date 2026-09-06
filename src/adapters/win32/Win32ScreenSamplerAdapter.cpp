#include "Win32ScreenSamplerAdapter.hpp"

#include <utility>

namespace neneloupe
{
std::expected<ScreenSample, SamplingFailure>
Win32ScreenSamplerAdapter::sample(ScreenPosition position)
{
    if (!surface_)
    {
        auto created = CaptureSurface::create();
        if (!created)
        {
            return std::unexpected(created.error());
        }
        surface_ = std::move(*created);
    }
    auto result = surface_->capture(POINT{position.x(), position.y()});
    if (!result)
    {
        surface_.reset();
    }
    return result;
}
} // namespace neneloupe
