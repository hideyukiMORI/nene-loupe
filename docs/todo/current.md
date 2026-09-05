# いまのタスク — NeNe Loupe

> GitHub Issueが正。この文書は実測に基づく要約。

## 現在のIssue

[Issue #1](https://github.com/hideyukiMORI/nene-loupe/issues/1): 初期化とC++の実測・検査基盤。
productionコードはこのIssueの対象外。

| 段階 | 状態 |
| --- | --- |
| 初期化・仕様・ADR 0001/0002 | 完了 |
| Phase 0 / M-1〜M-8 | MSVC・clang-tidyで19ケース実測済み。ADR 0001/0003とphase0-results.jsonに記録 |
| Phase 1 / 検査基盤 | CMake・警告エラー・clang-tidy・clang-format・規約検査・フック・CI定義を追加。ローカルのフルゲート成功 |
| Phase 2 / negative proof | 検査器44ケース、実ツール6反例と復帰を実測。main保護は設定・読み戻し済み。CIは道具の選択差を修正して再確認中 |
| Phase 3 / 最初の縦切り | 未着手。別Issueで枠なし窓とカーソル中心のHEX表示を作る |

## 動くものと限界

CMakeで基盤確認用のexeがビルドできる。ルーペ本体はまだ存在しない。
単一ゲートは `pwsh -NoProfile -File ./eng/check.ps1`。
言語や字句検査で十分に塞げない規則はplannedのまま。意味的な不変性、完全なAPI禁止、
中核カバレッジは未完成であり、検査設定があるだけでactiveにはしない。

## 次の作業

Draft PRからCIの実行とリポジトリ保護設定を確認する。
上位ポリシーはWindowsでは `\\wsl.localhost\Ubuntu-22.04\home\xi\docker\_work\reports\ayane-strict-repo-policy\`。
