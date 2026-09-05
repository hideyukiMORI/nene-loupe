#pragma once

namespace neneloupe
{
class ScreenPosition final
{
  public:
    static ScreenPosition from_physical_pixels(int x, int y);
    int x() const noexcept;
    int y() const noexcept;
    bool operator==(const ScreenPosition &) const = default;

  private:
    ScreenPosition(int x, int y);
    int x_;
    int y_;
};
} // namespace neneloupe
