# ゲート発火の証明 — NeNe Loupe

> 記録: 2026-09-06 / Issue #1。以下は実行した結果のみ。
> 関連: QLT-007 / QLT-013 / ADR 0003

## 1. 環境と範囲

Windows x64。MSVC 19.44.35228、LLVM 19.1.5、CMake 3.31.6-msvc6、Ninja 1.12.1、Python 3.12.10。
これらは実測時の版であり、今後使う版の正本は `eng/tool-versions.json`。
製品コードは0。緑は検査基盤を証明し、ルーペ・DPI・複数モニタの動作を証明しない。

## 2. activeの規則

| 規則 | 最小の違反 | 検証経路 | 実測 |
| --- | --- | --- | --- |
| QLT-002 | 未使用変数・lint違反・整形違反 | eng/prove-gates.py / 実CMakeビルド・clang-format | C4101 / clang-tidy診断 / 整形診断で非0。各復帰は0 |
| QLT-004 | 一行に詰めたmain | eng/prove-gates.py / clang-format --dry-run --Werror | clang-format-violationsで非0。元の整形は0 |
| CNF-006 | 未定義ID・重複定義・状態不一致・証明行欠落・未置換値 | tests/conformance の document_checks 正例・反例 | 正例は指摘0、反例はCNF-006 |
| CNF-008 | Issue番号のないタスクコメント | tests/conformance の configuration_checks 正例・反例 | 番号付きは指摘0、番号なしはCNF-008 |

## 3. plannedの部分的な証明

| 規則 | 実測 |
| --- | --- |
| ARC-002 | CMakeの禁止依存でARC-002。元に戻すと構成・ビルドとも成功。字句検査でも逆方向includeと循環を拒否 |
| ARC-003 | coreのwindows.hを拒否。推移的includeまでは未保証 |
| ARC-007 | coreと製品テストのsystem_clockを拒否。adapters/win32では許可。文字列・コメントは検出しない |
| CPP-002 | 同じビルド経路の分岐漏れをC4062で拒否。元のソースは成功 |
| CPP-003 | メソッドを持つ公開メンバーをclang-tidyで拒否。aggregateだけの場合は漏れる |
| CPP-012 | 5引数をreadability-function-sizeで拒否。元のソースは成功 |
| CNF-001 | 禁止型名・別名・モジュール名を拒否。普通の型名は通る |
| CNF-002 | 複数型とファイル名不一致を拒否。主要宣言の完全な分類は未完了 |
| CNF-003 | waiverなし・Scope違い・ファイル単位の抑制を拒否。有効な行単位waiverは通る |
| CNF-004 | 期限切れ・必須項目欠落・不整合索引を拒否。期限当日と将来期限は通る |
| CNF-005 | 禁止設定ファイル名・/WX-を拒否。正常な設定は通る |
| CNF-007 | 設定参照の欠落・余分な設定を拒否。正常な参照は通る |
| GIT-003 | 日本語・Issue番号・破壊的変更フッタを検査。CIの実行証拠は未取得 |

## 4. 再現

```powershell
pwsh -NoProfile -File ./eng/measure-language.ps1
python -m unittest discover -s tests/conformance -v
pwsh -NoProfile -File ./eng/check.ps1
```

Phase 0の19結果は [phase0-results.json](phase0-results.json)。
実ツールの6反例と復帰結果はフルゲートから実行し、`out/proofs/results.json`に出力する。
反例は`out/proofs/`以下の使い捨てプロジェクトへ入れ、本体のソースは変更しない。
検査器の正例・反例はフルゲートから常に呼ぶ。
ローカルの単一ゲートは2026-09-06に終了コード0を確認した。
検査器は44テスト。C++のCTestは1件。製品のテスト件数には数えない。

## 5. GitHubと実機

2026-09-06にruleset `main-quality-gate`（ID 22345027）を適用し、APIで読み戻した。
PR必須、GitHub Actionsの`check`必須（integration 15368）、strict up-to-date、
force push・削除禁止、未解決レビューの解消を要求する。bypass actorは空。
リポジトリ設定はsquashのみを許可し、件名はPRタイトルを使う。

最初の [CI実行](https://github.com/hideyukiMORI/nene-loupe/actions/runs/33981391008) は
PATH上の別CMake（3.31.6）を拾い、QLT-011で失敗した。版の比較を緩めず、
Visual Studio同梱のCMake/Ninjaをtoolchain.ps1で明示的に選択するよう修正した。
修正後の [CI実行](https://github.com/hideyukiMORI/nene-loupe/actions/runs/33981539645) は
コミット `55e6116` で成功した。ローカルと同じ単一コマンドで44テスト、CTest、6反例と復帰を実行した。
Ready後のhead更新を自動でDraftへ戻す処理や、Git規約の全条件の反例証明は未実装なので、
GITとQLT-012の規則全体はplannedを維持する。
表示・DPI・複数モニタ・クリップボードの実機検証は未実施。
