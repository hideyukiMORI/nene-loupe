#pragma once

#include "CaptureSurface.hpp"
#include "ScreenSamplerPort.hpp"

#include <memory>

namespace neneloupe
{
class Win32ScreenSamplerAdapter final : public ScreenSamplerPort
{
  public:
    std::expected<ScreenSample, SamplingFailure> sample(ScreenPosition position) override;

  private:
    // 30 ms ごとに GDI 資源を作り直さない。ただし失敗は固定しない。
    // 取得に失敗したら捨て、次の呼び出しで作り直す（ADR 0006 第 8 節）。
    std::unique_ptr<CaptureSurface> surface_;
};
} // namespace neneloupe
