#pragma once
#include "LoupeFrame.hpp"
#include <Windows.h>

namespace neneloupe
{
class LoupeRenderer final
{
  public:
    static void render(HDC dc, const LoupeFrame &frame, UINT dpi);

  private:
    static void render_content(HDC dc, const LoupeFrame &frame, UINT dpi);
    static void render_sample(HDC dc, const ScreenSample &sample, UINT dpi);
    static void render_caption(HDC dc, const LoupeFrame &frame, UINT dpi);
};
} // namespace neneloupe
