# NeNe Loupe

A tiny frameless screen loupe and colour picker for Windows. C++ / Win32, no UI library.

**Status: verification foundation; no product implementation yet.** The repository has a
CMake/MSVC smoke test, clang-tidy/clang-format checks, standard-library-only conformance tests,
and reproducible negative proofs. See [current work](docs/todo/current.md) for measured status.

On Windows, install the versions in `eng/tool-versions.json` (Visual Studio Build Tools with
C++ tools, LLVM and CMake, plus Python), then run:

```powershell
pwsh -NoProfile -File ./eng/bootstrap.ps1
pwsh -NoProfile -File ./eng/check.ps1
```

Bootstrap enables this repository's Git hooks. The full gate checks the foundation;
it does not demonstrate a working loupe. CI and branch protection are only considered enforced
after remote verification is recorded in [gate proofs](docs/quality/gate-proofs.md).

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
