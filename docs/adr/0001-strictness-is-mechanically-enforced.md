# ADR 0001 — 厳格さは機械で強制する

- 状態: 受理
- 日付: 2026-09-06
- Issue: #1
- 影響する規則: すべて

## 文脈

本リポジトリの規約は、AYANE 厳格規約ポリシー（`/home/xi/docker/_work/reports/ayane-strict-repo-policy/POLICY.md`）と、
先行する 5 リポジトリ（NENE-PIXEL＝Kotlin / nene-recall＝Go / xi-tools＝Rust / NeNeCommander＝C# / NeNeClock＝Java）で
確立された考え方を C++ (C++23, MSVC) へ持ち込んだものである。共通する中核は 1 つに要約できる。

> **一つのことを実現する方法を 1 つに固定し、そのことを人の記憶ではなく機械に守らせる。**

規約は、破ったときに何も起きなければ、時間とともに必ず腐る。

### C++ (C++23, MSVC) で実測したこと

```
M1-w4: exit 0（分岐漏れを拒否しない）
M1-exhaustive: exit 2 / C4062
M1-default: exit 2 / C4061
M2-private / M4-private: exit 2 / C2248
M3-uninitialized: exit 2 / C4700
M2-tidy-aggregate-hole: exit 0
M2-tidy: exit 1 / misc-non-private-member-variables-in-classes
M5-time / M6-win32 / M7-suppression / M8-file-suppression: exit 0
```

| 先行事例の規則 | C++ (C++23, MSVC)（実測） |
| --- | --- |
| 不正状態を表現不能に・網羅性 | 不十分。明示的なC4061/C4062で分岐漏れを拒否。範囲外enumは書ける |
| 公開状態は不変 | 不十分。private代入は拒否。公開aggregateはコンパイラ・採用lintとも通る |
| `null` の意味は一つ（ゼロ値・未初期化の穴） | 不十分。未初期化は拒否、nullptrは通る |
| 非公開コンストラクタ＋唯一のファクトリ（迂回経路） | privateはC2248。検証済み値のコピーは可能。不正な低水準操作を完全には塞がない |
| 中核の決定性を構文的に塞げるか | 言語では無い。clock呼出しは成功。字句検査で一部を補う |
| 依存方向をビルドが拒むか（標準同梱の枠組みの漏れ） | CMakeの宣言・実グラフの照合で補う。windows.hは言語だけでは拒否しない |
| 抑制を言語で封じられるか | 無い。pragmaで/we4062を抑制可能 |
| baseline を作る経路が塞がっているか | 言語では無い。ファイル全体の警告抑制が可能。CNFで補う |

**そして、いま実装が空である。** 規則を後から被せる場合に必要になる baseline が、今は要らない。
違反ゼロから始められる時点は今しかない。

## 決定

**すべての規範規則に「機械強制の状態」を持たせ、その状態を文書の側で機械検査する。**

1. 規則の正本を `docs/ARCHITECTURE_CONSTITUTION.md` / `docs/CODING_RULES.md` / `docs/QUALITY_GATES.md` に置く。
   すべての規則に ID を与え、状態を **active / planned / 不能 / 不採用** で明示する。**planned を active と書かない**
2. `pwsh -NoProfile -File ./eng/check.ps1` を唯一の入口とする。CI は同じコマンドを呼ぶだけにし、CI 側にしか無い検査を作らない
3. baseline を持たない。例外は期限付きの狭い waiver だけ
4. 抑制には理由と規則 ID を要求する（台帳を持つ。C++は抑制を言語で封じられない）
5. ゲートを弱める変更は ADR を要する
6. 道具の版は eng/tool-versions.json だけが決める。2 か所に書かない
7. **ゲートは意図的な違反で発火することを証明してから active と書く**（`docs/quality/gate-proofs.md`）
8. 規約検査（`eng/conformance.py`）は依存ゼロで書く

## 強制

- **planned**: CNF-006（文書整合）・CNF-005（baseline 禁止）・CNF-004（waiver 期限）。実装後に active へ
- **不能**: 「`planned` を勝手に `active` と書き換えないこと」そのもの。マトリクスの行と実装の対応は、いまは人が見るしかない

## 結果

得られるもの:

- 規約が自分について嘘をつく経路が塞がる
- 新しい貢献者（人でも AI でも）が「何がいま守られているか」を 1 か所で読める
- 規約を緩めるコストが上がる

失うもの:

- 文書のメンテナンスコスト。規則を足すたびに 3 か所（本文・マトリクス・実装）が同期を要求される
- 検査の実装そのものの保守
- 書き味の制約: Win32の生のハンドルとメッセージを、中核の閉じた値型へ変換する境界が必要

**正直に記録しておくこと:**

- 🔴 不正なenumキャスト、nullptrの意味、可変参照の流出、マクロ・間接呼出しを含む完全な決定性は保証できていない。対応する規則全体はplanned
- **本 ADR の厳格さは、実装がまだ空である今だからこそ無傷で導入できた。** 緑であることは、規則が良いことの証明ではない。
  検査対象がまだ小さいことの結果でもある。実装で規則が邪魔になったとき、緩めるのではなく **ADR で判断を残すこと**が本 ADR の眼目である

## 却下した選択肢

| 選択肢 | 却下の理由 |
| --- | --- |
| 散文の規約だけを置く | 守られているかを確かめる手段が無く、時間とともに必ず腐る。**本 ADR の出発点そのもの** |
| 既製 lint の既定セットだけで済ませる | 既製ルールは「C++ (C++23, MSVC) として危ういこと」を見るが、「NeNe Loupe として守るべきこと」は見られない。規則と lint の対応が付かず「なぜ有効か」を説明できない |
| 全 lint の一括有効化 | 相互に矛盾する lint が同時に入り、規則ではなく道具の機嫌に従うことになる |
| baseline を作って既存違反を後回しにする | 新規リポジトリなので既存違反が無い。ここで baseline を許すと、以後の違反も同じ入口から入ってくる |
| 実装が入ってから導入する | 既存違反を凍結する baseline が必要になる。今なら違反ゼロで始められる |
| 規約を `CLAUDE.md` にだけ書く | 人間と AI の遵守に依存する。前例（nene-recall）で実証済みの失敗——規約は CLAUDE.md にあったが、守っていたのはテスト 10 ケースだけで、新しく書かれるコードには及んでいなかった |
| 規則 ID を付けず散文で参照する | 「どの規則の話か」がレビューのたびに揺れる。ID があると tooling・ADR・PR・waiver が同じ語を指せる |
| `/W4 /WX`だけで列挙型の網羅性を保証する | M1-w4が成功した。C4061/C4062の明示指定が必要 |

## 関連

- 先行: NENE-PIXEL `docs/ADR`、nene-recall ADR 0010、xi-tools ADR 0001、NeNeCommander ADR-0001、NeNeClock ADR 0001
- ポリシー: `/home/xi/docker/_work/reports/ayane-strict-repo-policy/`

## 実測の追記（Issue #1）

空欄は2026-09-06の実行結果で記入した。受理済みの判断は変更していない。
詳細な道具選定と限界は [ADR 0003](0003-cpp-foundation-and-measured-limits.md)。
再現: `pwsh -NoProfile -File ./eng/measure-language.ps1`。全19結果は
[phase0-results.json](../quality/phase0-results.json) に保存した。
