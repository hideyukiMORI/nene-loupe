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
} // namespace neneloupe
