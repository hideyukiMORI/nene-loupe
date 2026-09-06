# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #11実装時点の要約。

## 現在のIssue

[Issue #11](https://github.com/hideyukiMORI/nene-loupe/issues/11): アプリアイコンを単色ピクトと
2×2市松模様へ改める。ブランチは`feat/11-flat-checker-icon`。Issue #5、#6、#9は
main `35d5dea`まで統合済み。

## 実装・検証した内容

- [SVG](../design/app-icon.svg)をデザイン正本とし、NENE-PIXELと同系統の2×2市松模様へ
  単色charcoalのルーペを重ねた。
- 既存generatorはroot `svg`要素の寸法だけを対象pxへ変え、内側の256 unit背景rectを維持する。
- 全面背景のanti-aliasingを正しく扱うため、alpha契約を「最小値255未満、最大値255」とした。
  alpha 0を必須にしない。
- 16/20/24/32/48/64/128/256pxの32-bit ICOを2回再生成し、SHA-256一致を確認した。
- 隔離buildはRCを含む71/71、conformance違反0。新exeはICON 8件、GROUP_ICON 1件、manifest 1件。
- Shell抽出は32×32と16×16。120 DPIで実行した窓のクラスアイコンは40×40と20×20で期待寸法と一致。
  製品は終了0、元foregroundも復元した。

## 完了まで

Issue #11と対応PRを進行状態の正本とし、未完了の工程だけ進める。利用者が確認中の
`build/NeNeLoupe.exe`は最終統合まで維持する。最終HEADの全体ゲートとCI成功後に統合し、
検証済みexeへ一度だけ入れ替える。

Win32のアイコン読込・所有、スタートアップ登録、ショートカットやインストーラー、通知領域常駐、
AppUserModelID、設定スキーマは変更しない。Waivers: none。
