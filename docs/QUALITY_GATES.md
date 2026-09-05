# 品質ゲート — NeNe Loupe

> Status: normative（規範）/ 2026-09-06 初版
> 本書は「いま何が機械で守られているか」の**正本**である。
> 規範の本文は [ARCHITECTURE_CONSTITUTION.md](ARCHITECTURE_CONSTITUTION.md) と
> [CODING_RULES.md](CODING_RULES.md)、機械側の実体はこの文書に対応する。

`pwsh -NoProfile -File ./eng/check.ps1` がローカルと CI の**唯一の完了定義**である。
個別の道具は診断のために単独で回してよいが、その成功は完了の代わりにならない。

---

## 1. ゲートの整合性規則（QLT-0xx）

### QLT-001 — ゲートは一つ

ローカルの検証と CI は**同じ 1 つのコマンド**（`pwsh -NoProfile -File ./eng/check.ps1`）を呼ぶ。CI のワークフローに品質判断のロジックを書かない。
第 2 の完了定義を文書化しない。道具の版は 1 か所（{{VERSION_FILE}}）にだけ書く。

- 機械強制: **planned**（CI が `pwsh -NoProfile -File ./eng/check.ps1` を呼ぶだけであること）

### QLT-002 — 警告は失敗する

コンパイラの警告・整形差分・静的解析の指摘はすべて失敗にする。重大度の引き下げは禁止。

- 機械強制: **planned**

### QLT-003 — baseline を作らない

lint・静的解析・アーキテクチャ・依存・テストのいずれについても baseline を持たない。既存の違反を「無かったこと」にする経路を作らない。
**唯一の例外機構は期限付きの狭い waiver である。**

- 機械強制: **planned** → CNF-005

### QLT-004 — 整形は検査であって修復ではない

整形は設定から決定的に決まる。CI がソースを書き換えて成功させることはしない。整形の正本は 1 つ（設定ファイルは置かないか 1 つだけ）。

- 機械強制: **planned**

### QLT-005 — ローカルと CI は同一

CI で必要なゲートは、すべてローカルで実行できる。検査を足したくなったら、まずローカルのゲートに足す。

- 機械強制: **planned**

### QLT-006 — アーキテクチャは実行可能

モジュール境界・import 規律・状態の経路は機械が検査する。散文だけのアーキテクチャ規則は、**「不能」と明記されない限り未完成**として扱う。

- 機械強制: **planned**

### QLT-007 — カスタムゲートには negative proof が要る

自作の検査は、「わざと違反させた入力で、意図した規則によって落ちること」と「正しい入力では通ること」の両方を示すまで信用しない。
証拠は [quality/gate-proofs.md](quality/gate-proofs.md) に記録する。ゲートを変えたら証拠も同じ変更で更新する。

🔴 **設定が在ることは、検査が効いていることの証明ではない。** 前例で 2 度、設定だけあって検査が効いていない状態が証明手順で発覚した。

- 機械強制: **planned**（検査器自身の正例・反例テストをゲートに結線する）

### QLT-008 — 振る舞いの変更はテストを伴う

振る舞いを変えたら、いちばん狭い安定した境界にテストを足す。不具合の修正は、可能なら**先に落ちる回帰テスト**を書いてから直す。

- 機械強制: **planned**（レビュー事項。カバレッジ下限＝QLT-009 が部分的に代替する）

### QLT-009 — カバレッジは下げられない

中核の分岐カバレッジ下限を {{COVERAGE_MIN}}% とする。閾値は上げてよいが下げてはならない。下げるには ADR が要る。
**置いていない層は「置いていない」と書く。**

- 機械強制: **planned**

### QLT-010 — ゲートの弱体化はアーキテクチャ変更

重大度・除外・閾値・モジュール境界・必須 CI ジョブを変えるには明示的な根拠が要る。MUST 規則を弱める変更には ADR か waiver が要る。
**無関係な作業を通すために検査を切ることは禁止。**

- 機械強制: **不能**（判断そのものが対象。PR の手続きで担保する）

### QLT-011 — 依存は再現可能

依存の版は 1 か所にだけ書き、lock ファイルのドリフトはビルドを落とす。

- 機械強制: **planned**

### QLT-012 — 検証の頻度は変更に従う

反復中は最も狭い検査を使う。**フルゲートは PR の Draft → Ready の時点で必ず通す。**
CI の起動条件は `ready_for_review`（＋非 draft の `synchronize` / `edited`）。draft の間にフルゲートを回さない。
head が動いたら Draft に戻して再度 Ready にする。古い成功 SHA・スキップされたジョブ・狭い検査は、通ったフルゲートの代わりにならない。

- 機械強制: **planned**（ruleset の必須 check と strict up-to-date）

### QLT-013 — 環境依存の主張は正直に名付ける

単体テストは表示サーバ・実機・実 OS 資源を必要としない。単体テストが通ったことを「実機で動く」の証拠として扱わない。
表示を伴う確認は [quality/gate-proofs.md](quality/gate-proofs.md) に環境と手順を書いて別に記録する。

- 機械強制: **planned**

---

