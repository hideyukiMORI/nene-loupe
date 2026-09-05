#include "SettingsRenderer.hpp"

#include "LoupeLayout.hpp"
#include "LoupeRenderer.hpp"
#include "SettingsLayout.hpp"

#include <array>
#include <memory>
#include <type_traits>

// 版はビルド時に 1 か所（CMake の project VERSION）から入る。既定値は置かない（FR-011）。
#ifndef NENELOUPE_VERSION
#error "NENELOUPE_VERSION must be defined by the build"
#endif

namespace neneloupe
{
namespace
{
constexpr std::array<const wchar_t *, SettingsLayout::theme_choices> theme_names{
    L"ダーク", L"ライト", L"システムに従う"};

int index_of(Theme theme)
{
    switch (theme)
    {
    case Theme::dark:
        return 0;
    case Theme::light:
        return 1;
    case Theme::system:
        return 2;
    }
    std::unreachable();
}

void draw_disc(HDC dc, RECT bounds, COLORREF fill, COLORREF edge)
{
    SetDCBrushColor(dc, fill);
    SetDCPenColor(dc, edge);
    const auto previous_brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    const auto previous_pen = SelectObject(dc, GetStockObject(DC_PEN));
    Ellipse(dc, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SelectObject(dc, previous_brush);
    SelectObject(dc, previous_pen);
}
} // namespace

void SettingsRenderer::render(HDC dc, const SettingsFrame &frame, UINT dpi)
{
    const auto bounds = SettingsLayout::window(dpi);
    const auto delete_dc = [](HDC memory) { DeleteDC(memory); };
    std::unique_ptr<std::remove_pointer_t<HDC>, decltype(delete_dc)> memory(CreateCompatibleDC(dc),
                                                                            delete_dc);
    const auto delete_bitmap = [](HBITMAP bitmap) { DeleteObject(bitmap); };
    std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(delete_bitmap)> bitmap(
        memory ? CreateCompatibleBitmap(dc, bounds.right, bounds.bottom) : nullptr, delete_bitmap);
    const auto previous = bitmap ? SelectObject(memory.get(), bitmap.get()) : nullptr;
    if (!previous || previous == HGDI_ERROR)
    {
        PaletteBrush(frame.palette()).fill(dc, bounds, PaletteRole::background);
        return;
    }
    render_content(memory.get(), frame, dpi);
    BitBlt(dc, 0, 0, bounds.right, bounds.bottom, memory.get(), 0, 0, SRCCOPY);
    SelectObject(memory.get(), previous);
}

void SettingsRenderer::render_content(HDC dc, const SettingsFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    const auto bounds = SettingsLayout::window(dpi);
    brush.fill(dc, bounds, PaletteRole::background);
    brush.outline(dc, bounds, PaletteRole::border);
    SetTextColor(dc, brush.color(PaletteRole::text));
    LoupeRenderer::write_text(dc, SettingsLayout::title(dpi), L"設定",
                              LoupeRenderer::font_of(L"Segoe UI", 14, FW_BOLD, dpi));
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::close_glyph(dpi), L"×",
                              LoupeRenderer::font_of(L"Segoe UI", 14, FW_NORMAL, dpi));
    brush.fill(dc, SettingsLayout::divider(48, dpi), PaletteRole::line);
    render_theme(dc, frame, dpi);
    brush.fill(dc, SettingsLayout::divider(180, dpi), PaletteRole::line);
    render_layer(dc, frame, dpi);
    brush.fill(dc, SettingsLayout::divider(266, dpi), PaletteRole::line);
    render_about(dc, frame, dpi);
}

void SettingsRenderer::render_theme(HDC dc, const SettingsFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::theme_label(dpi), L"テーマ",
                              LoupeRenderer::font_of(L"Segoe UI", 12, FW_BOLD, dpi));
    const int selected = index_of(frame.theme());
    for (int index = 0; index < SettingsLayout::theme_choices; ++index)
    {
        const bool on = index == selected;
        const auto circle = SettingsLayout::theme_radio(index, dpi);
        draw_disc(dc, circle, brush.color(PaletteRole::surface),
                  brush.color(on ? PaletteRole::accent : PaletteRole::line));
        if (on)
        {
            RECT dot = circle;
            InflateRect(&dot, -LoupeLayout::scale(4, dpi), -LoupeLayout::scale(4, dpi));
            draw_disc(dc, dot, brush.color(PaletteRole::accent), brush.color(PaletteRole::accent));
        }
        SetTextColor(dc, brush.color(PaletteRole::text));
        LoupeRenderer::write_text(
            dc, SettingsLayout::theme_text(index, dpi),
            theme_names[static_cast<std::size_t>(index)],
            LoupeRenderer::font_of(L"Segoe UI", 13, on ? FW_SEMIBOLD : FW_NORMAL, dpi));
    }
}

