# ADR 0006 — デザインに沿った操作部と、版を持つ設定の保存

- 状態: 受理
- 日付: 2026-09-06
- Issue: #6
- 影響する規則: ARC-001 / ARC-003 / ARC-004 / ARC-007 / ARC-009 / ARC-010 / ARC-011 / CPP-002 / CPP-007 / CPP-012 / QLT-009
- 置換しない: ADR 0002（素の Win32）・0004（層と単一のサンプル状態）・0005（レンズ直下の採取）はそのまま有効。
  本 ADR は採取契約に触れず、その上に操作部と設定を足す。

## 文脈

Issue #3 で FR-001〜005 の縦切りが通った。残りは FR-006（コピー）・FR-007/008（色形式）・
FR-009〜012（設定・テーマ・版・著作権・保存）で、いずれも**現状の 160 × 64 DIP には置き場が無い**。

施主（hide）の依頼でデザインを起こし、値主役の 240 × 64 DIP 案を推奨として具体化した
（[docs/design/loupe-look.md](../design/loupe-look.md)）。

**施主が直接指示したのは次の 3 点である。**

- 「設定で常に最前面のトグルも欲しい」
- 「クリックでクリップボードに値をコピーも忘れないでね」
- 実装まで進めること

**次は施主の指示ではなく、進行を任された実装側（サナ）の判断である。** 採否は後から施主が覆せる。

- テーマを 3 択（ダーク / ライト / システムに従う）にすること
- 保存形式を版 1 だけで始め、旧版の読み取りを書かないこと
- 色番号の区切りに空白を残すこと（`255, 128, 0`）
- 最前面の既定を「入」にし、再起動をまたいで保存すること

実測で分かった制約は 2 つ。

1. 現状の値欄は 90 DIP しかなく、仕様例の区切り（`255, 128, 0`）で書いた CMYK の最長は入らない。
2. 値を proportional の Segoe UI で描くと、30 ms ごとの再採取で桁数が変わるたび文字列幅が動く。

実 Windows での GDI 実測（Per-Monitor v2、96/120/144/168 DPI、`GetTextExtentPoint32W`）では、
Consolas Bold 15 DIP のとき、**素朴 CMYK が実際に生成しうる最長** `100, 100, 0, 100` が全 DPI で
値欄 148 DIP に収まる。素朴換算では最大チャンネル由来の成分が必ず 0 になるため、
`100, 100, 100, 100` は生成されない。

## 決定

**推奨デザイン B を製品の形とし、窓を 240 × 64 DIP に広げ、
表示・コピー・形式・テーマ・最前面を 1 つずつ正典の経路に載せ、設定を版 1 の 1 ファイルに保存する。**

### 1. 窓と操作部（FR-002 / 006 / 007 / 008 / 009）

- 窓は 240 × 64 DIP。高さと 7 × 7 画素 × 8 倍の採取は現状のまま。**ADR 0004 / 0005 の採取契約は変えない。**
- `WM_NCHITTEST` は 3 つの矩形だけ `HTCLIENT` を返し、残りは `HTCAPTION` のまま。
  左 86 × 64 DIP（レンズと色板）が握りになる。寸法の正本は `ui/win32` の `LoupeLayout` 1 か所。
- 値の矩形を左クリックすると、**いま表示している文字列そのもの**をクリップボードへ書く。
  表示とコピーは同じ `ColorText::of` の戻り値で、第二の整形経路を作らない（ARC-001）。
- 成功は約 900 ms のあいだチェックと「コピー」を出す。**失敗も見える形で出す**（「コピー不可」）。
- 押した位置と離した位置が `SM_CXDRAG` / `SM_CYDRAG` を超えたらクリックを取り消し、
  `WM_NCLBUTTONDOWN` へ委ねて移動に切り替える。操作部の上でも窓を掴んで動かせる。

### 2. 色形式（FR-007 / FR-008）

- 閉じた集合は `ColorFormat`（`rgb_decimal` / `rgb_hex` / `cmyk` / `hsl` / `hsv`）。
  巡回は `next_format` の 1 か所で、この順に回る。
- 文字列は `core` の `ColorText` に閉じる。**`RgbColor::hex()` は削除した。**
  同じ意味（色の表示文字列）に経路を 2 本持たせないため（ARC-001）。
- 区切りは仕様の例に合わせて `", "` を保つ。値は Consolas Bold 15 DIP で描く。
- CMYK は ICC を使わない素朴換算であることを設定モーダルに明記する（SPECIFICATION 第 3 節）。
- **色相の丸めの契約**: 角度を先に `[0,360)` へ正規化してから四捨五入し、360 は 0 へ畳む。
  負のまま丸めると赤の手前で丸めが非対称になる（RGB(240,0,2) の 359.5 度）。

### 3. テーマ（FR-010）