## 2. 規約検査の規則（CNF-0xx）— `{{CONFORMANCE}}`

汎用の lint が見ないもの、つまり**このリポジトリ固有の規約**を検査する。依存ゼロで書き、ゲートから常に呼ぶ。
各規則には正例・反例の単体テストを付け、それもゲートに結線する（QLT-007）。

<!-- 最初に入れる 5 つ。実装したら状態を active にし、gate-proofs に行を足す。 -->

| ID | 内容 | 規則 | 状態 |
| --- | --- | --- | --- |
| `CNF-001` | 禁止された総称名（型名の語尾・パッケージ名） | CPP-010 | planned |
| `CNF-002` | 1 ファイル 1 主要宣言 | CPP-011 | planned |
| `CNF-003` | 抑制には waiver ID が要る（台帳を持つ言語のみ） | CPP-015 | planned |
| `CNF-004` | waiver 台帳の整合（命名・必須項目・索引・**期限切れで落とす**） | CPP-015 | planned |
| `CNF-005` | baseline とゲート無力化の禁止（`baseline` / `suppressions` / `ignoreFailures` 等） | QLT-003 | planned |
| `CNF-006` | 文書整合（規則 ID の重複・未定義参照・マトリクスに行の無い規則） | ADR 0001 | planned |
| `CNF-007` | 宣言した検査設定が実際にビルドから読み込まれていること | QLT-007 | planned |
| `CNF-008` | `TODO` / `FIXME` には Issue 番号 | — | planned |

🔴 **検出語は検査器のソースに直書きしない**（検査器が自分自身を違反として報告する。前例: xi-tools 初版で 7 件の自己検出）。
🔴 **テストソースは検査対象から外す**（テストは意図的な違反を書く場所）。

---

## 3. 強制マトリクス

規範の各規則が、いまどの層で守られているか。**この表が実装と食い違ったら merge を止める。**
CNF-006 が「本文に定義があるのにここに行が無い」を拒否する。

| 規則 | 状態 | 機械強制の実体 |
| --- | --- | --- |
| ARC-001 | planned | レビュー事項（CNF-001 が温床を減らす） |
| ARC-002 | planned | |
| ARC-003 | planned | |
| ARC-004 | planned | |
| ARC-005 | planned | |
| ARC-006 | planned | |
| ARC-007 | planned | |
| ARC-008 | planned | レビュー事項 |
| ARC-009 | planned | |
| ARC-010 | planned | |
| ARC-011 | planned | |
| ARC-012 | 不能 | QLT-010 の手続き |
| CPP-001 | planned | |
| CPP-002 | planned | |
| CPP-003 | planned | |
| CPP-004 | planned | |
| CPP-005 | planned | |
| CPP-006 | planned | |
| CPP-007 | planned | |
| CPP-008 | planned | |
| CPP-009 | planned | |
| CPP-010 | planned | CNF-001 |
| CPP-011 | planned | CNF-002 |
| CPP-012 | planned | |
| CPP-013 | planned | |
| CPP-014 | planned | |
| CPP-015 | planned | CNF-003 ＋ CNF-004 |
| QLT-001 | planned | |
| QLT-002 | planned | |
| QLT-003 | planned | CNF-005 |
| QLT-004 | planned | |
| QLT-005 | planned | |
| QLT-006 | planned | |
| QLT-007 | planned | |
| QLT-008 | planned | レビュー事項 |
| QLT-009 | planned | |
| QLT-010 | 不能 | PR の手続き |
| QLT-011 | planned | |
| QLT-012 | planned | |
| QLT-013 | planned | |
| CNF-001 〜 CNF-008 | planned | `{{CONFORMANCE}}` |

---

## 4. `pwsh -NoProfile -File ./eng/check.ps1` に入っている層

| 層 | 目的 | 実体 |
| --- | --- | --- |
| コンパイル | 型安全・網羅性・警告ゼロ | {{…}} |
| 整形 | 文字列としての正本 | {{…}} |
| API 禁止 | シンボル単位の禁止 | {{…}} |
| 静的解析 | 構造と複雑度 | {{…}} |
| アーキテクチャ | レイヤ・パッケージ境界 | {{…}} |
| 規約検査 | NeNe Loupe 固有 | `{{CONFORMANCE}}` |
| 検査自身のテスト | 規約検査の正例・反例 | {{…}} |
| 単体テスト | 振る舞い | {{…}} |
| カバレッジ | 中核の検証密度 | {{…}} |
| 依存 | 再現性 | {{…}} |

---

## 5. マージゲート

`main` は次を必須とする。

- Pull Request 経由であること
- `pwsh -NoProfile -File ./eng/check.ps1` が成功していること（CI の必須 check）
- 生成物のドリフトが無いこと（作業ツリーが汚れないこと）
- 期限切れ waiver が無いこと
- 未解決のレビュー指摘が無いこと
- squash merge のみ

`main` への直接 push・force push・ブランチ削除は禁止する。

🔴 **リポジトリ設定（ruleset）を適用したら、その事実を [quality/gate-proofs.md](quality/gate-proofs.md) に記録する。**
**設定していないものを「必須になっている」と書かない。**
