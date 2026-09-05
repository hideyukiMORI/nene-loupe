#include "LoupeFrame.hpp"

#include <utility>

namespace neneloupe
{
LoupeFrame::LoupeFrame(std::optional<ScreenSample> sample, std::wstring caption)
    : sample_(std::move(sample)), caption_(std::move(caption))
{
}

LoupeFrame LoupeFrame::from_sample(const std::expected<ScreenSample, SamplingFailure> &sample)
{
    if (sample)
    {
        const auto text = sample->center().hex();
        return LoupeFrame(*sample, std::wstring(text.begin(), text.end()));
    }
    switch (sample.error())
    {
    case SamplingFailure::cursor_unavailable:
        return LoupeFrame(std::nullopt, L"カーソル取得不可");
    case SamplingFailure::capture_unavailable:
        return LoupeFrame(std::nullopt, L"画面取得不可");
    }
    std::unreachable();
}

const std::optional<ScreenSample> &LoupeFrame::sample() const noexcept
{
    return sample_;
}

const std::wstring &LoupeFrame::caption() const noexcept
{
    return caption_;
}
} // namespace neneloupe
