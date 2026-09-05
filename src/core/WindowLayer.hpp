#pragma once

namespace neneloupe
{
enum class WindowLayer
{
    topmost,
    normal
};

// 反転の意図はここが正本。選択肢が増えたら網羅性検査でここが落ちる。
WindowLayer next_layer(WindowLayer layer);
} // namespace neneloupe
