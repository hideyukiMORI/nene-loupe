#include "LoupeRenderer.hpp"
#include <memory>
#include <type_traits>

namespace neneloupe
{
namespace
{
int scale(int value, UINT dpi)
{
    return MulDiv(value, static_cast<int>(dpi), 96);
}
} // namespace

void LoupeRenderer::render(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    const auto delete_dc = [](HDC memory) { DeleteDC(memory); };
    std::unique_ptr<std::remove_pointer_t<HDC>, decltype(delete_dc)> memory(CreateCompatibleDC(dc),
                                                                            delete_dc);
    if (!memory)
        return;
    const auto delete_bitmap = [](HBITMAP bitmap) { DeleteObject(bitmap); };
    std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(delete_bitmap)> bitmap(
        CreateCompatibleBitmap(dc, scale(160, dpi), scale(64, dpi)), delete_bitmap);
    if (!bitmap)
        return;
    const auto previous = SelectObject(memory.get(), bitmap.get());
    if (!previous || previous == HGDI_ERROR)
        return;
    render_content(memory.get(), frame, dpi);
    BitBlt(dc, 0, 0, scale(160, dpi), scale(64, dpi), memory.get(), 0, 0, SRCCOPY);
    SelectObject(memory.get(), previous);
}

void LoupeRenderer::render_content(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    RECT bounds{0, 0, scale(160, dpi), scale(64, dpi)};
    SetDCBrushColor(dc, RGB(24, 26, 30));
    FillRect(dc, &bounds, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
    if (frame.sample())
        render_sample(dc, *frame.sample(), dpi);
    render_caption(dc, frame, dpi);
}

void LoupeRenderer::render_sample(HDC dc, const ScreenSample &sample, UINT dpi)
{
    const auto pixels = sample.pixels();
    for (int index = 0; index < ScreenSample::side() * ScreenSample::side(); ++index)
    {
        const int x = 4 + (index % ScreenSample::side()) * 8;
        const int y = 4 + (index / ScreenSample::side()) * 8;
        RECT cell{scale(x, dpi), scale(y, dpi), scale(x + 8, dpi), scale(y + 8, dpi)};
        const auto color = pixels[static_cast<std::size_t>(index)];
        SetDCBrushColor(dc, RGB(color.red(), color.green(), color.blue()));
        FillRect(dc, &cell, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
    }
    const auto center = lens_center(dpi);
    RECT marker{center.x - scale(5, dpi), center.y - scale(5, dpi), center.x + scale(5, dpi),
                center.y + scale(5, dpi)};
    FrameRect(dc, &marker, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    InflateRect(&marker, -scale(1, dpi), -scale(1, dpi));
    FrameRect(dc, &marker, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
}

POINT LoupeRenderer::lens_center(UINT dpi)
{
    const auto center = scale(4 + ScreenSample::side() * 8 / 2, dpi);
    return POINT{center, center};
}

void LoupeRenderer::render_caption(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    LOGFONTW description{};
    description.lfHeight = -scale(13, dpi);
    description.lfWeight = FW_SEMIBOLD;
    description.lfQuality = CLEARTYPE_QUALITY;
    wcscpy_s(description.lfFaceName, L"Segoe UI");
    const auto delete_font = [](HFONT font) { DeleteObject(font); };
    std::unique_ptr<std::remove_pointer_t<HFONT>, decltype(delete_font)> font(
        CreateFontIndirectW(&description), delete_font);
    HGDIOBJ previous = nullptr;
    if (font)
        previous = SelectObject(dc, font.get());
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(150, 156, 169));
    RECT label{scale(68, dpi), scale(9, dpi), scale(156, dpi), scale(26, dpi)};
    DrawTextW(dc, L"HEX", -1, &label, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX);
    SetTextColor(dc, RGB(245, 246, 250));
    RECT value{scale(68, dpi), scale(28, dpi), scale(158, dpi), scale(55, dpi)};
    DrawTextW(dc, frame.caption().c_str(), -1, &value, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX);
    if (previous && previous != HGDI_ERROR)
        SelectObject(dc, previous);
}
} // namespace neneloupe
