#include "CaptureSurface.hpp"
#include <algorithm>
#include <cstdint>

namespace neneloupe
{
std::expected<std::unique_ptr<CaptureSurface>, SamplingFailure> CaptureSurface::create()
{
    auto surface = std::unique_ptr<CaptureSurface>(new CaptureSurface());
    if (!surface->initialize())
        return std::unexpected(SamplingFailure::capture_unavailable);
    return surface;
}

bool CaptureSurface::initialize()
{
    screen_ = GetDC(nullptr);
    if (!screen_)
        return false;
    memory_ = CreateCompatibleDC(screen_);
    if (!memory_)
        return false;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = ScreenSample::side();
    info.bmiHeader.biHeight = -ScreenSample::side();
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    bitmap_ = CreateDIBSection(screen_, &info, DIB_RGB_COLORS, &pixels_, nullptr, 0);
    if (!bitmap_)
        return false;
    previous_ = SelectObject(memory_, bitmap_);
    return previous_ && previous_ != HGDI_ERROR;
}

CaptureSurface::~CaptureSurface()
{
    if (previous_ && previous_ != HGDI_ERROR)
        SelectObject(memory_, previous_);
    if (bitmap_)
        DeleteObject(bitmap_);
    if (memory_)
        DeleteDC(memory_);
    if (screen_)
        ReleaseDC(nullptr, screen_);
}

std::expected<ScreenSample, SamplingFailure> CaptureSurface::capture(POINT cursor)
{
    constexpr int side = ScreenSample::side();
    auto *words = static_cast<std::uint32_t *>(pixels_);
    std::fill_n(words, side * side, 0U);
    if (!copy_pixels(cursor) || !GdiFlush())
        return std::unexpected(SamplingFailure::capture_unavailable);
    std::vector<RgbColor> colors;
    colors.reserve(side * side);
    for (int index = 0; index < side * side; ++index)
    {
        const auto word = words[index];
        colors.push_back(RgbColor::from_channels(static_cast<std::uint8_t>(word >> 16),
                                                 static_cast<std::uint8_t>(word >> 8),
                                                 static_cast<std::uint8_t>(word)));
    }
    auto sample = ScreenSample::create(colors);
    if (!sample)
        return std::unexpected(SamplingFailure::capture_unavailable);
    return std::move(*sample);
}

bool CaptureSurface::copy_pixels(POINT cursor)
{
    constexpr int side = ScreenSample::side();
    const RECT requested{cursor.x - side / 2, cursor.y - side / 2, cursor.x + side / 2 + 1,
                         cursor.y + side / 2 + 1};
    const int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const RECT desktop{left, top, left + GetSystemMetrics(SM_CXVIRTUALSCREEN),
                       top + GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    RECT visible{};
    if (!IntersectRect(&visible, &requested, &desktop))
        return false;
    return BitBlt(memory_, visible.left - requested.left, visible.top - requested.top,
                  visible.right - visible.left, visible.bottom - visible.top, screen_, visible.left,
                  visible.top, SRCCOPY | CAPTUREBLT) != FALSE;
}
} // namespace neneloupe