- 利用者が選ぶ閉じた集合は `Theme`（`dark` / `light` / `system`）。
- 実際に描く配色は `ThemeAppearance`（`dark` / `light`）で、**2 つを別の型にする**。
  `system` を解決できるのは OS の設定を読める `adapters/win32` だけなので、
  `SystemAppearancePort` を通す（ARC-007）。解決結果は `application` が持ち、
  設定変更時と `WM_SETTINGCHANGE` のときだけ更新する。30 ms ごとに OS を読まない。
- 配色表は `core` の `ThemePalette` に 1 つ。`windows.h` を含まない素の 3 成分で持ち、
  `COLORREF` への変換は `ui/win32` で行う（ARC-003）。

### 4. 常に最前面（新しい要件）

- 閉じた集合 `WindowLayer`（`topmost` / `normal`）。boolean で表さない（CPP-002 / CPP-006）。
- 既定は `topmost`。現状の `WS_EX_TOPMOST` と同じ挙動から始める。
- 切替はその場で `SetWindowPos(..., HWND_TOPMOST / HWND_NOTOPMOST, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)`。
- **設定モーダルとルーペ窓の最前面属性は必ず同じ値にし、親 → モーダルの順に適用する。**
  片方だけ最前面にすると、モーダルが親の裏へ回るか、逆に無関係な窓の上へ浮く。

### 5. 設定モーダル（FR-009 / 010 / 011）

- 320 × 392 DIP の 2 枚目の `WS_POPUP` 窓。クラス名 `NeNeLoupe.Settings`。
- ルーペ窓をオーナーに持つモーダル。開いている間は `EnableWindow(loupe, FALSE)` で
  **入力だけ**を止め、30 ms の採取と再描画は続ける。閉じるときは先に `EnableWindow(TRUE)` に戻す。
- モーダルにもルーペ窓と同じ `SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)` を掛ける。
  掛けないと、モーダルがレンズに重なったときに自分自身の画素を採る（ADR 0005 の規則を 2 枚目にも通す）。
- 版はビルド時に 1 か所（CMake の `project(... VERSION ...)`）から埋め込む。
  ビルドの sha は埋めない。再現性のためにビルド入力を増やさない。

### 6. 設定の保存（FR-012 / ARC-009）

- 保存先は `%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`。UTF-8、LF、4 行、順序固定。

  ```text
  schema=1
  theme=dark|light|system
  format=rgb|hex|cmyk|hsl|hsv
  layer=topmost|normal
  ```

- パス解決は `adapters/win32` の中だけで `GetEnvironmentVariableW(L"LOCALAPPDATA")` を使う。
  `SHGetKnownFolderPath` は環境変数の差し替えを見ないため、検証子プロセスが
  `LOCALAPPDATA` を一時フォルダへ向けるだけで、**利用者の実設定に触れずに再起動を試せる**。
- 書き込みは同じフォルダの `settings.v1.txt.tmp` へ出してから
  `MoveFileExW(..., MOVEFILE_REPLACE_EXISTING)` で置換する。半端なファイルを残さない。
- **ファイルが無いのは失敗ではない。** 既定値を返す（初回起動）。
- `schema` が無い / `1` 以外 / 行が足りない / 未知のトークンは、**既定値へ黙って落とさず**
  `SettingsFailure::unreadable` を返し、設定モーダルに見える形で出す（ARC-009）。
- **版 1 が初版なので、移行規則も旧版の読み取りも書かない。** 存在しない過去との互換を装わない。
- 保存の失敗は `SettingsFailure::unwritable`。これもモーダルに出す。

### 7. 状態の所有者

`LoupeController` が最新のサンプル・設定・コピー状態・設定の状態を 1 つずつ持つ（ARC-004）。
UI は `LoupeFrame` / `SettingsFrame` を受け取って描くだけで、失敗理由を解釈しない（ARC-011）。

### 8. 採取資源の再利用

`Win32ScreenSamplerAdapter` が `CaptureSurface` を保持して使い回す。
30 ms ごとに `GetDC` / `CreateCompatibleDC` / `CreateDIBSection` を作り直していたのをやめる。
**ただし失敗を固定しない。** 取得に失敗したら保持していた面を捨て、次の呼び出しで作り直す。
表示構成が変わっても、次の 30 ms で回復する。

## 強制

強制の状態の正本は [QUALITY_GATES.md](../QUALITY_GATES.md) の強制マトリクスである。
**本 ADR は新しい規則を足さないので、どの規則の状態も動かさない。** 下は「この決定が実際に何に支えられているか」の内訳。

- **QLT-009 は active。** `eng/coverage.py` が `core` / `application` の分岐 90% を要求する。
  色変換・設定・表示状態の遷移はここで実測される。本 ADR の決定のうち、機械が本当に守るのはここが中心。
