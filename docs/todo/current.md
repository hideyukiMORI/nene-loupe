# いまのタスク — NeNe Loupe

> GitHub Issue が正。ここは要約であり、Markdown のチェックリストをタスク状態として扱わない。
> 更新は実測でだけ行う（「動くもの」は `pwsh -NoProfile -File ./eng/check.ps1` が通ったもの）。

## 段階

| 段階 | 状態 |
| --- | --- |
| リポジトリ初期化（雛形・仕様・ADR 0001/0002） | ✅ 2026-09-06 |
| Phase 0 言語の実測（C++ で M-1〜M-8） | 🔲 次 |
| Phase 1 文書とゲートの足場（CMake・`/W4 /WX`・clang-tidy・clang-format・`eng/check.ps1`・規約検査） | 🔲 |
| Phase 2 negative proof | 🔲 |
| Phase 3 縦切り 1 本（枠なし窓が出て、カーソル下の 1 ピクセルの HEX を表示する） | 🔲 |

## 動くもの

（まだ無い。production コードは 0 行）

## 動かないもの

（まだ無い）

## 埋まっていないプレースホルダ

雛形の `{{TOOL}}` `{{CONFORMANCE}}` `{{VERSION_FILE}}` `{{BUILD_GRAPH_FILE}}` `{{M1}}`〜`{{M7}}` 等は
Phase 0 / 1 の実測で埋める。**埋めるまで対応する規則は planned のまま**。

## 次の 1 手

Phase 0: `_work/reports/ayane-strict-repo-policy/INIT_PROCEDURE.md` に従い、MSVC（`/std:c++latest` `/W4 /WX /permissive-`）と
clang-tidy で M-1〜M-8 を実測し、ADR 0001 の空欄を埋める。
