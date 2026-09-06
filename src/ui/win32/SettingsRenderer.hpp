#pragma once

#include "PaletteBrush.hpp"
#include "SettingsFrame.hpp"

#include <Windows.h>

namespace neneloupe
{
class SettingsRenderer final
{
  public:
    static void render(HDC dc, const SettingsFrame &frame, UINT dpi);

  private:
    static void render_content(HDC dc, const SettingsFrame &frame, UINT dpi);
    static void render_theme(HDC dc, const SettingsFrame &frame, UINT dpi);
    static void render_layer(HDC dc, const SettingsFrame &frame, UINT dpi);
    static void render_about(HDC dc, const SettingsFrame &frame, UINT dpi);
};
} // namespace neneloupe
