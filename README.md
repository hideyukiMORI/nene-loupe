# NeNe Loupe

A tiny frameless screen loupe and colour picker for Windows. C++ / Win32, no UI library.

**Status: nothing is built yet.** This repository was initialised on 2026-09-06 under the
AYANE strict-repository policy; the rule set and gates go in before the first line of
production code. See [docs/todo/current.md](docs/todo/current.md) for what actually exists.

## What it will do

- A small, borderless, draggable window
- Left: a loupe magnifying the pixels around the mouse cursor
- Right: the colour code of the pixel at the loupe's centre; click to copy to clipboard
- Switch the display format (RGB decimal / HEX / CMYK / …)
- A gear icon opens a small settings modal: app colouring, copyright, version

That is all. The window size is deliberately not fixed yet; it will be adjusted after the
first working build.

## Why C++ and plain Win32

This is an experiment repository: the first purpose is to show that a strict, mechanically
enforced rule set can be applied to C++ (the sixth language after Kotlin, Go, Rust, C#, Java).
The tool itself is the second purpose. WinUI 3 was rejected for a widget this small; the reasons
are recorded in [ADR 0002](docs/adr/0002-plain-win32-no-ui-library.md).

## Documents

- [CLAUDE.md](CLAUDE.md) — handbook (Japanese), [AGENTS.md](AGENTS.md) — short entry (English)
- [SPECIFICATION.md](SPECIFICATION.md) — what to build (FR-NNN)
- [docs/](docs/) — constitution, coding rules, quality gates, ADRs

## Licence

MIT
