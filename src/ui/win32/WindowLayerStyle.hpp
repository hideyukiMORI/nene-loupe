#pragma once

#include "WindowLayer.hpp"

#include <Windows.h>

namespace neneloupe
{
// 重なり順から Win32 の表現への唯一の変換。両方の窓がここを通る（ARC-001）。
class WindowLayerStyle final
{
  public:
    static DWORD extended(WindowLayer layer);
    static HWND insert_after(WindowLayer layer);
    // 親と子は必ず同じ値にし、親を先に動かす（ADR 0006 第 4 節）。
    static void apply(HWND owner, HWND modal, WindowLayer layer);
};
} // namespace neneloupe
