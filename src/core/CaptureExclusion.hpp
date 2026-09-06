#pragma once

#include <string_view>

namespace neneloupe
{
// 自分の窓を画面の取り込みから外すかどうか。既定は enabled（ADR 0005）。
enum class CaptureExclusion
{
    enabled,
    disabled
};

// 文書化した診断用の起動引数だけがこの選択を動かす（ADR 0007）。
// 引数の解釈はここが正本。窓ごとに別の判断を持たない。
CaptureExclusion capture_exclusion_of(std::wstring_view command_line);
} // namespace neneloupe
