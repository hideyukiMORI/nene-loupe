# ゲート発火の証明 — NeNe Loupe

> 記録: 2026-09-06 / Issue #1・#3・#5・#6。以下は実行した結果のみ。
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
GDI objectは11→11で変化しなかった。150〜180秒の間にUSER objectは14→16、handleは
136→159、working setは+2,416,640 byte、private bytesは+258,048 byteへ一段増え、
その後120秒はUSER objectとhandleが横ばいだった。
結果は`out/window-verification/runtime-diagnostics/endurance-results.json`。

この測定では150秒時点に`WM_SETTINGCHANGE`を`PostMessageW`で送ろうとしたが、同期送信専用の
システムメッセージなので戻り値は偽だった。したがって上記の資源増加をOS設定変更通知の結果とは扱わない。
後の15分測定の初回も同じAPI選択をassertして終了1となり、`finally`が製品を正常終了した。
製品の異常終了ではない。再測定は`SendMessageTimeoutW`と対象PID・HWND・終了コードの監視を使う。

この測定が示すのは旧exeの5分間と、資源が一段増えた後120秒の範囲だけであり、
長時間リークが無いことの保証ではない。
この旧測定では実ポインタ・実キーボード、利用者のOSテーマ設定は変更していない。
今回の修正後に次の全体ゲートを実行し、終了0となった。ログは`out/check-issue6.log`。

```powershell
pwsh -NoProfile -File ./eng/check.ps1
```

Conformance違反0、Python gate tests 54/54、clean build 68/68、CTest 2/2、分岐111/120＝92.50%、
実ツールによる反例・復帰7組が成功した。設定スキーマ変更なし。Waivers: none。

### 9.3 残操作、ポップアップ自己採取、15分間の資源測定

専用fixtureと対象PID・foreground・実座標を毎回確認し、約4秒の実入力を行った。右クリックメニューで
HEXからRGBを選択すると表示は`255, 128, 0`へ変わり、同じfixtureへの外クリックでメニューが閉じた。
形式チップからのドラッグで窓は`[120,120,420,200]`から`[208,173,508,253]`へ移動した。
設定窓はforegroundを取得し、×で閉じるとownerへforegroundが戻った。レンズを上端へ動かした結果は
`[1880,0,2180,80]`、`IsZoomed=false`で、固定`WS_POPUP`はAero Snapによる最大化を受けず通常移動した。
終了時に元のpointer位置`[3014,1014]`とforeground HWNDを復元した。キーボード入力とOS設定変更はない。
結果は`out/window-verification/input-ux/results.json`。

開いている設定窓を120 DPIと168 DPIのモニタ間で4回移動した。最終寸法は120 DPIで400×490物理画素、
168 DPIで560×686物理画素となり、各移動は1つの安定状態へ収束してwork area内に収まった。
実OSのモニタ構成やwork areaは変更せず、画面外へ置いた設定窓へ`WM_DISPLAYCHANGE`と
`WM_SETTINGCHANGE/SPI_SETWORKAREA`を同期送信する回帰を追加した。修正前exe
（SHA-256 `2C49342ECE16DD94E0E2AE54CA035E04D8515E26B003C205F64914C2B42445F6`）では
`[100,2120,500,2610]`のまま残り終了1、修正後は`[100,1610,500,2100]`へ再配置され終了0となった。
負例は`out/window-verification/regression-red.json`、DPIと通知の診断は
`out/window-verification/notification-probe/results.json`。

