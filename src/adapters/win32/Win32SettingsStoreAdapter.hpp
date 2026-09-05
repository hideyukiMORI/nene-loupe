#pragma once

#include "SettingsStorePort.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace neneloupe
{
// 版 1 の保存経路。%LOCALAPPDATA% の NeNeLoupe フォルダに settings.v1.txt を置く（ADR 0006 第 6
// 節）。
class Win32SettingsStoreAdapter final : public SettingsStorePort
{
  public:
    static constexpr int schema_version = 1;
    std::expected<LoupeSettings, SettingsFailure> load() override;
    std::expected<void, SettingsFailure> save(const LoupeSettings &settings) override;

  private:
    static std::optional<std::filesystem::path> directory();
    static std::expected<std::vector<std::string>, SettingsFailure>
    read_lines(const std::filesystem::path &file);
    static std::expected<LoupeSettings, SettingsFailure>
    parse(const std::vector<std::string> &lines);
};
} // namespace neneloupe
