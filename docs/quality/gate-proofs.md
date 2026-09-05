# ゲート発火の証明 — NeNe Loupe

> 記録: 2026-09-06 / Issue #1・#3。以下は実行した結果のみ。
> 関連: QLT-007 / QLT-013 / ADR 0003

## 1. 環境と範囲

Windows x64。MSVC 19.44.35228、LLVM 19.1.5、CMake 3.31.6-msvc6、Ninja 1.12.1、Python 3.12.10。
これらは実測時の版であり、今後使う版の正本は `eng/tool-versions.json`。
Issue #1時点では製品コードは0。Issue #3の製品・表示確認は第6節へ分けて記録する。

## 2. activeの規則

| 規則 | 最小の違反 | 検証経路 | 実測 |
| --- | --- | --- | --- |
| QLT-002 | 未使用変数・lint違反・整形違反 | eng/prove-gates.py / 実CMakeビルド・clang-format | C4101 / clang-tidy診断 / 整形診断で非0。各復帰は0 |
| QLT-004 | 一行に詰めたmain | eng/prove-gates.py / clang-format --dry-run --Werror | clang-format-violationsで非0。元の整形は0 |
| QLT-009 | 失敗系の単体テストを省いて実行 | eng/coverage.py / 同一exeの別プロファイル | 8.33%でQLT-009。全テストへ復帰すると11/12分岐、91.67%で成功 |
| CNF-006 | 未定義ID・重複定義・状態不一致・証明行欠落・未置換値 | tests/conformance の document_checks 正例・反例 | 正例は指摘0、反例はCNF-006 |
| CNF-008 | Issue番号のないタスクコメント | tests/conformance の configuration_checks 正例・反例 | 番号付きは指摘0、番号なしはCNF-008 |

## 3. plannedの部分的な証明

| 規則 | 実測 |
| --- | --- |
| ARC-002 | CMakeの禁止依存でARC-002。元に戻すと構成・ビルドとも成功。字句検査でも逆方向includeと循環を拒否 |
| ARC-003 | coreのwindows.hを拒否。推移的includeまでは未保証 |
| ARC-007 | coreと製品テストのsystem_clockを拒否。adapters/win32では許可。文字列・コメントは検出しない |
| CPP-002 | 同じビルド経路の分岐漏れをC4062で拒否。元のソースは成功 |
| CPP-003 | メソッドを持つ公開メンバーをclang-tidyで拒否。aggregateだけの場合は漏れる |
| CPP-012 | 5引数をreadability-function-sizeで拒否。元のソースは成功 |
| CNF-001 | 禁止型名・別名・モジュール名を拒否。普通の型名は通る |
| CNF-002 | 複数型とファイル名不一致を拒否。主要宣言の完全な分類は未完了 |
| CNF-003 | waiverなし・Scope違い・ファイル単位の抑制を拒否。有効な行単位waiverは通る |
| CNF-004 | 期限切れ・必須項目欠落・不整合索引を拒否。期限当日と将来期限は通る |
| CNF-005 | 禁止設定ファイル名・/WX-を拒否。正常な設定は通る |
| CNF-007 | 設定参照の欠落・余分な設定を拒否。正常な参照は通る |
| GIT-003 | 日本語・Issue番号・破壊的変更フッタを検査。CIの実行証拠は未取得 |

## 4. 再現

```powershell
pwsh -NoProfile -File ./eng/measure-language.ps1
python -m unittest discover -s tests/conformance -v
pwsh -NoProfile -File ./eng/check.ps1
```

Phase 0の19結果は [phase0-results.json](phase0-results.json)。
実ツールの7反例と復帰結果はフルゲートから実行し、`out/proofs/results.json`に出力する。
反例は`out/proofs/`以下の使い捨てプロジェクトへ入れ、本体のソースは変更しない。
検査器の正例・反例はフルゲートから常に呼ぶ。
ローカルの単一ゲートは2026-09-06に終了コード0を確認した。
Issue #1の記録は検査器44テスト、C++のCTestは基盤1件。製品のテスト件数には数えない。

## 5. GitHubと実機

2026-09-06にruleset `main-quality-gate`（ID 22345027）を適用し、APIで読み戻した。
PR必須、GitHub Actionsの`check`必須（integration 15368）、strict up-to-date、
force push・削除禁止、未解決レビューの解消を要求する。bypass actorは空。
リポジトリ設定はsquashのみを許可し、件名はPRタイトルを使う。

