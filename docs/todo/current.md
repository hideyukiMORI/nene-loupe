# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #15実装時点の要約。

## 現在のIssue

[Issue #15](https://github.com/hideyukiMORI/nene-loupe/issues/15): 太い単色虫眼鏡と2×2市松模様を保ち、
リングと持ち手の一体形状、レンズ内配色、外枠と余白を洗練する。ブランチは
`feat/15-unified-icon-silhouette`。Issue #5、#6、#9、#11、#13はmain `4881e70`まで統合済み。

## 実装・検証した内容

- ルーペを中心(116,116)、外半径78、内半径48とし、幅30の持ち手まで1つのevenoddシルエットで描いた。
- 外周は角半径22、見える幅1.5へ軽くした。背景4色と2×2構成は維持した。
- 正本SVGから既存generatorで8サイズの32-bit ICOを2回生成し、SHA-256一致を確認した。
- 全8サイズのRGBA/alphaを確認。隔離buildは71/71、conformance違反0。
- 新exeはICON 8件、GROUP_ICON 1件、manifest 1件。Shell抽出は32×32と16×16。
- 120 DPIのクラスアイコンは40×40と20×20で期待寸法と一致し、起動smokeは終了0。

## 完了まで

Issue #15と対応PRを進行状態の正本とする。利用者が確認中のアプリは最終統合まで維持する。
最終HEADの全体ゲートとCI成功後、直前に実位置と設定SHAを再読し、同じ状態を保って検証済みexeへ
一度だけ入れ替える。

Win32のアイコン読込・所有、generator、背景4色、スタートアップ登録、ショートカット、通知領域、
AppUserModelID、設定スキーマは変更しない。Waivers: none。
