# ADR 0004 — 最初の縦切りを単一のサンプル状態で通す

- 状態: 受理
- 日付: 2026-09-06
- Issue: #3
- 影響する規則: ARC-002 / ARC-004 / ARC-005 / ARC-007 / ARC-010 / ARC-011 / CPP-007 / CPP-012 / QLT-009 / QLT-013

## 文脈

PR #2で検査基盤を統合した。今回はFR-001〜005の最小の縦切りで、承認済みの各層に初めて具体的な責務が生じる。
160×64の初期寸法に収まるよう、7×7画素を8倍で描く。中心画素は黒白の枠で示す。
色形式はこの段ではHEXのみ。コピー・切替・設定・保存は別Issueで追加する。

## 決定

**ScreenSamplerPortから取得した最新の結果だけをLoupeControllerが所有し、描画のたびに不変のLoupeFrameを作る。**

- `core`: `RgbColor`は8bitの3成分からだけ生成する。HEXの判断はここに閉じる。
  `ScreenSample`は7×7画素だけを受け入れ、入力をコピーして所有し、読み取り専用のspanを返す。
- `application`: `ScreenSamplerPort`は`std::expected<ScreenSample, SamplingFailure>`を返す。
  `LoupeController`は初回と各更新でポートを一度呼ぶ。`LoupeFrame`がHEXまたは失敗文言を作り、
  UIは失敗理由を解釈しない。失敗時は前回画素を残さない。
- `adapters/win32`: GetCursorPos、画面DC、32bit top-down DIB、BitBlt、GdiFlushを一つの取得経路に置く。
  GDI資源はRAIIで解放する。画面外の領域は黒で埋め、中心位置をずらさない。取り込んだ画素をディスクへ保存しない。
- `ui/win32`: 枠なし・最前面の160×64 DIP窓。WM_NCHITTESTで移動し、Esc/Alt+F4で閉じる。
  メモリDCで完成させてから一括転送する。実画面の検証で更新途中の描画が見えたため導入した。
  ウィンドウの状態タイトルにも表示値を反映し、画面読み取りと外部検証から参照できるようにする。
  30msのWM_TIMERは更新のきっかけ。Per-Monitor v2マニフェストとWM_DPICHANGEDで寸法を更新する。
  Win32メッセージは開いた集合なので、未処理メッセージをDefWindowProcWへ渡す。
- `app`: wWinMainがポートと実装を結び、メッセージループと終了コードを所有する。
  UIからPostQuitMessageで終了コードを発行しない。
- `tests/unit`: OS非依存の中核テストを置く。許可グラフに検証専用の`unit_tests`を追加し、core/applicationへの依存だけを許す。
  これは製品の新しい層ではない。依存方向の例外も作らない。
- 中核の分岐測定は、同じソースと同じ単体テストを固定済みLLVMで計装コンパイルし、llvm-profdata/llvm-covで集計する。
  製品の正典ビルドは引き続きMSVC。計測ビルドは検証専用で、第二の製品実装ではない。
  core/applicationの全cppを計測し、分岐90%未満・計測対象欠落・空の集計を拒否する。
  対象内ヘッダの計測値も集計する。標準ライブラリとテストの分岐で中核の率を水増ししない。
  初回実測は9/10分岐（90%）。閉じたenumのswitchにLLVMが作る暗黙の範囲外分岐1本も
  分母へ残し、除外や不正enum値の注入で100%を作らない。

## 強制

承認グラフ、MSVCとclang-tidy、規約検査、単体テスト、分岐カバレッジをeng/check.ps1へ結ぶ。
新しいゲートは反例を実行してからactiveにする。UIの実機確認は別に記録する。

## 結果

状態の二重所有やUI内の色整形を作らずに最初の動作を確かめられる。
Win32境界の資源所有とメッセージ処理は実機検証が必要で、LLVM上の中核カバレッジはMSVCの全動作を証明しない。
HDR、ICC、画面外の実在しない画素は扱わない。自分の窓の上にカーソルを置いた場合は、その画面上の画素を取得する。
検証スクリプトの一時画像はUI確認の証拠であり、製品の画面保存機能ではない。

## 却下した選択肢

| 選択肢 | 却下の理由 |
| --- | --- |
| UIからGetPixelや色整形を直接呼ぶ | サンプリングと表示判断に第二の経路が生じる |
| 取得失敗時に古い色を表示し続ける | 最新のカーソル下の色と誤認させる |
| 全色形式・設定を同時に作る | 最初の縦切りの検証範囲を超える |
| MSVCの行カバレッジだけを分岐カバレッジと呼ぶ | QLT-009が求める指標と異なる |

## 参考

- [LLVM source-based coverage](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)
- [WM_DPICHANGED](https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-dpichanged)
