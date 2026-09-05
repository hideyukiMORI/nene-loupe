# いまのタスク — NeNe Loupe

> GitHub Issueが正。この文書は2026-09-06の終了時点の要約。

## 現在のIssue

[Issue #6](https://github.com/hideyukiMORI/nene-loupe/issues/6): デザインB、値クリックのコピー、色形式、保存可能な設定。
[PR #4](https://github.com/hideyukiMORI/nene-loupe/pull/4)は統合済み。
作業ブランチは `feat/6-designed-loupe-controls`、分岐元mainは `ad65487`。
実装と検証を保存する区切りで、レビューと統合は次回。

## 実装・検証

- 240×64 DIPのルーペ、5形式、値クリックのコピー、成功・失敗表示。
- 320×392 DIPの設定モーダル、テーマ3択、常に最前面の即時切替と保存。
- 設定スキーマ1、破損・未対応入力の通知と無操作時の保全。ADR 0006に記録。
- 全体ゲート終了0、CTest 2/2、分岐111/120＝92.50%、実ツール7反例と復帰成功。
- Windows検証終了0。4モニタ・120/144/168 DPI、色採取、ヒット領域、コピー全形式、
  モーダル性、最前面OFF/ON/OFF、再起動後の復元、不正設定5ケースを確認。

## 次回

1. [引き継ぎ書](../handoffs/2026-09-06.md)を読む。実装・未確認事項・Claude再開方法を記録済み。
2. 手動のドラッグ・Aero Snap・右クリックメニュー・フォーカス・見た目をレビューする。
3. Draft PRのCIを確認し、Ready直前に `pwsh -NoProfile -File ./eng/check.ps1` を再実行する。
4. 画面端・長時間の性能検証を進める。増分ビルド改善のIssue #5は別作業。

記録: [日報](../reports/2026-09-06.md)、[検証証拠](../quality/gate-proofs.md)。Waivers: none。
