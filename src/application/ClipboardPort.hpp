#pragma once

#include "ClipboardFailure.hpp"

#include <expected>
#include <string>

namespace neneloupe
{
class ClipboardPort
{
  public:
    ClipboardPort() = default;
    virtual ~ClipboardPort() = default;
    ClipboardPort(const ClipboardPort &) = delete;
    ClipboardPort &operator=(const ClipboardPort &) = delete;
    virtual std::expected<void, ClipboardFailure> write(const std::wstring &text) = 0;
};
} // namespace neneloupe
