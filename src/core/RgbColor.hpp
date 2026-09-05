#pragma once

#include <cstdint>

namespace neneloupe
{
class RgbColor final
{
  public:
    static RgbColor from_channels(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
    std::uint8_t red() const noexcept;
    std::uint8_t green() const noexcept;
    std::uint8_t blue() const noexcept;
    bool operator==(const RgbColor &) const = default;

  private:
    RgbColor(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
    std::uint8_t red_;
    std::uint8_t green_;
    std::uint8_t blue_;
};
} // namespace neneloupe
