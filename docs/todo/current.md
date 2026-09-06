# いまのタスク — NeNe Loupe

> GitHub IssueとPRが進行状態の正本。この文書は2026-09-06のIssue #6実装・残検証完了時点の要約。

## 今回対応したIssue

[Issue #6](https://github.com/hideyukiMORI/nene-loupe/issues/6): デザインB、値クリックのコピー、色形式、
保存可能な設定。[PR #7](https://github.com/hideyukiMORI/nene-loupe/pull/7)の現在のDraft/Ready、CI、レビュー、
統合状態はGitHubで確認する。ここでは未統合の変更を統合済みとは扱わない。
[Issue #5](https://github.com/hideyukiMORI/nene-loupe/issues/5)は
[PR #8](https://github.com/hideyukiMORI/nene-loupe/pull/8)でmainへ統合済み。

## 実装・検証済みの内容

- 240×64 DIPのルーペ、5形式、値クリックのコピー、成功・失敗表示。
- 320×392 DIPの設定モーダル、テーマ3択、常に最前面の即時切替と保存。設定スキーマ1。
- ドラッグ開始座標、タイトルのドラッグ帯、形式メニューのラジオ印、テーマ通知時の両窓再描画。
- 設定窓の初期・DPI変更・表示構成/work area通知後の配置を、対象work areaへ収める単一経路。
- ネイティブ形式メニューを表示前に取り込み除外し、重なった場合も背面色を継続採取する単一路。
- 4モニタ、120/144/168 DPI、画面端、仮想画面外、設定窓8隅、異DPI間4移動、
  同期通知後の再配置、クリップボード保全と全5形式、設定保存・再起動、不正設定5種。
- 対象PIDと実座標を限定した実入力で、メニュー選択・外クリック、ドラッグ、上端移動、
  設定窓のforeground取得とownerへの復帰を確認。pointerと元のforegroundは復元した。
- メニュー30回+warmupで全WDA 17、GDI・USER・handle・private bytesの最終差分0。
- 30秒warmup後の15分測定で、OS設定通知10回と設定窓開閉25回を実行。GDI 11→11、
  USER 14→14、handle 137→136。CPUは単一論理コア平均2.498%、20論理CPU全体0.1249%。
- hook寿命を狭める直前の同機能実装で`python -B eng/verify-window.py`の既定`full`は終了0。
  scope変更後は最終exeのメニュー30回回帰で補完した。最終`pwsh -NoProfile -File ./eng/check.ps1`も
  終了0（Conformance 0、Python 54/54、clean build 70/70、CTest 2/2、分岐92.50%、
  8件の実ツール検証（既存7組とヘッダ依存実測）、diff check 0）。設定スキーマ変更なし。Waivers: none。

詳細とコマンドは[検証証拠](../quality/gate-proofs.md)第9節、作業履歴は
[日報](../reports/2026-09-06.md)、再開情報は[引き継ぎ書](../handoffs/2026-09-06.md)に記録した。

## 検証境界

実OSテーマの大域切替と、実際のモニタ切断・work area変更は行っていない。system appearanceの実読込、
単体テストのdark/light切替、実メッセージによる再描画、synthetic通知後の再配置までを確認した。
資源測定の観測上限は15分であり、それを超える連続運転の保証ではない。HDR/ICCは仕様対象外。
