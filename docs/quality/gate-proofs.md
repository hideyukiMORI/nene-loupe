# ゲート発火の証明 — NeNe Loupe

> 記録: 2026-09-06 / Issue #1・#3・#5。以下は実行した結果のみ。
> 関連: QLT-007 / QLT-011 / QLT-013 / ADR 0003

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
| QLT-007 / QLT-011 | 実MSVCの`/showIncludes`とCMake/Ninjaのprefix一致を検査。公開ヘッダだけのABI変更で関連3翻訳単位を再コンパイルし、無関係な1翻訳単位を再コンパイルせず、リンクと実行に成功 |
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
実ツールの7反例と復帰結果、ヘッダ差分依存の実測はフルゲートから実行し、
`out/proofs/results.json`に出力する。
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

## 8. MSVCのヘッダ差分依存（Issue #5）

固定したMSVC 19.44.35228の`cl /showIncludes`を`VSLANG=1033`と`1041`で直接実行した。
この端末の固定toolsetに存在する`clui.dll`は`1041`（日本語）だけで、`1033`（英語）は無い。
両方の`VSLANG`で実際の出力はUTF-8の`メモ: インクルード ファイル:  `だった。
`VSLANG=1033`だけを根拠に英語版を実測したとは扱わない。

修正前はconsole code page 932でCMakeの検出prefixが文字化けし、同じprefixが`rules.ninja`へ渡り、
`ninja -t deps`は全objectを`#deps 0`と報告した。console入出力をUTF-8へ固定してから
新しい子`pwsh -NonInteractive`で構成すると、子プロセスのcode pageは65001、CMakeとNinjaの
prefixは実出力と一致し、fixtureの4 objectは期待する1件または2件のヘッダ依存を保持した。

`eng/prove-gates.py`はこの実MSVC/CMake/Ninja経路を毎回検査する。fixtureの公開ヘッダだけで
関数引数のaliasを`int`から`long long`へ変え、`consumer_a.cpp`、`consumer_b.cpp`、
`provider.cpp`のobjectが変わり、公開ヘッダを含まない`main.cpp`のobjectが変わらないことを
更新時刻とSHA-256で確認した。その後のリンクとexe実行は終了コード0で、依存表も維持された。
fixtureの`rules.ninja`だけを、日本語の実prefixをUTF-8のbyte列としてCP932で誤復号した
合成prefixへ置き換えた反例は、初回ビルドに成功しても4 objectすべての依存が0件となった。
正しいprefixへ復帰した上記の差分ビルドは成功した。
この変更を含む`python eng/prove-gates.py`単独実行は終了コード0。
`pwsh -NoProfile -File ./eng/check.ps1`も終了コード0で、検査器54件、CTest 2件、
11/12分岐（91.67%）、実ツール7反例と復帰、ヘッダ差分依存のproofを実行した。

