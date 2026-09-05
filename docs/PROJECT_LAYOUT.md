# モジュール構成と依存規則 — NeNe Loupe

> Status: normative（規範）/ 2026-09-06 初版
> パッケージルート: `neneloupe`

モジュールグラフはアーキテクチャの一部である。**パッケージの命名規約だけでは依存の境界にならない。**
承認された一覧の機械可読な正本は `eng/architecture.json` で、規約検査がそれと実際のビルドグラフを突き合わせる（ARC-002）。

---

## 1. 承認されたモジュール

```text
app（wWinMain）
    合成ルート。ポートに実装を結び、UI を起動する。端末出力と終了コードを持つ唯一の場所

ui/win32
    画面。状態を描き、意図を発行する

adapters/win32
    画面取得・クリップボード・保存を実装する。現在時刻・乱数・環境を読める唯一の製品モジュール

application
    ポートの宣言、表示値の生成、結果型

core
    値・不変条件・閉じた選択肢。標準ライブラリ以外に依存しない
```

---

## 2. 許可された依存グラフ

```text
core
    -> 標準ライブラリのみ

application
    -> core

adapters/win32
    -> application, core

ui/win32
    -> application, core

app（wWinMain）
    -> すべて（明示的な合成のためだけに）
```

ここに無い依存はすべて禁止である。とくに:

- core -> UI 枠組み: **禁止**（標準ライブラリ同梱ならビルドグラフでは塞げないので検査層で塞ぐ）
- core -> application: 禁止
- application -> アダプタ: 禁止
- ui -> アダプタ: 禁止
- アダプタ -> 別のアダプタ: 禁止
- 何か -> 合成ルート: 禁止

---

## 3. 各モジュールの責務

### `core`

意味の正本を持つ。値型・閉じた選択肢・不変の設定値・拒否理由と結果型。
UI 状態・直列化注釈・永続化・現在時刻を持たない。実行時依存の許可表は空。追加にはADRとゲート整備を要する（ARC-003 / QLT-011）。

### `application`

振る舞いの調整を持つ。ポート・表示値の生成・結果型。UI 枠組み・永続化・ファイル・ネットワークを知らない。

### `adapters/win32`

画面取得、カーソル位置、クリップボード、永続化のポートを実装する。保存形式の版と移行（ARC-009）、DTOをここに閉じる。
**決定性の禁止を適用しない唯一の製品モジュール**（ARC-007）。字句検査の区画判定に明示する。

### `ui/win32`

application が作った値を描き、操作を意図として渡す（ARC-011）。可変性の隔離区画（ARC-005）。

### `app（wWinMain）`

依存を結ぶ。端末出力と終了コードを扱ってよい唯一の場所（ARC-006）。

---

## 4. ソースの置き場

| 種類 | 置き場 |
| --- | --- |
| production | `src/core` / `src/application` / `src/adapters/win32` / `src/ui/win32` / `src/app` |
| テスト | `tests/build`（C++基盤）/ `tests/unit`（OS非依存の中核）/ `tests/conformance`（検査器自身の正例・反例） |
| 検査設定 | `.clang-format` / `.clang-tidy` / `eng/*.json`。参照の一覧は `eng/config-bindings.json`（CNF-007） |
| 生成物 | `build/`（CMake・オブジェクト・検証exe）/ `out/`（実測fixture・出力）。製品のコード生成は未採用 |

Issue #3で検証専用の`unit_tests`を追加した（ADR 0004）。依存先はcore/applicationだけ。
中核の全cppと同じ単体テストを、測定専用LLVMビルドでも使う。
OSライブラリの宣言は`neneloupe_system_link`を通し、`platformLibraries`の許可表と照合する。
手動起動する`eng/verify-window.py`は実Windows・ポインタ・表示を扱う環境検証で、単体テストではない。
