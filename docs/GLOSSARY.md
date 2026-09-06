# 用語集 — NeNe Loupe

> Status: normative（規範）/ 2026-09-06 初版
> ここに載っている語は、コード・ドキュメント・Issue で**同じ意味**で使う。同義語を発明しない。

| 語 | 意味 | 型／場所 |
| --- | --- | --- |
| 結果（outcome） | 期待される成功・失敗を表す閉じた型 | `*Outcome` |
| 拒否理由（rejection / failure） | 結果に添える閉じた理由の集合 | `*Rejection` / `*Failure` |
| ポート（port） | 中核がプラットフォームを使うための型のある窓口 | `*Port`（application） |
| アダプタ（adapter） | 外部入力ポートの実装。画面取得などのOS入力を中核へ渡す。ウィンドウ描画はui/win32が担う | `*Adapter` |
| 合成ルート | 依存を結ぶ唯一の場所。端末と終了コードを持つ | `app（wWinMain）` |
| 反映（render） | 決まった値を UI 部品へ写す操作。UI 状態を変えてよい唯一の場所 | `render*`（UI） |
| 隔離区画 | 可変性を許した唯一の場所 | ARC-005 の表 |
| 規約検査（conformance） | NeNe Loupe 固有の自作ゲート | `eng/conformance.py`（CNF-NNN） |
| waiver | 1 つの規則に対する期限付きの狭い例外 | `docs/waivers/WVR-NNNN-*.md` |
| 機械強制の状態 | active / planned / 不能 / 不採用 | `docs/QUALITY_GATES.md` の強制マトリクス |
| negative proof | ゲートが意図した規則で落ちることの実測 | `docs/quality/gate-proofs.md` |
| RGB色 | 8bitの赤・緑・青の値。整形は持たない | `RgbColor`（core） |
| 表示文字列 | 色を表示形式に従って綴った文字列。表示・窓のタイトル・クリップボードで同じものを使う | `ColorText`（core） |
| 画面サンプル | 7×7画素の所有コピー。中央の画素が色番号の元になる | `ScreenSample`（core） |
| 採取座標 | レンズ中心の物理スクリーン座標。負の座標を含む | `ScreenPosition`（core） |
| 現在サンプルの所有者 | 一度のポート取得で最新結果へ置き換える | `LoupeController`（application） |
| 表示フレーム | 最新サンプルから生成する画素とHEX、または失敗文言 | `LoupeFrame`（application） |
| 色形式 | RGB10進・RGB16進・CMYK・HSL・HSVの閉じた集合。巡回の順もここが正本 | `ColorFormat`（core） |
| テーマ | 利用者が選ぶ配色。ダーク・ライト・システムに従うの閉じた集合 | `Theme`（core） |
| 見えの配色 | 実際に描くほうの閉じた集合。ダーク・ライトの2つだけ。システム追従はここへ解決される | `ThemeAppearance`（core） |
| 配色表 | 見えの配色ごとの色の対応。`windows.h`を含まない | `ThemePalette`（core） |
| 重なり順 | 常に最前面か、通常の重なり順かの閉じた集合 | `WindowLayer`（core） |
| 設定 | テーマ・色形式・重なり順の組。保存される値の正本 | `LoupeSettings`（core） |
| 設定フレーム | 設定モーダルが描く値と、読込・保存の状態 | `SettingsFrame`（application） |
| 取り込み除外 | 自分の窓を画面の取り込みから外すかどうかの閉じた集合。既定は外す。診断用の起動引数だけが動かす | `CaptureExclusion`（core） |

## 使ってはいけない語

`Manager` / `Helper` / `Util` / `Utils` / `Common` を型名の語尾に使わない（CPP-010）。
役割を語る名前が思いつかないときは、その型が 2 つの責務を持っている可能性が高い。