英語resourceを持つMSVCでの実出力はこの端末では採取できないため、prefixを言語別に決め打ちせず、
実際の`cl`出力、CMakeの検出値、Ninjaの値を照合する同じproofを英語CIでも実行した。
[PR #8のCI](https://github.com/hideyukiMORI/nene-loupe/actions/runs/34015538775)は、
英語MSVCの実prefix `Note: including file: `、code page 65001、実ツール8反例と復帰を確認して
全体ゲート終了0となった。HEAD `df30ff2`をsquash統合し、main `2c51bf9`でIssue #5を閉じた。Waivers: none。

## 9. デザインと操作（Issue #6 / ADR 0006）

2026-09-06 05:35 JST、Claude Codeが最終ソースに対して全体ゲートを実行し、サナが生ログの終了0を確認した。
CTest 2/2、分岐111/120＝92.50%、不足実行18.33%をQLT-009で拒否、実ツール7反例と復帰が成功。
適合違反0、MSVC・clang-tidy・clang-formatのゲートも成功した。

同日05:39、サナが `python -B eng/verify-window.py` を実行し終了0を確認した。
検証はbuildからコピーした専用exeと隔離したLOCALAPPDATAを使用し、テスト窓だけをPIDで特定する。
実ポインタ・キーボードを操作しない。コピー検証は元のクリップボードをメモリ上で退避し、
所有者がテスト窓のままの場合に復元する。対応外形式がある場合は変更前に中止する。

- 第7節と同じ4モニタ、120/144/168 DPIで240×64 DIP、4色、1物理画素移動、ヒット領域を確認。
- 背景のダブルクリックメッセージで窓位置・寸法が変わらないことを確認。Aero Snapの手動操作は未確認。
- 実クリップボード値は `#FF8000`、`0, 50, 100, 0`、`30, 100%, 50%`、
  `30, 100%, 100%`、`255, 128, 0`、巡回後 `#FF8000`。
- 設定窓の取り込み除外17、親窓無効化、最前面OFF/ON/OFFを親・設定窓で確認。
- dark/light/system/darkを選択し、Esc後の親窓再有効化を確認。
- 保存4行がschema=1/theme=dark/format=hex/layer=normalと一致し、再起動で最前面OFFを復元。
- 未対応版・テーマ・形式・不足行・余分な行の5ケースは既定値で起動し、無操作では原本を上書きしない。
- 短時間のGDI資源11→11、Esc終了0。証拠は `out/window-verification/lens-results.json`。

補助診断は色変換20,485比較で不一致0、Win32保存adapter10/10、実HWNDのUnicodeコピーと占有失敗を確認した。
GDI rendererのダーク／ライト・コピー成否・設定失敗の画像を確認した。これらは全体ゲートの代替ではない。
手動の操作感、右クリックメニュー、フォーカスの詳細、OSテーマ変更追従、画面端、長時間性能は未確認。
HDR/ICCは仕様対象外。Waivers: none。

### 9.1 レビュー後のUI修正と画面端回帰

同日の再開後、Issue #6 / ADR 0006の実装を静的レビューし、次を修正した。

- 操作領域からドラッグへ移る`WM_NCLBUTTONDOWN`に、開始点のスクリーン座標を渡す。
- 設定窓のタイトル帯だけを`HTCAPTION`、操作領域を`HTCLIENT`として扱う。
- 設定窓をownerのモニタとDPIで生成し、初期配置と`WM_DPICHANGED`の双方を同じwork areaへの
  配置経路へ通す。
- `WM_SETTINGCHANGE`でルーペと開いている設定窓の両方を再描画する。
- 形式メニューの現在値を通常のチェックではなく、単一選択のラジオ印で示す。

設定窓の旧配置は、モニタ移動後にDPIで拡大した最終寸法を再クランプしなかった。
旧exe（SHA-256 `4454C6D6C8A39C679B5D0645CEFE27C49C7315304DA4DA1E9BB54AA8099AB02B`）へ
次を実行すると終了1となった。

```powershell
python -B eng/verify-window.py --suite geometry --executable out/window-verification/runtime-diagnostics/NeNeLoupe-runtime.exe
```

144 DPIモニタのwork area `[-3840, 0, 0, 2160]`でownerを右下
`[-360, 2064, 0, 2160]`へ置くと、設定窓は`[-400, 1670, 80, 2258]`となり右・下へはみ出した。
修正版exe（SHA-256 `38B2393257EFD509379C26B590811892FC6AAEAAD1BD6BDBF4EF4AD17E81698A`）では
同じgeometry suiteが終了0となった。

`eng/verify-window.py`は既定の`full`と、クリップボードを扱わない`geometry`を明示選択できる。
両suiteは、従来の4モニタ検証に加え、次を共通で実測する。

- 4モニタの物理的な左右端に採取中心を置き、既知の中心色が一致すること。
- 採取中心を仮想デスクトップ外へ出すと、古い色を残さず「画面取得不可」になること。
- 各モニタのwork area左上・右下、計8配置で、設定窓の最終DPI寸法と配置、取り込み除外17、
  ×のヒット領域による閉じる操作、ownerの再有効化を確認すること。
- 右クリックの形式メニューが現れ、ownerモニタのwork area内に収まり、安全に閉じられること。

`geometry`はクリップボードの退避・コピー、設定変更と保存、再起動復元、不正設定拒否を実行せず、
JSONの`suite`と`skippedChecks`へ明記する。既定の`full`はgeometryを含む上位集合である。
最終ビルドに対する次の既定コマンドは終了0となり、5形式の実クリップボード、設定保存、再起動復元、
不正設定5種まで成功した。結果は`out/window-verification/lens-results.json`、geometryだけの結果は
`out/window-verification/geometry-results.json`にある。

```powershell
python -B eng/verify-window.py
```

最初の再実行では、クリップボードに`GlobalSize`で安全に退避できない
`EnterpriseDataProtectionId`形式があり、`ClipboardSnapshot`が変更前に終了1で停止した。
これは製品のコピー失敗ではない。後の再実行時には全形式を安全に退避でき、値検証後の復元を含めて
既定`full`が終了0となった。安全停止の条件は緩めていない。

### 9.2 5分間の資源測定

上記旧exeの専用コピーと隔離した`LOCALAPPDATA`だけを使い、300秒間、30秒ごとに資源を測定した。
CPU時間は合計6.0625秒で、単一論理コア換算の平均は約2.0%、20論理CPU全体では約0.10%。
GDI objectは11→11で変化しなかった。150秒時点で`WM_SETTINGCHANGE`を1回送った後、
USER objectは14→16、handleは136→159、working setは+2,416,640 byte、private bytesは
+258,048 byteへ一段増え、その後120秒はUSER objectとhandleが横ばいだった。
結果は`out/window-verification/runtime-diagnostics/endurance-results.json`。

この測定が示すのは旧exeの5分間と通知後120秒の範囲だけであり、長時間リークが無いことの保証ではない。
実ポインタ・実キーボード、利用者のOSテーマ設定は変更していない。Aero Snapを含む実ドラッグ、
OSテーマを実際に切り替えた追従、実フォーカス復帰、長時間の連続運転は引き続き手動確認が必要である。
今回の修正後に次の全体ゲートを実行し、終了0となった。ログは`out/check-issue6.log`。

```powershell
pwsh -NoProfile -File ./eng/check.ps1
```

Conformance違反0、Python gate tests 54/54、clean build 68/68、CTest 2/2、分岐111/120＝92.50%、
実ツールによる反例・復帰7組が成功した。設定スキーマ変更なし。Waivers: none。
