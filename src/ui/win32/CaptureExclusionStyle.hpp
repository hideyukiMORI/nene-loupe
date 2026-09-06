#pragma once

#include "CaptureExclusion.hpp"

#include <Windows.h>

namespace neneloupe
{
// 取り込み除外から Win32 の表現への唯一の変換。窓はすべてここを通る（ARC-001）。
class CaptureExclusionStyle final
{
  public:
    static DWORD affinity(CaptureExclusion exclusion);
    static bool excludes(CaptureExclusion exclusion);
};
} // namespace neneloupe