- **QLT-002 は active。** MSVC `/W4 /WX`、clang-tidy の `WarningsAsErrors`、clang-format が
  警告と整形差分を失敗にする。
- **CPP-002 は planned のまま。** 実在する補助は `/we4061 /we4062` と
  clang-tidy の `covered-switch-default` で、**新しい閉じた集合の `switch` に分岐漏れがあれば実際に落ちる**。
  塞げていないのは、`if` / `else` で書いた意味分類と、範囲外の enum 値を作れてしまうこと。
- **ARC-002 / ARC-003 は planned のまま。** 実在する補助は `eng/targets.cmake` と
  CMake File API の突き合わせ、および `src/core/` `src/application/` に対する
  プラットフォームヘッダの字句検査。塞げていないのは推移的な include とマクロ経由の迂回。
- **ARC-007 は planned のまま。** 実在する補助は禁止 API の字句検査で、
  `GetEnvironmentVariableW` を `src/adapters/win32/` の外に書けば落ちる。
  塞げていないのは、間接呼び出しと未登録の API。
- **ARC-009 は planned のまま。** 版 1 しか無く、読めない版を型のある失敗として返す実装があるだけである。
  **移行規則そのものは機械で見ていない。** 保存と再起動をまたぐ復元は、この ADR の時点では未検証であり、
  `eng/verify-window.py` の実測で別に記録する。
- 「表示とコピーが同じ文字列であること」は、同じ `ColorText::of` の戻り値を使うという
  **正典経路の形**で担保する。機械の証明は無い。正典経路のレビューと実 Windows 検証で確認する。

## 結果

得られるもの:

- 仕様の FR-006〜012 に置き場ができ、操作部とドラッグが場所で分かれる。
- 色の表示文字列の正本が 1 つになる。表示・タイトル・クリップボードが同じ文字列を使う。
- 設定が版を持ち、読めない版を黙って既定値にしない。
- 採取の GDI 資源が毎秒 33 回の生成・破棄をやめる。

失うもの・正直に記録すること:

- **窓が現状比 +50% の幅になる。** 覆う面積が増える。高さは変えていない。
- `RgbColor::hex()` を削除したので、GLOSSARY の「HEX 整形の正本」は `ColorText` に移った。
- 素朴 CMYK の丸めは実装の定義であり、他ツールと 1% 単位で一致する保証はない。
- テーマの「システムに従う」は OS の設定を読む。読めないときは light として扱う。
  これは失敗ではなく Windows の既定に合わせた解決であり、利用者には見せない。
- 最前面を切ると、`WDA_EXCLUDEFROMCAPTURE` により一般のキャプチャにも映らないままなので、
  背面へ回った窓を探しにくい。主窓に状態表示は描いていない。
- 実 Windows での見え方・ヒット領域・再起動の保持は、この ADR では証明されない。
  `eng/verify-window.py` の実測で別に記録する。

## 却下した選択肢

| 選択肢 | 却下の理由 |
| --- | --- |
| 窓を 160 × 64 のままにする | コピー・形式・ギアを置く場所が無い。値欄も仕様例の区切りで CMYK が入らない |
| 値を proportional のまま描く | 30 ms ごとの再採取で桁が踊る。色番号は読み取りが目的で、揺れは実害 |
| 区切りを `,` に詰めて幅を稼ぐ | 実装側の判断で仕様例の表記（`255, 128, 0`）を保った。GDI 実測で 15 DIP でも収まり、詰める理由が無い |
| `Theme` に `system` を入れたまま配色表を引く | `system` に対応する配色が無い。解決を型で分けないと、どこかで既定へ落とす分岐が要る |
| 最前面を `bool` で持つ | 閉じた選択肢を boolean で表さない（CPP-002 / CPP-006） |
| 設定をレジストリに保存する | 検証が利用者の実設定を書き換えるか、退避・復元の手順が要る。`%LOCALAPPDATA%` なら子プロセスの環境変数だけで隔離できる |
| 版 0 や旧形式の読み取りを先回りで書く | 存在しない過去との互換は検証できない。初版は版 1 だけを読む |
| 読めない設定を既定値で上書きして起動する | ARC-009 が禁じる。利用者の設定を黙って捨てる |
| コピー失敗を無視する | 実装側の判断。読込不能・保存不可・コピー失敗はすべて見える状態にする。設定側は ARC-009 の要請でもある |
| 検証のために製品へコマンドラインや隠し入口を足す | 第二の経路になる。通常の UI をウィンドウメッセージで操作して検証する |
| ビルドの sha を版に埋める | 構成に git を足し、同じソースから同じ成果物が出なくなる |

## 参考

- [SetWindowPos](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos)
- [MoveFileExW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw)
- [docs/design/loupe-look.md](../design/loupe-look.md)
