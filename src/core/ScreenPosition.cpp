#include "ScreenPosition.hpp"

namespace neneloupe
{
ScreenPosition::ScreenPosition(int x, int y) : x_(x), y_(y) {}

ScreenPosition ScreenPosition::from_physical_pixels(int x, int y)
{
    return ScreenPosition(x, y);
}

int ScreenPosition::x() const noexcept
{
    return x_;
}

int ScreenPosition::y() const noexcept
{
    return y_;
}
} // namespace neneloupe
