# NeNe Loupe アプリアイコン

> Status: accepted / Issue #15

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

- 256 SVG unitの全面を角半径22の2×2市松模様とする。左上・右下はoff-white `#F5F5F4`、
  右上はorange `#F97316`、左下はteal `#14B8A6`。
- 外周と前景は単色charcoal `#18181B`。外周は見える幅1.5。ルーペは中心(116,116)、外半径78、
  内半径48。リングと幅30の持ち手を`fill-rule=evenodd`の1つのシルエットで描く。
- レンズ内を別色で塗らず、市松模様を見せる。文字、グラデーション、影、外部画像、フォントは使わない。

NENE-PIXELの4マス構成を参照し、単色ベタ塗りのルーペを重ねた。太い前景と競っていた外枠を
1.5 unitへ軽くし、リングと持ち手を接線でつながる一体形へ整えた。レンズ中心を4マス交点へ寄せ、
レンズ内のorangeとtealを釣り合わせる。アプリ内テーマとは独立した1つの識別資産として扱う。

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

採用SVGのSHA-256は`2B9F0B6C72B6C518FFE0725566F33F59CFBE05BCA2DCD1AED3F0AB52D627A7A3`、
派生ICOは`02035547F9C94FA68961F82A4E54DF0C1CF751E62900FA22DA999F7A04D0C22E`である。

スタートアップへの自動登録、ショートカット生成、通知領域常駐は本設計に含めない。
