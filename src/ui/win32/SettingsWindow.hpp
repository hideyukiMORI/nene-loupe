#pragma once

#include "LoupeController.hpp"
#include "SettingsHitArea.hpp"
#include "WindowFailure.hpp"

#include <Windows.h>
#include <expected>
#include <memory>

namespace neneloupe
{
// FR-009 の 2 枚目の窓。ルーペ窓をオーナーに持つモーダル（ADR 0006 第 5 節）。
class SettingsWindow final
{
  public:
    static std::expected<std::unique_ptr<SettingsWindow>, WindowFailure>
    create(HINSTANCE instance, HWND owner, LoupeController &controller);
    ~SettingsWindow();
    SettingsWindow(const SettingsWindow &) = delete;
    SettingsWindow &operator=(const SettingsWindow &) = delete;
    bool is_open() const noexcept;
    HWND handle() const noexcept;

  private:
    SettingsWindow(HINSTANCE instance, HWND owner, LoupeController &controller);
    std::expected<void, WindowFailure> initialize();
    void place();
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM word, LPARAM data) noexcept;
    LRESULT dispatch(UINT message, WPARAM word, LPARAM data);
    void render();
    void activate(SettingsHitArea area);
    void change_dpi(WPARAM word, LPARAM data);
    HINSTANCE instance_;
    HWND owner_;
    LoupeController &controller_;
    HWND window_ = nullptr;
    ATOM class_ = 0;
    UINT dpi_ = 96;
    SettingsHitArea pressed_ = SettingsHitArea::none;
};
} // namespace neneloupe
