# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-10-04のv1.0.0公開（Microsoft StoreとGitHub Release）後の要約。

## 現在のIssue

[ADR 0008](../adr/0008-add-msix-store-channel.md)（配布経路にMicrosoft Store（MSIX）を足し、ZIPは残す）を
hideが2026-10-03に受理した（[Issue #29](https://github.com/hideyukiMORI/nene-loupe/issues/29)）。実装と提出は済み、
2026-10-04からStoreとportable ZIPの2本で配っている。Store提出までの作業は
[Issue #31](https://github.com/hideyukiMORI/nene-loupe/issues/31)〜[#38](https://github.com/hideyukiMORI/nene-loupe/issues/38)に分けてある（すべて完了）
（対応表はADR 0008の「受理の記録」）。身元の値はADR 0008の追記と`eng/store-identity.json`にある。

プライバシーポリシー（[Issue #32](https://github.com/hideyukiMORI/nene-loupe/issues/32)）はhideが読んで統合済み（`PRIVACY.md`）。
スキップされた`check`でも統合できてしまう穴は[Issue #30](https://github.com/hideyukiMORI/nene-loupe/issues/30)で塞いだ。
統合の前にheadのSHAで最新の`check`の成功を読む手順は続ける（[引き継ぎ書](../handoffs/2026-10-04.md)）。

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

- **v1.0.0（最新）。** Microsoft Storeで公開中（Store ID `9N6HZB1SXKBM`、<https://apps.microsoft.com/detail/9N6HZB1SXKBM>。
  2026-10-04にhideがPartner Centerの「Microsoft Store で取り扱い中」を確認）。
  同じ日に`v1.0.0`のtagを`0c26ed4da205cc3152355bc0b6be6f10df5061c6`に打ち、
  [GitHub Release v1.0.0](https://github.com/hideyukiMORI/nene-loupe/releases/tag/v1.0.0)を公開した。
  ZIPは提出のときに控えたもので、作り直していない。SHA-256は`EBE65AA16347DF68C7B4053D233C38FBD18DCE6AC9E72B3C240F7DDA150B36DD`、
  中のexeは`587268D15A362970562FD1650A40A700F8B736A6B62DCC481C9B14DBD2612FDA`（Storeへ出したMSIXの中と同じ）。
  GitHubからの再ダウンロード後も`SHA256SUMS`と一致した。ZIPに同梱の`README.txt`はStore版を「未公開」と書いたままである。
- 以下はv0.2.0の記録。公開時のmainと`v0.2.0` tagの対象は`91ca33273cc6c8203349f250c03fa576f87bf20e`。
- [GitHub Release v0.2.0](https://github.com/hideyukiMORI/nene-loupe/releases/tag/v0.2.0)を公開済み。
- 最終全体ゲートはConformance違反0、Python 57/57、clean build 71/71、CTest 2/2、分岐92.50%、
  実ツール検証8件で終了0。[CI](https://github.com/hideyukiMORI/nene-loupe/actions/runs/34034847773)も成功。
- portable ZIPのSHA-256は`7C9B023ACD98916C9C9E3A03902BAD304C289103A3EE3A90FEDDFED80598B961`。
  GitHubからの再ダウンロード後も`SHA256SUMS`と一致した。設定スキーマ変更なし。Waivers: none。

## 次に行うこと

**Microsoft Storeの認定を通り、1.0.0をStoreとGitHub Releaseの両方で公開した（2026-10-04・[Issue #68](https://github.com/hideyukiMORI/nene-loupe/issues/68)）。**
IARCの年齢区分の確定の通知も2026-10-04に届いた（[Issue #66](https://github.com/hideyukiMORI/nene-loupe/issues/66)）。
Storeが署名したパッケージの実測（起動、設定の保存先、シェルが返すアイコン）も2026-10-04に済んだ
（[Issue #70](https://github.com/hideyukiMORI/nene-loupe/issues/70)、`docs/quality/gate-proofs.md`第17節）。結果は検証用パッケージと同じで、
新しく分かったのは、設定のフォルダが残っていてファイルだけが無いと、Store版は本物の場所へ書くことである。
提出までの記録は[日報](../reports/2026-10-04.md)にある。[引き継ぎ書](../handoffs/2026-10-04.md)の「認定された場合」の1〜4番は済み、いま進行中の作業は無い。
測っていないのは、スタートメニューとタスクバーの画面、アンインストール後の残留、版を上げたときの設定の引き継ぎ、Windows 10である。
Windows App Certification KitはPASS 23・WARNING 1・FAIL 0（高DPIの警告は原因が分かっていない。`docs/quality/gate-proofs.md`第16節）。
掲載用の画像は`docs/images/store/`、掲載文と年齢区分の根拠は`docs/release/store-listing.md`にある。アプリの表示は英語だけになった（Issue #52）。
公開状態はGitHub Releaseを正本とする。新しい製品作業は焦点Issueを作成してから始める。
Issue #21の統合後は、READMEの画像がUIの変更に追随しないこと（機械の検査が無いこと）が残る穴である。
