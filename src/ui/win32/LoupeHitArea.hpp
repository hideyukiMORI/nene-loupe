#pragma once

namespace neneloupe
{
// ルーペ窓でクリックを受ける場所の閉じた集合。none はドラッグ（HTCAPTION）に回る。
enum class LoupeHitArea
{
    none,
    format_chip,
    gear,
    value
};
} // namespace neneloupe