void SettingsRenderer::render_layer(HDC dc, const SettingsFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::window_label(dpi), L"ウィンドウ",
                              LoupeRenderer::font_of(L"Segoe UI", 12, FW_BOLD, dpi));
    SetTextColor(dc, brush.color(PaletteRole::text));
    LoupeRenderer::write_text(dc, SettingsLayout::topmost_text(dpi), L"常に最前面",
                              LoupeRenderer::font_of(L"Segoe UI", 13, FW_NORMAL, dpi));
    const auto layer = frame.layer();
    const bool on = layer == WindowLayer::topmost;
    const auto track = SettingsLayout::switch_track(dpi);
    SetDCBrushColor(dc, brush.color(on ? PaletteRole::accent : PaletteRole::surface));
    SetDCPenColor(dc, brush.color(on ? PaletteRole::accent : PaletteRole::line));
    const auto previous_brush = SelectObject(dc, GetStockObject(DC_BRUSH));
    const auto previous_pen = SelectObject(dc, GetStockObject(DC_PEN));
    const auto radius = LoupeLayout::scale(20, dpi);
    RoundRect(dc, track.left, track.top, track.right, track.bottom, radius, radius);
    SelectObject(dc, previous_brush);
    SelectObject(dc, previous_pen);
    const auto knob_color = brush.color(on ? PaletteRole::on_accent : PaletteRole::muted_text);
    draw_disc(dc, SettingsLayout::switch_knob(layer, dpi), knob_color, knob_color);
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::topmost_help(dpi),
                              L"オフにすると通常の重なり順になります。",
                              LoupeRenderer::font_of(L"Segoe UI", 11, FW_NORMAL, dpi));
}

void SettingsRenderer::render_about(HDC dc, const SettingsFrame &frame, UINT dpi)
{
    const PaletteBrush brush(frame.palette());
    const auto body = LoupeRenderer::font_of(L"Segoe UI", 12, FW_NORMAL, dpi);
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::about_label(dpi), L"このアプリについて",
                              LoupeRenderer::font_of(L"Segoe UI", 12, FW_BOLD, dpi));
    SetTextColor(dc, brush.color(PaletteRole::text));
    LoupeRenderer::write_text(dc, SettingsLayout::about_version(dpi),
                              std::wstring(L"NeNe Loupe  版 ") + NENELOUPE_VERSION, body);
    SetTextColor(dc, brush.color(PaletteRole::muted_text));
    LoupeRenderer::write_text(dc, SettingsLayout::about_copyright(dpi),
                              L"© 2026 Hideyuki Mori — MIT License", body);
    LoupeRenderer::write_text(dc, SettingsLayout::about_cmyk(dpi),
                              L"CMYK は ICC を使わない素朴換算です。", body);
    if (frame.status() == SettingsStatus::ok)
    {
        return;
    }
    SetTextColor(dc, brush.color(PaletteRole::warning));
    LoupeRenderer::write_text(dc, SettingsLayout::status_text(dpi), frame.status_text(),
                              LoupeRenderer::font_of(L"Segoe UI", 12, FW_SEMIBOLD, dpi));
}
} // namespace neneloupe
