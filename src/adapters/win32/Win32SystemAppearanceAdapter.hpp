#pragma once

#include "SystemAppearancePort.hpp"

namespace neneloupe
{
class Win32SystemAppearanceAdapter final : public SystemAppearancePort
{
  public:
    ThemeAppearance current() override;
};
} // namespace neneloupe
