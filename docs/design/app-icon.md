# NeNe Loupe アプリアイコン

> Status: accepted / Issue #9

## 目的

Windowsのタスクバー、Alt+Tab、エクスプローラー、実行ファイルを参照するショートカットで、
NeNe Loupeを小さい表示でも識別できるアプリアイコンを定める。

## 正本

デザインの正本は[`app-icon.svg`](app-icon.svg)である。`src/app/NeNeLoupe.ico`はWindowsへ
組み込む派生資産であり、SVGを変更したときだけ再生成する。通常のconfigure/buildには画像変換を
組み込まず、レンダラーや画像ライブラリを実行時依存にしない。

ICOは透明な32-bit画像を16、20、24、32、48、64、128、256pxの8サイズで持つ。
各サイズをSVGから直接描画し、小さい画像を256px画像の単純縮小だけで作らない。

## 形と色

- charcoal `#283747`の丸いルーペを主形状とする。
- pale teal `#D9EEEC`のレンズ内に、淡いtealの3×3画素群を置く。
- 中央の採取画素をcoral `#EF856E`で示す。中央は32 SVG unitとし、16px出力でも2px残す。
- pale `#C6D3D8`の外縁で暗いタスクバーから主形状を分離する。
- 透明背景、文字なし、グラデーションなし、影なしとする。

初期の角形案は16pxで機能を読み取りやすかったが、道具の説明図に寄りすぎた。丸いルーペと
抑えたteal/coralを採り、機能の識別と「おしゃれな感じ」を両立する。アプリ内テーマとは独立した
識別資産なので、テーマごとに別アイコンを持たない。

## 派生と確認

採用時はChrome 152.0.7977.76をdevice scale factor 1、透明背景で使い、SVGのwidth/heightを
各対象pxへ変えた一時入力を個別に描画した。Pillow 12.3.0で32-bit ICOへ格納し、256px画像だけを
PNG圧縮した。再生成は次の手動コマンドに限定し、通常buildの依存にはしない。

```powershell
python -B eng/render-app-icon.py --chrome "C:\Program Files\Google\Chrome\Application\chrome.exe"
```

派生後は次を確認する。

- ICO directoryに8サイズが1件ずつあり、各画像がRGBAと透明領域を持つ。
- clean buildしたexeに`GROUP_ICON`と`ICON`が入り、既存manifestと共存する。
- 実行したメインウィンドウの大・小クラスアイコンが埋込資産を指す。
- Shellがexeから大・小アイコンを抽出できる。
- 16〜64pxを明色・暗色の両背景で目視し、coral中心とルーペの輪郭を判別できる。

採用SVGのSHA-256は`E64718954E579A8B7429976181358C2D18147A181A4075F5EFC537E5C13F033C`、
派生ICOは`D6DD319A1B69C2CAB103F1A2CEF5D2B5667AC4DACB7FF50121148BADE22960E5`である。

スタートアップへの自動登録、ショートカット生成、通知領域常駐は本設計に含めない。
