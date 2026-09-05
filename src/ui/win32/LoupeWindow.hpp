#pragma once
#include "LoupeController.hpp"
#include "WindowFailure.hpp"
#include <Windows.h>
#include <memory>

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

  private:
    LoupeWindow(HINSTANCE instance, LoupeController &controller);
    std::expected<void, WindowFailure> initialize();
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM word, LPARAM data) noexcept;
    LRESULT dispatch(UINT message, WPARAM word, LPARAM data);
    void render();
    void refresh();
    void change_dpi(WPARAM word, LPARAM data);
    HINSTANCE instance_;
    LoupeController &controller_;
    HWND window_ = nullptr;
    ATOM class_ = 0;
    UINT dpi_ = 96;
};
} // namespace neneloupe
