#include "Win32ScreenSamplerAdapter.hpp"
#include "CaptureSurface.hpp"

namespace neneloupe
{
std::expected<ScreenSample, SamplingFailure> Win32ScreenSamplerAdapter::sample()
{
    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return std::unexpected(SamplingFailure::cursor_unavailable);
    auto surface = CaptureSurface::create();
    if (!surface)
        return std::unexpected(surface.error());
    return (*surface)->capture(cursor);
}
} // namespace neneloupe