同じ修正前exeを右端へ置くと、ネイティブ形式メニュー`[7484,1052,7680,1248]`がレンズ
`[7417,1024,7529,1136]`へ重なり、既知背景`#FF8000`が表示中だけ`#FE7F00`となった。
これはメニューの影を自己採取した再現である。`WH_CALLWNDPROC`で表示前にメニューへ
`WDA_EXCLUDEFROMCAPTURE`（17）を設定する単一路へ修正した。最終exe
（SHA-256 `8B9717FE743296FCB9C192CED9A6CB9D80D541508A6B9E4A5CA5FE989F57C85E`）では、
同じ重なりでWDA 17、メニュー表示中も`#FF8000`を維持し、背面fixtureを`#00FF00`へ変えると
メニューを開いたまま新色へ追従し、キャンセル後も`#00FF00`だった。採取の凍結は使っていない。
最終的にhookの寿命を`TrackPopupMenu`の間だけへ狭めたexe（SHA-256
`7227823C13331DD1F60A6F2A53153AA397B24C1954C613C3F8C8C46E5859D85B`）で、30回の開閉と
1回のwarmupを再実行した。GDI・USER・handle・private bytesの最終差分0、working setは
+77,824 byte。全30回のWDAは17、製品終了0。結果は`out/window-verification/menu-resource/results.json`。
WDA設定失敗時にメニューを表示しない経路の単独証拠は`out/popup-affinity-probe/destroy-results.json`。

修正前exeと隔離した`LOCALAPPDATA`を固定し、30秒warmup後に900.11秒測定した。
対象SHA-256は`2C49342ECE16DD94E0E2AE54CA035E04D8515E26B003C205F64914C2B42445F6`。
`SendMessageTimeoutW`による`WM_SETTINGCHANGE`を10回、設定窓の開閉を25回行い、対象PID・HWNDと
終了コードを監視した。GDIは11→11、USERは14→14、handleは137→136、private bytesは
+172,032 byte、working setは+704,512 byte。CPU時間は22.484375秒で、単一論理コア換算の平均
2.498%、20論理CPU全体で0.1249%。各設定窓burst直後のUSER 16は次のsampleで14へ戻り、
private bytesも2,109,440〜2,347,008 byteの範囲で単調増加しなかった。製品とprobeは終了0。
測定中15:36:39.581〜15:36:47.372 JSTに別PIDのpopup単独probeを実行したため、純粋idle測定とは
扱わない。結果は`out/window-verification/endurance-15m/results.json`。これは15分の観測であり、
それを超える連続運転の保証ではない。

hook寿命を狭める直前の同じ機能実装（SHA-256 `8B9717FE743296FCB9C192CED9A6CB9D80D541508A6B9E4A5CA5FE989F57C85E`）に
対する次の実Windows検証は終了0。既定`full`は4モニタ、画面端・仮想画面外、
設定窓8配置、同期通知再配置、ポップアップのWDAと背面色追従、クリップボード保全と5形式、
設定保存・再起動、不正設定5種を含む。証拠は`out/window-verification/lens-results.json`。
その後のhook寿命変更は、上記の最終SHA
`7227823C13331DD1F60A6F2A53153AA397B24C1954C613C3F8C8C46E5859D85B`でポップアップ30回のWDA・閉鎖・
資源回帰を再実行して補完した。scope変更後に既定`full`を再実行したとは扱わない。

```powershell
python -B eng/verify-window.py --suite geometry --executable build/NeNeLoupe.exe
python -B eng/verify-window.py --executable build/NeNeLoupe.exe
python -B out/window-verification/menu-resource-probe.py
pwsh -NoProfile -File ./eng/check.ps1
```

最終全体ゲートは2026-09-06 15:54:31〜15:55:16 JSTに終了0。ログは
`out/check-issue6-final.log`。Conformance違反0、Python gate tests 54/54、clean build 70/70、
CTest 2/2、分岐111/120＝92.50%、8件の実ツール検証（既存7組とヘッダ依存実測）、
diff checkが成功した。
clean rebuild後のexe SHA-256は`7A0A9B2647D8D1E8485243BB54174B0E76BB85326F34EFD1C141C80A39881643`。
最終popup回帰後にソース変更はなく、popup回帰済みexeとの差はclean rebuildによるバイナリ差である。
設定スキーマ変更なし。Waivers: none。

実OSテーマの大域切替と、実際のモニタ切断・work area変更は行っていない。system appearanceの実読込、
単体テストのdark/light切替、実`WM_SETTINGCHANGE`による両窓再描画、synthetic通知の再配置を組み合わせて
境界を確認した。これら大域変更を伴う2項目と15分を超える連続運転が、今回の検証上限である。

