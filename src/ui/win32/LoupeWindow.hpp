#pragma once

#include "LoupeController.hpp"
#include "LoupeHitArea.hpp"
#include "SettingsWindow.hpp"
#include "WindowFailure.hpp"

#include <Windows.h>
#include <expected>
#include <memory>
#include <string>

namespace neneloupe
{
class LoupeWindow final
{
  public:
    static std::expected<std::unique_ptr<LoupeWindow>, WindowFailure>
    create(HINSTANCE instance, LoupeController &controller);
    ~LoupeWindow();
    LoupeWindow(const LoupeWindow &) = delete;
    LoupeWindow &operator=(const LoupeWindow &) = delete;
    bool is_open() const noexcept;
    HWND handle() const noexcept;

  private:
    LoupeWindow(HINSTANCE instance, LoupeController &controller);
    std::expected<void, WindowFailure> initialize();
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM word, LPARAM data) noexcept;
    LRESULT dispatch(UINT message, WPARAM word, LPARAM data);
    LRESULT on_mouse(UINT message, LPARAM data);
    LRESULT hit_result(LPARAM data) const;
    bool controls_enabled() const;
    void begin_caption_drag(POINT point);
    void track_hover(POINT point);
    void render();
    void refresh();
    void activate(LoupeHitArea area);
    void open_settings();
    void show_format_menu(POINT client);
    std::expected<ScreenPosition, SamplingFailure> sampling_position() const;
    void change_dpi(WPARAM word, LPARAM data);
    HINSTANCE instance_;
    LoupeController &controller_;
    std::unique_ptr<SettingsWindow> settings_;
    std::wstring title_;
    HWND window_ = nullptr;
    ATOM class_ = 0;
    HICON large_icon_ = nullptr;
    HICON small_icon_ = nullptr;
    UINT dpi_ = 96;
    LoupeHitArea pressed_ = LoupeHitArea::none;
    LoupeHitArea hover_ = LoupeHitArea::none;
    bool tracking_ = false;
    POINT anchor_{};
};
} // namespace neneloupe
