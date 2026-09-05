#pragma once

#include "ClipboardPort.hpp"

#include <Windows.h>

namespace neneloupe
{
class Win32ClipboardAdapter final : public ClipboardPort
{
  public:
    // OpenClipboard に NULL を渡すと所有者が NULL になり SetClipboardData が失敗する。
    // 合成ルートが窓を作ったあとで、実在する所有者を結ぶ。
    void bind(HWND owner) noexcept;
    std::expected<void, ClipboardFailure> write(const std::wstring &text) override;

  private:
    static std::expected<void, ClipboardFailure> store(HGLOBAL handle);
    HWND owner_ = nullptr;
};
} // namespace neneloupe
