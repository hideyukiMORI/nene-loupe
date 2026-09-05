# ADR 0003 — C++ の検査基盤と実測できた限界を固定する

- 状態: 受理
- 日付: 2026-09-06
- Issue: #1
- 影響する規則: ARC-002 / ARC-003 / ARC-007 / CPP-002 / CPP-003 / CPP-012 / CPP-015 / QLT-001 / QLT-007 / QLT-011 / CNF-001 / CNF-006

## 文脈

Phase 0 の MSVC 19.44.35228 / clang-tidy 19.1.5 の実測は
`eng/measure-language.ps1` と `eng/probes/language.json` から再現する。
通常の `/W4 /WX` は列挙子の分岐漏れを拒否しない。`/we4061 /we4062` は拒否するが、
`#pragma warning(disable: 4062)` で迂回できる。
clang-tidy の公開メンバー検査はメソッドを持たない aggregate を拒否しない。
型の意味や可変参照の流出まで検証したとは主張できない。

初期雛形には検査未実装のまま「成功した」と書かれた証明記録、未置換の道具名、
同じアダプタの重複があった。Issue #1 で文書の状態と実体をそろえる。

## 決定

**MSVC + CMake/Ninja + LLVM と、Python 標準ライブラリだけの規約検査を単一 PowerShell ゲートに結ぶ。**

- MSVC は C++23 の利用可能な実装として `/std:c++latest /W4 /WX /permissive- /we4061 /we4062` を使う。
  `latest` は将来の言語機能も含み得るため、道具の版を `eng/tool-versions.json` で固定する。
- 整形の正本は `.clang-format`、lint の正本は `.clang-tidy`。一括の lint 群は採用しない。
  関数の認知的複雑度10、長さ60行、ネスト3、引数4を検査する。
- モジュールの許可グラフは `eng/architecture.json`。実ターゲットは具体的な責務とソースがある時だけ作る。
  今回作る C++ ターゲットは検査基盤のスモークテストだけで、製品モジュールは作らない。
- 規約検査は `eng/conformance.py`。検出設定は `eng/conformance-rules.json` に分離する。
  C++ ソースの字句検査は AST / 全てのマクロ展開・別名解決の代わりではない。意味的な規則全体は planned に残す。
- 検査器のテストは意図的な違反を含むため規約検査から除くが、製品テストへの決定性の検査は外さない。
  生成物は `build/` と `out/` にだけ置く。検証対象の選択は Git の追跡ファイルと未追跡・非無視ファイルから行う。
- C++ は抑制を言語で封じられないため waiver 台帳を持つ。行単位の `NOLINTNEXTLINE` は
  直前の有効な waiver と明示的な検査名が必要。ファイル単位の pragma / NOLINTBEGIN は認めない。
- 中核の分岐カバレッジ目標は90%。製品コードがない今回は測定・閾値強制とも未導入で QLT-009 は planned。
  MSVC 向けカバレッジ道具は最初の中核実装の Issue で実測して選ぶ。
- UI と application の状態遷移は単一の UI スレッドで行う。背景スレッドの追加には新しい ADR が必要。
- ADR 0001 の判断は維持する。Issue #1 が明示的に要求した実測欄の記入だけを行う。
  受理済み判断の変更は今後も新しい ADR に記録する。

## 強制

規則ごとの正本は `docs/QUALITY_GATES.md`。自作検査は正例・反例の自動テストをゲートに結び、
証拠を `docs/quality/gate-proofs.md` に残す。CI / ruleset は実際に反映・確認するまで planned。

## 結果

Windows の道具と既定ロケール・環境に触れる検証スクリプトは開発時の道具であり、製品の
ARC-007 の区画には含めない。検査器自身は日付を引数でも受け、期限のテストを固定できる。
検査スクリプトの依存は Python / PowerShell 標準機能のみ。実行時依存は0。
緑は検査基盤の正常性を示し、ルーペが動作することは示さない。

## 却下した選択肢

| 選択肢 | 却下の理由 |
| --- | --- |
| `/W4 /WX` だけ | M1-w4 が成功し、分岐漏れを検出しなかった |
| 公開メンバー lint だけで CPP-003 を active とする | M2-tidy-aggregate-hole が成功した。可変参照の流出も対象外 |
| 正規表現の API 検査だけで決定性を完全に保証したとする | マクロや別名、間接呼び出しを網羅しない |
| 未実装の層を空のライブラリで作る | 具体的な責務がない将来用モジュールになる |
| 全 lint の有効化 | 規則ごとの責務と採用理由が失われる |
| カバレッジ100%と記録する | 中核の分岐も測定器もまだ存在しない |

## 参考

- [MSVC C4061](https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4061?view=msvc-170)
- [clang-tidy function-size](https://clang.llvm.org/extra/clang-tidy/checks/readability/function-size.html)
- [clang-tidy non-private members](https://clang.llvm.org/extra/clang-tidy/checks/misc/non-private-member-variables-in-classes.html)
