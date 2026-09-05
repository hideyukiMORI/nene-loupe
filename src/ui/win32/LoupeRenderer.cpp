#include "LoupeRenderer.hpp"

#include "LoupeLayout.hpp"

#include <array>
#include <cmath>
#include <memory>
#include <type_traits>

namespace neneloupe
{
namespace
{
using FontHolder = std::unique_ptr<std::remove_pointer_t<HFONT>, void (*)(HFONT)>;
using PenHolder = std::unique_ptr<std::remove_pointer_t<HPEN>, void (*)(HPEN)>;

void delete_font(HFONT font)
{
    DeleteObject(font);
}

void delete_pen(HPEN pen)
{
    DeleteObject(pen);
}

constexpr int gear_teeth = 8;
constexpr int gear_points = gear_teeth * 4;

POINT polar(POINT center, double radius, double degrees)
{
    const double angle = (degrees - 90.0) * 3.14159265358979323846 / 180.0;
    return POINT{center.x + static_cast<LONG>(std::lround(radius * std::cos(angle))),
                 center.y + static_cast<LONG>(std::lround(radius * std::sin(angle)))};
}
} // namespace

LOGFONTW LoupeRenderer::font_of(const wchar_t *face, int height, int weight, UINT dpi)
{
    LOGFONTW description{};
    description.lfHeight = -LoupeLayout::scale(height, dpi);
    description.lfWeight = weight;
    description.lfQuality = CLEARTYPE_QUALITY;
    wcscpy_s(description.lfFaceName, face);
    return description;
}

void LoupeRenderer::write_text(HDC dc, RECT bounds, const std::wstring &text,
                               const LOGFONTW &description)
{
    FontHolder font(CreateFontIndirectW(&description), delete_font);
    HGDIOBJ previous = nullptr;
    if (font)
    {
        previous = SelectObject(dc, font.get());
    }
    SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, text.c_str(), -1, &bounds, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
    if (previous && previous != HGDI_ERROR)
    {
        SelectObject(dc, previous);
    }
}

void LoupeRenderer::draw_gear(HDC dc, RECT bounds, COLORREF mark, COLORREF hole)
{
    const POINT center{(bounds.left + bounds.right) / 2, (bounds.top + bounds.bottom) / 2};
    const double span = static_cast<double>(bounds.right - bounds.left);
    std::array<POINT, gear_points> outline{};
    for (int tooth = 0; tooth < gear_teeth; ++tooth)
    {
        const double base = tooth * 45.0;
        outline[static_cast<std::size_t>(tooth) * 4] = polar(center, span * 0.315, base);
        outline[static_cast<std::size_t>(tooth) * 4 + 1] = polar(center, span * 0.47, base + 10.0);
        outline[static_cast<std::size_t>(tooth) * 4 + 2] = polar(center, span * 0.47, base + 35.0);
        outline[static_cast<std::size_t>(tooth) * 4 + 3] = polar(center, span * 0.315, base + 45.0);
    }
    SetDCBrushColor(dc, mark);
    SetDCPenColor(dc, mark);
    const auto previous_brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    const auto previous_pen = SelectObject(dc, GetStockObject(DC_PEN));
    Polygon(dc, outline.data(), gear_points);
    // 中心の穴は背景色で抜く。歯車の下は必ず平塗りなので、これで足りる。
    SetDCBrushColor(dc, hole);
    SetDCPenColor(dc, hole);
    const auto radius = static_cast<LONG>(std::lround(span * 0.16));
    Ellipse(dc, center.x - radius, center.y - radius, center.x + radius, center.y + radius);
    SelectObject(dc, previous_brush);
    SelectObject(dc, previous_pen);
}

void LoupeRenderer::draw_check(HDC dc, RECT bounds, COLORREF mark, UINT dpi)
{
    const LONG width = bounds.right - bounds.left;
    const std::array<POINT, 3> stroke{POINT{bounds.left + width / 6, bounds.top + width / 2},
                                      POINT{bounds.left + width * 2 / 5, bounds.bottom - width / 4},
                                      POINT{bounds.right - width / 6, bounds.top + width / 4}};
    PenHolder pen(CreatePen(PS_SOLID, LoupeLayout::scale(2, dpi), mark), delete_pen);
    if (!pen)
    {
        return;
    }
    const auto previous = SelectObject(dc, pen.get());
    Polyline(dc, stroke.data(), static_cast<int>(stroke.size()));
    SelectObject(dc, previous);
}

void LoupeRenderer::draw_cross(HDC dc, RECT bounds, COLORREF mark, UINT dpi)
{
    RECT inner = bounds;
    InflateRect(&inner, -LoupeLayout::scale(2, dpi), -LoupeLayout::scale(2, dpi));
    PenHolder pen(CreatePen(PS_SOLID, LoupeLayout::scale(2, dpi), mark), delete_pen);
    if (!pen)
    {
        return;
    }
    const auto previous = SelectObject(dc, pen.get());
    MoveToEx(dc, inner.left, inner.top, nullptr);
    LineTo(dc, inner.right, inner.bottom);
    MoveToEx(dc, inner.right, inner.top, nullptr);
    LineTo(dc, inner.left, inner.bottom);
    SelectObject(dc, previous);
}

void LoupeRenderer::render(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover)
{
    const auto bounds = LoupeLayout::window(dpi);
    const auto delete_dc = [](HDC memory) { DeleteDC(memory); };
    std::unique_ptr<std::remove_pointer_t<HDC>, decltype(delete_dc)> memory(CreateCompatibleDC(dc),
                                                                            delete_dc);
    const auto delete_bitmap = [](HBITMAP bitmap) { DeleteObject(bitmap); };
    std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(delete_bitmap)> bitmap(
        memory ? CreateCompatibleBitmap(dc, bounds.right, bounds.bottom) : nullptr, delete_bitmap);
    const auto previous = bitmap ? SelectObject(memory.get(), bitmap.get()) : nullptr;
    if (!previous || previous == HGDI_ERROR)
    {
        // 二重バッファを用意できないときも、地色だけは必ず置いて未定義の内容を残さない。
        PaletteBrush(frame.palette()).fill(dc, bounds, PaletteRole::background);
        return;
    }
    render_content(memory.get(), frame, dpi, hover);
    BitBlt(dc, 0, 0, bounds.right, bounds.bottom, memory.get(), 0, 0, SRCCOPY);
    SelectObject(memory.get(), previous);
}

void LoupeRenderer::render_content(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover)
{
    const PaletteBrush brush(frame.palette());
    const auto bounds = LoupeLayout::window(dpi);
    brush.fill(dc, bounds, PaletteRole::background);
    brush.outline(dc, bounds, PaletteRole::border);
    render_lens(dc, frame, dpi);
    render_swatch(dc, frame, dpi);
    render_chip(dc, frame, dpi, hover);
    render_toast(dc, frame, dpi);
    brush.fill_color(dc, LoupeLayout::gear_glyph(dpi), brush.color(PaletteRole::background));
    draw_gear(dc, LoupeLayout::gear_glyph(dpi), brush.color(PaletteRole::muted_text),
              brush.color(PaletteRole::background));
    render_value(dc, frame, dpi, hover);
}

void LoupeRenderer::render_lens(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    brush.outline(dc, LoupeLayout::lens_frame(dpi), PaletteRole::line);
    if (!frame.sample())
    {
        brush.fill(dc, LoupeLayout::lens_pixels(dpi), PaletteRole::surface);
        return;
    }
    const auto pixels = frame.sample()->pixels();
    const int side = ScreenSample::side();
    for (int index = 0; index < side * side; ++index)
    {
        const int x = 4 + (index % side) * LoupeLayout::lens_cell;
        const int y = 4 + (index / side) * LoupeLayout::lens_cell;
        const auto cell = LoupeLayout::scaled(
            RECT{x, y, x + LoupeLayout::lens_cell, y + LoupeLayout::lens_cell}, dpi);
        const auto color = pixels[static_cast<std::size_t>(index)];
        PaletteBrush::fill_color(dc, cell, RGB(color.red(), color.green(), color.blue()));
    }
    auto marker = LoupeLayout::marker_outer(dpi);
    FrameRect(dc, &marker, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    auto inner = LoupeLayout::marker_inner(dpi);
    FrameRect(dc, &inner, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
}

void LoupeRenderer::render_swatch(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    brush.outline(dc, LoupeLayout::swatch_frame(dpi), PaletteRole::line);
    if (!frame.sample())
    {
        brush.fill(dc, LoupeLayout::swatch_fill(dpi), PaletteRole::surface);
        return;
    }
    const auto center = frame.sample()->center();
    PaletteBrush::fill_color(dc, LoupeLayout::swatch_fill(dpi),
                             RGB(center.red(), center.green(), center.blue()));
}

void LoupeRenderer::render_chip(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover)
{
    const PaletteBrush brush(frame.palette());
    const auto available = frame.has_color();
    const auto face =
        hover == LoupeHitArea::format_chip ? PaletteRole::surface_hover : PaletteRole::surface;
    brush.fill(dc, LoupeLayout::chip(dpi), available ? face : PaletteRole::background);
    brush.outline(dc, LoupeLayout::chip(dpi), PaletteRole::line);
    SetTextColor(dc, brush.color(available ? PaletteRole::text : PaletteRole::muted_text));
    write_text(dc, LoupeLayout::chip_text(dpi), frame.format_label(),
               font_of(L"Segoe UI", 12, FW_BOLD, dpi));
    const auto caret = LoupeLayout::caret(dpi);
    const std::array<POINT, 3> arrow{POINT{caret.left, caret.top}, POINT{caret.right, caret.top},
                                     POINT{(caret.left + caret.right) / 2, caret.bottom}};
    SetDCBrushColor(dc, brush.color(PaletteRole::muted_text));
    SetDCPenColor(dc, brush.color(PaletteRole::muted_text));
    const auto previous_brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    const auto previous_pen = SelectObject(dc, GetStockObject(DC_PEN));
    Polygon(dc, arrow.data(), static_cast<int>(arrow.size()));
    SelectObject(dc, previous_brush);
    SelectObject(dc, previous_pen);
}

void LoupeRenderer::render_toast(HDC dc, const LoupeFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    switch (frame.copy())
    {
    case CopyState::idle:
        return;
    case CopyState::copied:
        draw_check(dc, LoupeLayout::toast_icon(dpi), brush.color(PaletteRole::accent), dpi);
        SetTextColor(dc, brush.color(PaletteRole::accent));
        write_text(dc, LoupeLayout::toast_text(dpi), L"コピー",
                   font_of(L"Segoe UI", 12, FW_SEMIBOLD, dpi));
        return;
    case CopyState::failed:
        // 成功と同じ枠に収める。通知欄は 54 DIP しかないので語は 2 文字に収める。
        draw_cross(dc, LoupeLayout::toast_icon(dpi), brush.color(PaletteRole::warning), dpi);
        SetTextColor(dc, brush.color(PaletteRole::warning));
        write_text(dc, LoupeLayout::toast_text(dpi), L"失敗",
                   font_of(L"Segoe UI", 12, FW_SEMIBOLD, dpi));
        return;
    }
    std::unreachable();
}

void LoupeRenderer::render_value(HDC dc, const LoupeFrame &frame, UINT dpi, LoupeHitArea hover)
{
    const PaletteBrush brush(frame.palette());
    if (hover == LoupeHitArea::value && frame.has_color())
    {
        brush.fill(dc, LoupeLayout::hit_value(dpi), PaletteRole::surface);
        brush.outline(dc, LoupeLayout::hit_value(dpi), PaletteRole::line);
    }
    if (!frame.has_color())
    {
        brush.fill(dc, LoupeLayout::failure_bar(dpi), PaletteRole::warning);
        SetTextColor(dc, brush.color(PaletteRole::muted_text));
        write_text(dc, LoupeLayout::failure_text(dpi), frame.caption(),
                   font_of(L"Segoe UI", 13, FW_SEMIBOLD, dpi));
        return;
    }
    SetTextColor(dc, brush.color(PaletteRole::text));
    write_text(dc, LoupeLayout::value_text(dpi), frame.caption(),
               font_of(L"Consolas", 15, FW_BOLD, dpi));
    if (frame.copy() == CopyState::copied)
    {
        brush.fill(dc, LoupeLayout::copy_rule(dpi), PaletteRole::accent);
    }
}
} // namespace neneloupe
