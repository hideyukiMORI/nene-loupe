# NeNe Loupe アプリアイコン

> Status: accepted / Issue #11

## 目的

Windowsのタスクバー、Alt+Tab、エクスプローラー、実行ファイルを参照するショートカットで、
NeNe Loupeを小さい表示でも識別できるアプリアイコンを定める。

## 正本

デザインの正本は[`app-icon.svg`](app-icon.svg)である。`src/app/NeNeLoupe.ico`はWindowsへ
組み込む派生資産であり、SVGを変更したときだけ再生成する。通常のconfigure/buildには画像変換を
組み込まず、レンダラーや画像ライブラリを実行時依存にしない。

ICOは32-bit画像を16、20、24、32、48、64、128、256pxの8サイズで持つ。各サイズをSVGから
直接描画し、小さい画像を256px画像の単純縮小だけで作らない。

## 形と色

- 256 SVG unitの全面を角半径20の2×2市松模様とする。左上・右下はoff-white `#F5F5F4`、
  右上はorange `#F97316`、左下はteal `#14B8A6`。
- 外周と前景は単色charcoal `#18181B`。外周は見える幅5、ルーペは中心(108,108)、半径58、
  線幅24、ハンドルは(155,155)から(208,208)へ線幅30で結ぶ。
- レンズ内を別色で塗らず、市松模様を見せる。文字、グラデーション、影、外部画像、フォントは使わない。

NENE-PIXELの4マス構成を参照し、単色の太いルーペを重ねた。16pxでも機能と同系統性を読み取りやすくし、
アプリ内テーマとは独立した1つの識別資産として扱う。

## 派生と確認

Chrome 152.0.7977.76をdevice scale factor 1、透明背景で使い、SVGのroot `svg`要素の
width/heightだけを各対象pxへ変えた一時入力を個別に描画する。内側の背景rectは256 SVG unitのまま
維持する。Pillow 12.3.0で32-bit ICOへ格納し、256px画像だけをPNG圧縮する。

```powershell
python -B eng/render-app-icon.py --chrome "C:\Program Files\Google\Chrome\Application\chrome.exe"
```

派生後は次を確認する。

- ICO directoryに8サイズが1件ずつあり、各画像がRGBAで、半透明または透明な角と不透明部を持つ。
  全面背景の小サイズではanti-aliasingによりalpha 0の画素が無い場合があるため、alphaの契約は
  最小値が255未満かつ最大値が255であることとする。
- clean buildしたexeに`GROUP_ICON`と`ICON`が入り、既存manifestと共存する。
- 実行したメインウィンドウの大・小クラスアイコンが埋込資産を指す。
- Shellがexeから大・小アイコンを抽出できる。
- 16〜64pxを明色・暗色の両背景で目視し、ルーペと2×2市松模様を判別できる。

採用SVGのSHA-256は`8DF19F2CDD5217AA177E32923C91E5AE03AD4EE85B29B8ED3772404347330A4A`、
派生ICOは`AF697DD19CDED727654E9CCD319DDBF7B510781B316EDE782D8B0BE090E0EA23`である。

スタートアップへの自動登録、ショートカット生成、通知領域常駐は本設計に含めない。
