# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #9実装時点の要約。

## 現在のIssue

[Issue #9](https://github.com/hideyukiMORI/nene-loupe/issues/9): タスクバー、Alt+Tab、
エクスプローラー、実行ファイルを参照するショートカットへ独自アプリアイコンを表示する。
ブランチは`feat/9-application-icon`。Issue #5と#6はmain `ac91af4`まで統合済み。

## 実装・検証した内容

- [SVG](../design/app-icon.svg)をデザイン正本とし、16/20/24/32/48/64/128/256pxを
  個別に描画した32-bit ICOを派生資産として保存した。
- app targetのRC資源へICOを組み込み、`LoupeWindow`の大・小クラスアイコンを同じresource IDから
  読み込む。非shared HICONは窓とクラスの破棄後に解放する。
- 読込失敗をOS既定アイコンで隠さず、`WindowFailure`の既存起動失敗経路へ通す。
- 通常buildへ画像変換依存を追加していない。手動再生成だけがChromeとPillowを使う。
- 同じSVGから3回再生成したICOのSHA-256が一致。全8サイズ、32-bit、alphaを確認した。
- 隔離buildはRCを含む71/71、build graphのconformance違反0。
- 新exeはICON 8件、GROUP_ICON 1件、manifest 1件を持つ。Shell抽出は32×32と16×16。
- 120 DPIで実行した窓のクラスアイコンは40×40と20×20で期待寸法と一致した。
- `python -B eng/verify-window.py --executable out/build-icon/NeNeLoupe.exe`の既定fullは終了0。

## 完了まで

最終全体ゲート、Draft PR、Ready後のCIとレビューを確認する。利用者が確認中の旧
`build/NeNeLoupe.exe`は終了していない。新しいexeへの入れ替え時だけ終了する。

スタートアップへの自動登録、ショートカットやインストーラーの生成、通知領域常駐、
AppUserModelID追加は範囲外。設定スキーマ変更なし。Waivers: none。
