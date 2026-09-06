#pragma once

#include "LoupeSettings.hpp"
#include "SettingsFailure.hpp"

#include <expected>

namespace neneloupe
{
class SettingsStorePort
{
  public:
    SettingsStorePort() = default;
    virtual ~SettingsStorePort() = default;
    SettingsStorePort(const SettingsStorePort &) = delete;
    SettingsStorePort &operator=(const SettingsStorePort &) = delete;
    // 保存が無い初回は既定値を返す。読めた版が扱えないときだけ unreadable（ARC-009）。
    virtual std::expected<LoupeSettings, SettingsFailure> load() = 0;
    virtual std::expected<void, SettingsFailure> save(const LoupeSettings &settings) = 0;
};
} // namespace neneloupe
