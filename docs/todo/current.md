# いまのタスク — NeNe Loupe

> GitHub Issueが正。この文書は実測に基づく要約。

## 現在のIssue

[Issue #3](https://github.com/hideyukiMORI/nene-loupe/issues/3): 最初の縦切りでルーペ直下を拡大して中心のHEXを表示する。
[PR #2](https://github.com/hideyukiMORI/nene-loupe/pull/2)はsquashで統合済み、Issue #1は完了した。

| 段階 | 状態 |
| --- | --- |
| 初期化・仕様・ADR 0001/0002 | 完了 |
| Phase 0 / M-1〜M-8 | 19ケースを実測済み。phase0-results.jsonに記録 |
| Phase 1・2 / 検査基盤 | PR #2統合済み。ローカル・CIの単一ゲート成功、main保護適用済み |
| Phase 3 / 最初の縦切り | 5層の実装とOS非依存テストを追加。ローカルのフルゲート成功。検査器54件・CTest2件・分岐91.67%・実ツール7反例と復帰 |

## 動くものと限界

`build/NeNeLoupe.exe`は枠なし・最前面の160×64 DIP窓に背面の7×7画素の8倍拡大と中心HEXを表示する。
どこからでもドラッグ移動し、Esc/Alt+F4で閉じる。取得失敗時は古い画素を消して失敗文言を表示する。
最新の取得結果はLoupeControllerだけが所有する。コピー・形式切替・設定・保存は未実装。

完了定義は `pwsh -NoProfile -File ./eng/check.ps1`。
ADR 0005で採取中心をルーペ中央に固定し、自分の窓を取り込みから除外した。
4台のモニタ（120/144/168 DPI）、負の座標、1物理画素の移動による採取位置変更、色一致、Esc終了を確認した。
画面端と長時間の性能測定は未確認。
実行した証拠と未確認範囲は[gate-proofs.md](../quality/gate-proofs.md)を参照する。
意味的な不変性、完全なAPI禁止など、十分に塞げない規則はplannedを維持する。

## 次の作業

[PR #4](https://github.com/hideyukiMORI/nene-loupe/pull/4)のCI確認とレビュー。
実機検証はポインタを操作しない方法へ変更し、完了した。次はコピー・色形式切替・設定の個別実装。
上位ポリシーはWindowsでは `\\wsl.localhost\Ubuntu-22.04\home\xi\docker\_work\reports\ayane-strict-repo-policy\`。