### 10. アプリアイコン（Issue #9）

変更前の`build/NeNeLoupe.exe`を`llvm-readobj --coff-resources`で調べるとmanifest 1件だけで、
ICONとGROUP_ICONは無かった。起動中PID 34232の`NeNeLoupe.Window`も`GCLP_HICON`と
`GCLP_HICONSM`がともに0だった。

採用SVGから16、20、24、32、48、64、128、256pxを個別に透明RGBA描画した。generatorを3回実行し、
ICOのSHA-256が毎回`D6DD319A1B69C2CAB103F1A2CEF5D2B5667AC4DACB7FF50121148BADE22960E5`に
一致した。全8画像は32-bitとalpha範囲0〜255を持ち、16〜128pxはDIB、256pxはPNGである。

起動中の旧exeを維持したまま、`out/build-icon`へ隔離buildした。RCを含む71/71とbuild graphの
conformance違反0を確認した。新exeはICON 8件、GROUP_ICON 1件、manifest 1件を持つ。
`ExtractIconExW`によるShell抽出はlarge 32×32、small 16×16だった。新exeを短時間実行した
120 DPIの窓ではクラスアイコンがlarge 40×40、small 20×20で`GetSystemMetricsForDpi`の期待値と
一致し、製品は終了0、元のforegroundも復元した。取得したHICONを`DrawIconEx`で明暗背景へ描いた
結果は`out/icon-design/win32-icon-preview.png`。

```powershell
python -B eng/render-app-icon.py --chrome "C:\Program Files\Google\Chrome\Application\chrome.exe"
cmake --fresh -S . -B out/build-icon -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build-icon --clean-first
python -B eng/conformance.py --build-dir out/build-icon
python -B eng/verify-window.py --executable out/build-icon/NeNeLoupe.exe
```

最後の実Windows fullは終了0で、4モニタ、画面端、設定窓、popup、全5形式のコピー、設定保存と
不正設定拒否を含む。スタートアップ登録やショートカット生成は検証対象ではなく、実装もしていない。
設定スキーマ変更なし。Waivers: none。

### 11. アイコン改訂（Issue #11）

採用SVGは全面の角丸2×2市松模様と、単色charcoalのルーペで構成する。root `svg`要素の寸法だけを
16、20、24、32、48、64、128、256pxへ変えて個別描画し、内側の背景rectは256 SVG unitを維持した。
同じSVGから2回生成したICOはSHA-256
`AF697DD19CDED727654E9CCD319DDBF7B510781B316EDE782D8B0BE090E0EA23`で一致した。
SVGは`8DF19F2CDD5217AA177E32923C91E5AE03AD4EE85B29B8ED3772404347330A4A`。

8画像は32-bit RGBAで、16〜128pxはDIB、256pxはPNG。alpha範囲は16px 202〜255、20px 157〜255、
24px 137〜255、32px 70〜255、48/64/128/256px 0〜255だった。全面背景の小さい角では
anti-aliasingによりalpha 0が無いことが正常であるため、最小値255未満かつ最大値255を契約とする。
証拠は`out/icon-design-revision/ico-verification.json`。

隔離した`out/build-icon-revision`はRCを含む71/71で、build graphのconformance違反0。exeはICON 8件、
GROUP_ICON 1件、manifest 1件を持つ。Shell抽出はlarge 32×32、small 16×16。隔離`LOCALAPPDATA`で
短時間起動した120 DPIの窓ではクラスHICONが40×40と20×20で期待寸法と一致し、終了0、foreground復元に
成功した。結果は`out/icon-design-revision/win32-results.json`、実HICONの明暗描画は
`out/icon-design-revision/win32-icon-preview.png`。

```powershell
python -B eng/render-app-icon.py --chrome "C:\Program Files\Google\Chrome\Application\chrome.exe"
cmake --fresh -S . -B out/build-icon-revision -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build-icon-revision --clean-first
python -B eng/conformance.py --build-dir out/build-icon-revision
```

最終全体ゲートとCIはIssue #11のPRを進行状態の正本として確認する。設定スキーマ変更なし。
Waivers: none。
