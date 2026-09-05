#include "Win32SettingsStoreAdapter.hpp"

#include <Windows.h>
#include <array>
#include <fstream>
#include <string_view>
#include <utility>

namespace neneloupe
{
namespace
{
constexpr std::wstring_view folder_name = L"NeNeLoupe";
constexpr std::wstring_view file_name = L"settings.v1.txt";
constexpr std::wstring_view pending_name = L"settings.v1.txt.tmp";
constexpr std::size_t line_count = 4;

std::optional<std::string> value_of(const std::string &line, std::string_view key)
{
    if (!line.starts_with(key) || line.size() <= key.size() || line[key.size()] != '=')
    {
        return std::nullopt;
    }
    return line.substr(key.size() + 1);
}

std::optional<Theme> parse_theme(const std::string &text)
{
    if (text == "dark")
    {
        return Theme::dark;
    }
    if (text == "light")
    {
        return Theme::light;
    }
    if (text == "system")
    {
        return Theme::system;
    }
    return std::nullopt;
}

std::optional<ColorFormat> parse_format(const std::string &text)
{
    static constexpr std::array<std::pair<std::string_view, ColorFormat>, 5> names{
        {{"rgb", ColorFormat::rgb_decimal},
         {"hex", ColorFormat::rgb_hex},
         {"cmyk", ColorFormat::cmyk},
         {"hsl", ColorFormat::hsl},
         {"hsv", ColorFormat::hsv}}};
    for (const auto &entry : names)
    {
        if (text == entry.first)
        {
            return entry.second;
        }
    }
    return std::nullopt;
}

std::optional<WindowLayer> parse_layer(const std::string &text)
{
    if (text == "topmost")
    {
        return WindowLayer::topmost;
    }
    if (text == "normal")
    {
        return WindowLayer::normal;
    }
    return std::nullopt;
}

std::string theme_text(Theme theme)
{
    switch (theme)
    {
    case Theme::dark:
        return "dark";
    case Theme::light:
        return "light";
    case Theme::system:
        return "system";
    }
    std::unreachable();
}

std::string format_text(ColorFormat format)
{
    switch (format)
    {
    case ColorFormat::rgb_decimal:
        return "rgb";
    case ColorFormat::rgb_hex:
        return "hex";
    case ColorFormat::cmyk:
        return "cmyk";
    case ColorFormat::hsl:
        return "hsl";
    case ColorFormat::hsv:
        return "hsv";
    }
    std::unreachable();
}

std::string layer_text(WindowLayer layer)
{
    switch (layer)
    {
    case WindowLayer::topmost:
        return "topmost";
    case WindowLayer::normal:
        return "normal";
    }
    std::unreachable();
}

std::string document(const LoupeSettings &settings)
{
    return "schema=" + std::to_string(Win32SettingsStoreAdapter::schema_version) + "\n" +
           "theme=" + theme_text(settings.theme()) + "\n" +
           "format=" + format_text(settings.format()) + "\n" +
           "layer=" + layer_text(settings.layer()) + "\n";
}
} // namespace

std::optional<std::filesystem::path> Win32SettingsStoreAdapter::directory()
{
    std::array<wchar_t, MAX_PATH> buffer{};
    const auto length =
        GetEnvironmentVariableW(L"LOCALAPPDATA", buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length == 0 || length >= buffer.size())
    {
        return std::nullopt;
    }
    return std::filesystem::path(std::wstring(buffer.data(), length)) / folder_name;
}

std::expected<std::vector<std::string>, SettingsFailure>
Win32SettingsStoreAdapter::read_lines(const std::filesystem::path &file)
{
    std::ifstream input(file, std::ios::binary);
    if (!input)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        lines.push_back(line);
    }
    // 途中で読み取りが壊れたものを、部分的な成功として扱わない。
    if (input.bad())
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    return lines;
}

std::expected<LoupeSettings, SettingsFailure>
Win32SettingsStoreAdapter::parse(const std::vector<std::string> &lines)
{
    // 版 1 はちょうど 4 行。余分な行があるものを版 1 として読まない。
    if (lines.size() != line_count)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    const auto schema = value_of(lines[0], "schema");
    if (!schema || *schema != std::to_string(schema_version))
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    const auto stored_theme = value_of(lines[1], "theme");
    const auto stored_format = value_of(lines[2], "format");
    const auto stored_layer = value_of(lines[3], "layer");
    if (!stored_theme || !stored_format || !stored_layer)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    const auto theme = parse_theme(*stored_theme);
    const auto format = parse_format(*stored_format);
    const auto layer = parse_layer(*stored_layer);
    if (!theme || !format || !layer)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    return LoupeSettings::of(*theme, *format, *layer);
}

std::expected<LoupeSettings, SettingsFailure> Win32SettingsStoreAdapter::load()
{
    const auto folder = directory();
    if (!folder)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    const auto file = *folder / file_name;
    std::error_code code;
    const bool present = std::filesystem::exists(file, code);
    if (code)
    {
        return std::unexpected(SettingsFailure::unreadable);
    }
    // 「まだ保存が無い」だけが失敗ではない。初回起動は既定値で始める。
    // 在るのに開けない・読めないものは、既定値へ黙って落とさず失敗として返す（ARC-009 / FR-016）。
    if (!present)
    {
        return LoupeSettings::defaults();
    }
    const auto lines = read_lines(file);
    if (!lines)
    {
        return std::unexpected(lines.error());
    }
    return parse(*lines);
}

std::expected<void, SettingsFailure> Win32SettingsStoreAdapter::save(const LoupeSettings &settings)
{
    const auto folder = directory();
    if (!folder)
    {
        return std::unexpected(SettingsFailure::unwritable);
    }
    std::error_code code;
    std::filesystem::create_directories(*folder, code);
    if (code)
    {
        return std::unexpected(SettingsFailure::unwritable);
    }
    const auto pending = *folder / pending_name;
    std::ofstream output(pending, std::ios::binary | std::ios::trunc);
    const auto text = document(settings);
    output.write(text.data(), static_cast<std::streamsize>(text.size()));
    output.close();
    if (!output)
    {
        return std::unexpected(SettingsFailure::unwritable);
    }
    // 半端なファイルを残さないため、書き上げてから置換する。
    if (!MoveFileExW(pending.c_str(), (*folder / file_name).c_str(), MOVEFILE_REPLACE_EXISTING))
    {
        return std::unexpected(SettingsFailure::unwritable);
    }
    return {};
}
} // namespace neneloupe
