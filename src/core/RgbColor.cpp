#include "RgbColor.hpp"

namespace neneloupe
{
RgbColor::RgbColor(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
    : red_(red), green_(green), blue_(blue)
{
}

RgbColor RgbColor::from_channels(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
{
    return RgbColor(red, green, blue);
}

std::uint8_t RgbColor::red() const noexcept
{
    return red_;
}

std::uint8_t RgbColor::green() const noexcept
{
    return green_;
}

std::uint8_t RgbColor::blue() const noexcept
{
    return blue_;
}

std::string RgbColor::hex() const
{
    constexpr char digits[] = "0123456789ABCDEF";
    return {'#',
            digits[red_ >> 4],
            digits[red_ & 15],
            digits[green_ >> 4],
            digits[green_ & 15],
            digits[blue_ >> 4],
            digits[blue_ & 15]};
}
} // namespace neneloupe
