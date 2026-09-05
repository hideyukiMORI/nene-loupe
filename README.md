# NeNe Loupe

A tiny frameless screen loupe and colour picker for Windows. C++ / Win32, no UI library.

**Status: first working slice.** A 160×64 DIP window shows a sharp 8× loupe of the
7×7 pixels around the pointer and the centre colour in HEX. Drag anywhere to move it;
press Esc or Alt+F4 to close. It stays above other windows. See
[current work](docs/todo/current.md) for the implementation and measured limits.

On Windows, install the versions in `eng/tool-versions.json` (Visual Studio Build Tools with
C++ tools, LLVM and CMake, plus Python), then run:

```powershell
pwsh -NoProfile -File ./eng/bootstrap.ps1
pwsh -NoProfile -File ./eng/check.ps1
./build/NeNeLoupe.exe
```

Bootstrap enables this repository's Git hooks. The full gate builds the application,
runs OS-independent unit tests and enforces 90% core/application branch coverage with LLVM.
Interactive Windows checks are recorded separately in [gate proofs](docs/quality/gate-proofs.md).
The status title exposes the current HEX; capture failures clear stale pixels and show a message.

## Remaining features

- Click the colour to copy to the clipboard
- Switch the display format (RGB decimal / HEX / CMYK / …)
- A gear icon opens a small settings modal: app colouring, copyright, version

The initial window size may still be adjusted. This build has no clipboard, format switching,
settings or persistence. It samples the visible desktop, including its own window when the
pointer is over it. HDR and ICC colour management are outside the specification.

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
