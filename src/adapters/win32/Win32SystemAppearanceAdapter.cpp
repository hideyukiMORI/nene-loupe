#include "Win32SystemAppearanceAdapter.hpp"

#include <Windows.h>

namespace neneloupe
{
namespace
{
constexpr wchar_t personalize_key[] =
    LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)";
}

ThemeAppearance Win32SystemAppearanceAdapter::current()
{
    DWORD light = 1;
    DWORD size = sizeof(light);
    const auto status = RegGetValueW(HKEY_CURRENT_USER, personalize_key, L"AppsUseLightTheme",
                                     RRF_RT_REG_DWORD, nullptr, &light, &size);
    // 値が無い環境は Windows の既定に合わせてライトとして扱う。失敗としては返さない。
    if (status != ERROR_SUCCESS)
    {
        return ThemeAppearance::light;
    }
    return light == 0 ? ThemeAppearance::dark : ThemeAppearance::light;
}
} // namespace neneloupe
