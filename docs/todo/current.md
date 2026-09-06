# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #13実装時点の要約。

## 現在のIssue

[Issue #13](https://github.com/hideyukiMORI/nene-loupe/issues/13): 2×2市松模様を維持し、
アプリアイコンの単色虫眼鏡を大きく太く整える。ブランチは`feat/13-bold-loupe-icon`。
Issue #5、#6、#9、#11はmain `b22744c`まで統合済み。

## 実装・検証した内容

- 前景ルーペを中心(105,105)、半径60、線幅30、ハンドル線幅36へ調整した。
  背景4マスの色・配置・角丸と外周は変更していない。
- 正本SVGから既存generatorで8サイズの32-bit ICOを2回生成し、SHA-256一致を確認した。
- 全8サイズのRGBA/alphaを確認。隔離buildは71/71、conformance違反0。
- 新exeはICON 8件、GROUP_ICON 1件、manifest 1件。Shell抽出は32×32と16×16。
- 120 DPIで実行した窓のクラスアイコンは40×40と20×20で期待寸法と一致し、起動smokeは終了0。

## 完了まで

Issue #13と対応PRを進行状態の正本とし、未完了の工程だけ進める。利用者が確認中の正常位置
`[3343,1799,3643,1879]`のアプリは最終統合まで維持する。最終HEADの全体ゲートとCI成功後、
同じ位置と設定ファイルを保って検証済みexeへ一度だけ入れ替える。

Win32のアイコン読込・所有、generator、背景4マス、スタートアップ登録、ショートカット、通知領域、
AppUserModelID、設定スキーマは変更しない。Waivers: none。
