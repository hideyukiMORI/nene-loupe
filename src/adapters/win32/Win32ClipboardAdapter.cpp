#include "Win32ClipboardAdapter.hpp"

#include <cstring>

namespace neneloupe
{
void Win32ClipboardAdapter::bind(HWND owner) noexcept
{
    owner_ = owner;
}

std::expected<void, ClipboardFailure> Win32ClipboardAdapter::write(const std::wstring &text)
{
    if (!owner_)
    {
        return std::unexpected(ClipboardFailure::unavailable);
    }
    // 確保と複写を先に済ませる。ここで失敗しても利用者の既存のクリップボードは消さない。
    const std::size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!handle)
    {
        return std::unexpected(ClipboardFailure::write_failed);
    }
    void *memory = GlobalLock(handle);
    if (!memory)
    {
        GlobalFree(handle);
        return std::unexpected(ClipboardFailure::write_failed);
    }
    std::memcpy(memory, text.c_str(), bytes);
    GlobalUnlock(handle);
    if (!OpenClipboard(owner_))
    {
        GlobalFree(handle);
        return std::unexpected(ClipboardFailure::unavailable);
    }
    const auto result = store(handle);
    CloseClipboard();
    // 置けたときだけ所有権がシステムへ移る。置けなければこちらで解放する。
    if (!result)
    {
        GlobalFree(handle);
    }
    return result;
}

std::expected<void, ClipboardFailure> Win32ClipboardAdapter::store(HGLOBAL handle)
{
    if (!EmptyClipboard())
    {
        return std::unexpected(ClipboardFailure::write_failed);
    }
    if (!SetClipboardData(CF_UNICODETEXT, handle))
    {
        return std::unexpected(ClipboardFailure::write_failed);
    }
    return {};
}
} // namespace neneloupe
