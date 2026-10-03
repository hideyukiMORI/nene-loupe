# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のv0.2.0公開完了後の要約。

## 現在のIssue

[ADR 0008](../adr/0008-add-msix-store-channel.md)（配布経路にMicrosoft Store（MSIX）を足し、ZIPは残す）を
hideが2026-10-03に受理した（[Issue #29](https://github.com/hideyukiMORI/nene-loupe/issues/29)）。実装はこれからで、
実際に配っているのはportable ZIPの1本のまま。Store提出までの作業は
[Issue #31](https://github.com/hideyukiMORI/nene-loupe/issues/31)〜[#38](https://github.com/hideyukiMORI/nene-loupe/issues/38)に分けてある（#34と#35を除いて完了）
（対応表はADR 0008の「受理の記録」）。開発者アカウントの登録とアプリ名`NeNe Loupe`の予約は済んだ（下書き段階・申請は始めていない）。身元の値はADR 0008の追記にある。

プライバシーポリシー（[Issue #32](https://github.com/hideyukiMORI/nene-loupe/issues/32)）はhideが読んで統合済み（`PRIVACY.md`）。
スキップされた`check`でも統合できてしまう穴は[Issue #30](https://github.com/hideyukiMORI/nene-loupe/issues/30)で塞いだ。
統合の前にheadのSHAで最新の`check`の成功を読む手順は続ける（[引き継ぎ書](../handoffs/2026-10-03.md)）。

[Issue #25](https://github.com/hideyukiMORI/nene-loupe/issues/25)（下調べ）と
[Issue #27](https://github.com/hideyukiMORI/nene-loupe/issues/27)（CIのMSVCの版ずれ）は完了し、mainへ統合済み。
裏取りと実測は[調査報告](../reports/2026-10-03-store-msix-research.md)、当日の記録は[日報](../reports/2026-10-03.md)にある。

[Issue #21](https://github.com/hideyukiMORI/nene-loupe/issues/21)が、READMEのトップへ実機の
スクリーンショットを置く今回の作業の追跡先。撮影経路の選定は[ADR 0007](../adr/0007-photograph-the-loupe-for-the-readme.md)、
実測は`out/readme-capture/`にある。

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

残りはIssue #34（ロゴ一式）と#35（WACK）。どちらも実機への導入と管理者権限が要る。
掲載用の画像は`docs/images/store/`、掲載文と年齢区分の根拠は`docs/release/store-listing.md`にある。アプリの表示は英語だけになった（Issue #52）。
mainの版は1.0.0だが、tagとGitHub Releaseは作っていない（公開済みの最新はv0.2.0）。
公開状態はGitHub Releaseを正本とする。新しい製品作業は焦点Issueを作成してから始める。
Issue #21の統合後は、READMEの画像がUIの変更に追随しないこと（機械の検査が無いこと）が残る穴である。
