#pragma once

#include "ThemeAppearance.hpp"

namespace neneloupe
{
class SystemAppearancePort
{
  public:
    SystemAppearancePort() = default;
    virtual ~SystemAppearancePort() = default;
    SystemAppearancePort(const SystemAppearancePort &) = delete;
    SystemAppearancePort &operator=(const SystemAppearancePort &) = delete;
    virtual ThemeAppearance current() = 0;
};
} // namespace neneloupe
