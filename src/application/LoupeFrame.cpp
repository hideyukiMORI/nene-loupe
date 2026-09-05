#include "LoupeFrame.hpp"

#include "ColorText.hpp"

#include <utility>

namespace neneloupe
{
namespace
{
// 色の表示文字列は ASCII だけなので、そのまま広げてよい。
std::wstring widen(const std::string &text)
{
    return std::wstring(text.begin(), text.end());
}

std::wstring failure_caption(SamplingFailure failure)
{
    switch (failure)
    {
    case SamplingFailure::position_unavailable:
        return L"位置取得不可";
    case SamplingFailure::capture_unavailable:
        return L"画面取得不可";
    }
    std::unreachable();
}
} // namespace

LoupeFrame::LoupeFrame(std::optional<ScreenSample> sample, std::wstring caption,
                       const LoupeSettings &settings, ThemeAppearance appearance)
    : sample_(std::move(sample)), caption_(std::move(caption)),
      format_label_(widen(ColorText::label(settings.format()))), format_(settings.format()),
      appearance_(appearance)
{
}

LoupeFrame LoupeFrame::of(const std::expected<ScreenSample, SamplingFailure> &sample,
                          const LoupeSettings &settings, ThemeAppearance appearance, CopyState copy)
{
    // コピーの状態は採取の成否と独立に運ぶ。失敗中に落とすと、
    // コピーできなかったことが利用者へ届かない（FR-016）。
    if (!sample)
    {
        LoupeFrame failed(std::nullopt, failure_caption(sample.error()), settings, appearance);
        failed.copy_ = copy;
        return failed;
    }
    auto text = widen(ColorText::of(sample->center(), settings.format()));
    LoupeFrame frame(*sample, std::move(text), settings, appearance);
    frame.copy_ = copy;
    return frame;
}

const std::optional<ScreenSample> &LoupeFrame::sample() const noexcept
{
    return sample_;
}

const std::wstring &LoupeFrame::caption() const noexcept
{
    return caption_;
}

const std::wstring &LoupeFrame::format_label() const noexcept
{
    return format_label_;
}

ColorFormat LoupeFrame::format() const noexcept
{
    return format_;
}

CopyState LoupeFrame::copy() const noexcept
{
    return copy_;
}

ThemePalette LoupeFrame::palette() const
{
    return ThemePalette::of(appearance_);
}

bool LoupeFrame::has_color() const noexcept
{
    return sample_.has_value();
}
} // namespace neneloupe
