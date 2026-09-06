#include "CaptureExclusion.hpp"

namespace neneloupe
{
namespace
{
constexpr std::wstring_view diagnostic_argument = L"--allow-screen-capture";
constexpr std::wstring_view separators = L" \t";
} // namespace

CaptureExclusion capture_exclusion_of(std::wstring_view command_line)
{
    auto start = command_line.find_first_not_of(separators);
    while (start != std::wstring_view::npos)
    {
        const auto end = command_line.find_first_of(separators, start);
        if (command_line.substr(start, end - start) == diagnostic_argument)
        {
            return CaptureExclusion::disabled;
        }
        start = command_line.find_first_not_of(separators, end);
    }
    return CaptureExclusion::enabled;
}
} // namespace neneloupe
