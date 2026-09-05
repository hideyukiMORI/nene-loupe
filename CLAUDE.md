# CLAUDE.md — NeNe Loupe

Claude Code / AI エージェントがこのリポジトリで作業するための**中核ハンドブック**。
簡潔な英語版の入口は [AGENTS.md](AGENTS.md)。詳細の正本は `docs/` にあり、ここには複製しない。

---

## 0. まず読むもの（production コードに触れる前に必ず）

0. [SPECIFICATION.md](SPECIFICATION.md) — 何を作るか（FR-NNN）
1. [docs/ARCHITECTURE_CONSTITUTION.md](docs/ARCHITECTURE_CONSTITUTION.md) — 憲章（ARC-NNN）
2. [docs/PROJECT_LAYOUT.md](docs/PROJECT_LAYOUT.md) — モジュールと依存方向
3. [docs/CODING_RULES.md](docs/CODING_RULES.md) — C++ (C++23, MSVC) 規約（CPP-NNN）
4. [docs/QUALITY_GATES.md](docs/QUALITY_GATES.md) — **いま何が機械で守られているか**（QLT-NNN / CNF-NNN）
5. [docs/DEVELOPMENT_WORKFLOW.md](docs/DEVELOPMENT_WORKFLOW.md) — 手順
6. [docs/COMMIT_CONVENTIONS.md](docs/COMMIT_CONVENTIONS.md) — Issue・ブランチ・コミット・PR（GIT-NNN）
7. [docs/GLOSSARY.md](docs/GLOSSARY.md) — 用語
8. 該当する ADR（`docs/adr/`）と有効な waiver（`docs/waivers/`）

---

## 1. このリポジトリの統治原則

> **一つのことを実現する方法を 1 つに固定し、そのことを人の記憶ではなく機械に守らせる。**

その帰結として、次の 3 つを常に守る。

1. **正典の経路を先に特定してから編集する。** 「ここで書いたほうが早いから」で第 2 の経路を作らない（ARC-001 / ARC-012）
2. **ゲートを弱めて通さない。** 検査が落ちたらコードを直す。閾値・除外・重大度を触るのは ADR 相当の判断（QLT-010）
3. **`planned` を `active` と書かない。** 未実装の強制を実装済みに見せるのは、この規約体系で唯一「壊す」行為（[ADR 0001](docs/adr/0001-strictness-is-mechanically-enforced.md)）

---

## 2. このプロジェクトで間違えやすい所

<!-- 縦切り 1 本を通したら、実際に踏んだものをここに足す。前例（NeNeClock）の形:
### 現在時刻を読む場所は 1 つしかない
`now()` 系は **adapters/win32 以外では書けない**（ゲートが落とす）。時刻が要ればポートを注入する。**テストソースにも同じ禁止がかかる。**
### 網羅性検査を殺す `default` / `else` / `_` を書かない
選択肢が増えたらコンパイルが落ちるのが正しい状態。
### 期待される失敗は例外にしない
閉じた結果型で返す。広い catch はゲートが拒否する。
-->

### 現在時刻を読む場所は 1 つしかない

現在時刻・乱数・既定ロケール・環境変数を読んでよいのは **adapters/win32** だけである（ARC-007）。
中核で必要なら、型のあるポートから注入する。**テストが実時刻を読むことも決定性の破壊である。**

### 網羅性検査を殺す分岐を書かない

閉じた選択肢の分岐に `default` / `else` / `_` を書かない。選択肢が増えたらコンパイルが落ちるのが正しい状態（CPP-002）。

### 期待される失敗は例外にしない

検証エラー・見つからない・拒否・非互換は閉じた結果型で返す（ARC-010 / CPP-005）。

---

## 3. 検証コマンド

```bash
pwsh -NoProfile -File ./eng/check.ps1          # 唯一の完了定義（ローカルと CI で同じ）
```

開発中は最も狭い検査を使ってよい。フルゲートは **PR を Draft → Ready にする直前**に必ず通す。

🔴 **`pwsh -NoProfile -File ./eng/check.ps1` が通っていないものを「できた」と報告しない。**
実行していないコマンドの結果を書かない。テストの失敗を隠さない。
テストが本当の欠陥を見つけたら、期待値ではなく production コードを直す。

---

## 4. 変更の進め方

[docs/DEVELOPMENT_WORKFLOW.md](docs/DEVELOPMENT_WORKFLOW.md) が正本。要約すると:

Issue → 正典経路の特定 → ブランチ → （設計を変えるなら先に ADR）→ 最小の実装 →
テスト → 狭い検査 → `pwsh -NoProfile -File ./eng/check.ps1` → 規則 ID ごとの自己レビュー → PR（draft）→ Ready → squash merge。

コミットは Conventional Commits（`type` と `scope` は英語、説明は日本語、末尾に `(#N)`）。形の正本は GIT-003。

---

## 5. 完了報告の形

作業を終えたら必ず次を報告する。

```text
Issue / 規則 ID:
変更したファイルと振る舞い:
実行した検証コマンドと結果:
ドキュメント・スキーマの変更:
Waivers: none | WVR-NNNN
残るリスク:
```

調査だけを頼まれたときは、編集・コミット・push・PR 作成・外部状態の変更を行わない。

---

## 6. いまの状況

現在のタスクは [docs/todo/current.md](docs/todo/current.md)。GitHub Issue が正で、そこは要約。

<!-- 前例（xi-tools・nene-recall）は「動くもの／動かないもの／測れるもの」をここに実測つきで書いている。
     README の主張と実態が乖離しないよう、ここは実測でだけ更新する。 -->

---

## 7. 上位のポリシーと雛形の所在

このリポジトリの規約は **AYANE 厳格規約ポリシー**の適用例である。ポリシー本文・初期化手順・雛形は
施主のワークスペース（private）にある。

- WSL: `/home/xi/docker/_work/reports/ayane-strict-repo-policy/`（`POLICY.md` / `INIT_PROCEDURE.md` / `LINEAGE.md` / `templates/`）
- Windows: `\\wsl.localhost\Ubuntu\home\xi\docker\_work\reports\ayane-strict-repo-policy\`

雛形で足りなかったものを見つけたら、このリポを直すと同時に雛形へ還流する（board id:155）。
先行事例は NeNeClock（`/home/xi/docker/NeNeClock`・Java）と NeNeCommander（`C:\Users\info\WORKS\NeNeCommander`・C#）。