最初の [CI実行](https://github.com/hideyukiMORI/nene-loupe/actions/runs/33981391008) は
PATH上の別CMake（3.31.6）を拾い、QLT-011で失敗した。版の比較を緩めず、
Visual Studio同梱のCMake/Ninjaをtoolchain.ps1で明示的に選択するよう修正した。
修正後の [CI実行](https://github.com/hideyukiMORI/nene-loupe/actions/runs/33981539645) は
コミット `55e6116` で成功した。ローカルと同じ単一コマンドで44テスト、CTest、6反例と復帰を実行した。
Ready後のhead更新を自動でDraftへ戻す処理や、Git規約の全条件の反例証明は未実装なので、
GITとQLT-012の規則全体はplannedを維持する。
Issue #1では表示・DPI・複数モニタ・クリップボードの実機検証は未実施だった。


## 6. 最初の製品実装（Issue #3 / ADR 0004、採取位置修正前の記録）

ローカルの `pwsh -NoProfile -File ./eng/check.ps1` は2026-09-06に終了コード0。
検査器54テスト、CTest2件、分岐カバレッジの反例と復帰、実ツール7組の反例と復帰を実行した。
追加したOSライブラリ経路も、coreへのuser32宣言でARC-002となり、撤去後に構成・ビルドが成功した。
CIの実行状態とレビューは [PR #4](https://github.com/hideyukiMORI/nene-loupe/pull/4) で確認する。

同じMSVCビルドで製品と`loupe_tests`をコンパイルし、CTestの2件が成功した。
`python eng/coverage.py`は固定LLVMの計装exeを同じcore/application全cppと単体テストから作り、
全テストで9/10分岐（90%）、失敗系を省いた実行で10%を測定し、後者をQLT-009で拒否した。
閉じたenumの範囲外に対するLLVMの暗黙分岐も分母へ残す。core/application以外の分岐を加算しない。
測定器の正例・反例は対象欠落・重複・空・不正な件数・閾値境界・対象内ヘッダ・対象外ソースを含む。
実プロファイル・JSONは`out/coverage/`へ出す。

実Windowsの確認用スクリプトは`python -B eng/verify-window.py`。
ロックされていない対話デスクトップで動かし、検証中は手動のマウス操作を止める。
ポインタを一時移動し、自作の色窓とルーペだけを操作・終了し、元のポインタとフォーカスへ戻す。
保存画像はルーペのクライアント領域だけ。CIや単体テストからは実行しない（QLT-013）。

Windows x64、初期モニタ120 DPI（125%）で次を実測した。

- 枠なし・最前面、160×64 DIP＝200×80物理画素。
- 黒・白・#FF8000・#00010FでタイトルのHEXとルーペ中心／左上の画素が一致。
- 7×7の異なる色を描いたfixtureで49画素の位置と中心色#60B780を確認。
- ネイティブドラッグで(100,100)から(180,150)へ移動。EscとAlt+F4は終了コード0。
- 3秒間更新後のGDI資源の安定値は8→8。長時間のリーク検証ではない。
- 更新途中の表示が見えたため、描画をメモリDCからの一括転送へ修正した。
  修正後のルーペ画像は`out/window-verification/loupe.png`と`grid.png`で目視確認した。

接続モニタは4台、仮想画面は(-3840,-1440)から11520×3600。
複数モニタをまたぐ検証はポインタ操作の競合で中断しており、成功とは扱わない。
画面端・DPI変更・複数モニタ・HDR/ICC・長時間のCPU/GDI使用量は未確認。
クリップボード、形式切替、設定、保存は今回未実装。Waivers: none。


## 7. ルーペ直下の採取（Issue #3 / ADR 0005）

hideの実機確認を受け、採取位置をルーペ中心へ修正した。
`python -B eng/verify-window.py`はポインタ・実キーボードを自動操作せず、
テスト窓の重なりと位置だけを変更して終了コード0となった。第6節の操作競合による保留は解消した。

| モニタの物理範囲（左・上・右・下） | DPI | 実測 |
| --- | --- | --- |
| -3840, 0, 0, 2160 | 144 | 背面4色一致・レンズ中央・隣の画素への移動 |
| 3840, 0, 7680, 2160 | 168 | 同上 |
| 3818, -1440, 6378, 0 | 144 | 同上 |
| 0, 0, 3840, 2160 | 120 | 同上 |

各モニタへ移動後の窓寸法は160×64 DIPを維持し、WS_POPUP・最前面と取り込み除外値17を確認した。
背面は#000000 / #FFFFFF / #FF8000 / #00010F。7×7の色fixtureの中央は#60B780、
窓を右へ1物理画素動かすと#64B480、下へ1物理画素で#7CA280へ変化した。
GDI資源の安定値は3秒前後で8→8、Escメッセージで終了コード0。
結果は`out/window-verification/lens-results.json`。取り込み除外が有効なので、
現在の検証はスクリーン画像を保存しない。画像による拡大描画の確認は修正前の第6節の範囲に限る。

同じ中核ソースの計測は11/12分岐＝91.67%、反例は8.33%でQLT-009となった。
位置の受け渡し・負の座標・位置未取得時に取得しないこと・位置エラーからの復帰を単体テストした。
画面端・長時間の性能・HDR/ICCは未確認。コピー・形式切替・設定・保存は未実装。
