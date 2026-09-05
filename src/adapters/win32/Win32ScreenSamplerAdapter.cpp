#include "Win32ScreenSamplerAdapter.hpp"
#include "CaptureSurface.hpp"

namespace neneloupe
{
std::expected<ScreenSample, SamplingFailure>
Win32ScreenSamplerAdapter::sample(ScreenPosition position)
{
    auto surface = CaptureSurface::create();
    if (!surface)
        return std::unexpected(surface.error());
    return (*surface)->capture(POINT{position.x(), position.y()});
}
} // namespace neneloupe
