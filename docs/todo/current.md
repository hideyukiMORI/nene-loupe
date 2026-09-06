# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #17実装時点の要約。

## 現在のIssue

[Issue #17](https://github.com/hideyukiMORI/nene-loupe/issues/17): CMakeのPROJECT_VERSIONを唯一の
版入力としてWin32 metadataを揃え、Release x64のportable ZIPとchecksumを生成してv0.2.0を公開する。
ブランチは`feat/17-release-v0-2-0`。Issue #5、#6、#9、#11、#13、#15はmainへ統合済み。

## 実装時点の確認

- `src/app`のmanifestとVERSIONINFOはCMake configureでbuild下へ生成し、設定画面と同じ版入力を使う。
- `eng/package-release.ps1`は固定toolchainと既存CMake targetをReleaseでclean buildする。
- portable ZIP直下は`NeNeLoupe.exe`、利用者向け`README.txt`、正本`LICENSE`の3件だけで、
  `SHA256SUMS`はZIPを対象にする。
- Release narrow buildは71/71、build graph conformance 0、CTest 2/2。x64、静的runtime、
  manifest 0.2.0.0、FileVersion 0.2.0.0、ProductVersion 0.2.0を実測した。
- ZIPから展開したexeの短いPMv2起動smokeは終了0。120 DPIで300×80 px、大・小HICONは非0だった。

## 次に行うこと

Issue #17と対応PRを進行状態の正本として未完了の工程だけ進める。最終HEADの全体ゲートとCI成功後に
squash統合し、cleanなmainからRelease成果物を作り直して実測する。公開するZIP、`SHA256SUMS`、
release notesをレビュー後、`v0.2.0` tagとGitHub Releaseを作る。

インストーラ、自動更新、コード署名基盤、スタートアップ登録、ショートカットは今回追加しない。
利用者が起動中のアプリ、設定、位置は変更しない。
