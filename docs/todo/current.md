# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のv0.2.0公開完了後の要約。

## 現在のIssue

[Issue #19](https://github.com/hideyukiMORI/nene-loupe/issues/19)と対応PRが、v0.2.0正式公開までの実績を
日報と引き継ぎへ記録する今回の更新の追跡先。完了状態はGitHubを参照し、未完了工程がある場合だけ進める。
製品コードと公開済みのtag・成果物は変更しない。
[Issue #17](https://github.com/hideyukiMORI/nene-loupe/issues/17)と
[PR #18](https://github.com/hideyukiMORI/nene-loupe/pull/18)は完了し、mainへ統合済み。

## 公開済みの状態

- 公開時のmainと`v0.2.0` tagの対象は`91ca33273cc6c8203349f250c03fa576f87bf20e`。
- [GitHub Release v0.2.0](https://github.com/hideyukiMORI/nene-loupe/releases/tag/v0.2.0)を公開済み。
- 最終全体ゲートはConformance違反0、Python 57/57、clean build 71/71、CTest 2/2、分岐92.50%、
  実ツール検証8件で終了0。[CI](https://github.com/hideyukiMORI/nene-loupe/actions/runs/34034847773)も成功。
- portable ZIPのSHA-256は`7C9B023ACD98916C9C9E3A03902BAD304C289103A3EE3A90FEDDFED80598B961`。
  GitHubからの再ダウンロード後も`SHA256SUMS`と一致した。設定スキーマ変更なし。Waivers: none。

## 次に行うこと

[Issue #19](https://github.com/hideyukiMORI/nene-loupe/issues/19)と対応PRの完了状態を確認し、未完了工程が
ある場合だけ進める。公開状態はGitHub Releaseを正本とする。新しい製品作業は焦点Issueを作成してから始める。
