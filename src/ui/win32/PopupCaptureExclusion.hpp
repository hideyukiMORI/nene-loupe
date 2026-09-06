#pragma once

#include <Windows.h>
#include <expected>
#include <memory>

namespace neneloupe
{
class PopupCaptureExclusion final
{
  public:
    enum class Failure
    {
        installation_failed,
    };

    static std::expected<std::unique_ptr<PopupCaptureExclusion>, Failure> create(HWND owner);
    ~PopupCaptureExclusion();
    PopupCaptureExclusion(const PopupCaptureExclusion &) = delete;
    PopupCaptureExclusion &operator=(const PopupCaptureExclusion &) = delete;

  private:
    PopupCaptureExclusion() noexcept;
    static LRESULT CALLBACK callback(int code, WPARAM word, LPARAM data) noexcept;
    static bool is_popup_menu(HWND window) noexcept;
    static void exclude_or_close(HWND window) noexcept;
    HHOOK hook_ = nullptr;
};
} // namespace neneloupe
