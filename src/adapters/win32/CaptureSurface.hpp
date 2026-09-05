#pragma once

#include "ScreenSamplerPort.hpp"
#include <Windows.h>
#include <memory>

namespace neneloupe
{
class CaptureSurface final
{
  public:
    static std::expected<std::unique_ptr<CaptureSurface>, SamplingFailure> create();
    ~CaptureSurface();
    CaptureSurface(const CaptureSurface &) = delete;
    CaptureSurface &operator=(const CaptureSurface &) = delete;
    std::expected<ScreenSample, SamplingFailure> capture(POINT cursor);

  private:
    CaptureSurface() = default;
    bool initialize();
    bool copy_pixels(POINT cursor);
    HDC screen_ = nullptr;
    HDC memory_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ previous_ = nullptr;
    void *pixels_ = nullptr;
};
} // namespace neneloupe
