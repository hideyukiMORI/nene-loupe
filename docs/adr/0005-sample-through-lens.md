# ADR 0005 — ルーペ自身を除外し、その直下を採取する

- 状態: 受理
- 日付: 2026-09-06
- Issue: #3
- 影響する規則: ARC-001 / ARC-004 / ARC-007 / ARC-010 / ARC-011 / CPP-001 / CPP-002 / QLT-009 / QLT-013
- 置換: ADR 0004のカーソル周辺を取得する決定のみ。層・状態所有・単一の取得と描画経路は維持。

## 文脈

hideの実機確認で、欲しい動作はルーペを置いた場所を透かして拡大することであり、
マウス位置への追従ではないと確定した。FR-003と入力契約を修正する。

## 決定

ルーペ中心の物理スクリーン座標を`ScreenPosition`として明示的に渡し、
`ScreenSamplerPort::sample(position)`だけが背面の7×7画素を取得する。
GetCursorPosを製品から削除し、旧取得モードを残さない。

- UIは描画に使うレンズ中心をClientToScreenで変換する。位置はUIの配置入力であり、色の判断ではない。
  `LoupeController::refresh(expected<ScreenPosition, SamplingFailure>)`へ毎回渡す。
  変換失敗もこの経路で最新の失敗状態へ置き換え、古い画素を残さない。
- Controllerの構築時は位置未取得の結果を持ち、UIの最初の更新で初めてポートを呼ぶ。
- UIは表示前にSetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)を設定する。
  失敗時は閉じたWindowFailureで起動を中止する。窓を隠して取得する代替経路は作らない。
- BitBlt、GDI資源所有、画素の不変性、HEXの整形、描画の一括転送は従来どおり。
- 対応環境をWindows 10 version 2004以降・x64・DWM有効とする。
  古いWindowsでは同フラグが黒塗り相当になるため対応とは呼ばない。
  一般の画面キャプチャにも窓が映らなくなることをREADMEへ明記する。

## 証明

Windows上の二枚の自作色窓を重ね、上の窓だけを除外すると、画面DCの色が上の青から
背面の#FF8000へ変わることを実測した。マウスの自動操作は不要だった。
製品ではルーペを自作色窓へ重ね、背面の変化と移動後の中央色を確認する。
単体テストは位置の受け渡し、負の座標、位置取得失敗と復帰を追加する。

## 却下

- 毎回Hide/Showする方法: ちらつきやフォーカスへの影響を生む。
- 半透明の色を採取する方法: レンズ自身の描画色が混ざる。
- Magnification APIとBitBltの併用: 拡大像と色番号に別の取得経路が生じる。

## 参考

- [SetWindowDisplayAffinity](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowdisplayaffinity)
