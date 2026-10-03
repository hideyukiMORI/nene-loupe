# Microsoft Store の掲載文と年齢区分の根拠 — NeNe Loupe

> Issue #51 / ADR 0008。Partner Center への入力は hide が行う。ここは入力する文面の正本であり、
> 入力欄の名前と字数の上限は Partner Center の画面で確かめること（**下の「確かめていないこと」を参照**）。

掲載の言語は英語と日本語の 2 つ（hide の決定・2026-10-03）。アプリの表示は英語だけである（Issue #52）。
各文は README・SPECIFICATION・実装と突き合わせた。できないことは書いていない。

## 英語（en-US）

### Product name

```text
NeNe Loupe
```

### Short description

```text
A tiny screen loupe and colour picker. See the pixels behind the lens and copy the colour of the centre pixel.
```

### Description

```text
NeNe Loupe is a tiny, frameless screen loupe and colour picker for Windows.

Drag it over anything on your screen. The lens shows the 7×7 pixels directly behind it, magnified 8×, and the value next to it is the colour of the centre pixel. Click the value to copy it.

• Exact pixels. The 8× view is not smoothed, so every screen pixel is a sharp square.
• Five colour formats. RGB, HEX, CMYK, HSL and HSV. Click the format label to cycle through them, or right-click to pick one.
• Small on purpose. A 240×64 window that stays out of the way. Drag it anywhere; press Esc to close.
• Dark and light themes, or follow Windows.
• Always on top, with a switch to turn it off.
• Sharp on high-DPI displays and correct across multiple monitors.
• Private. No network connections, no account, no telemetry. The only thing saved is three settings in a local file.

Good to know

• The loupe looks through itself. Its window is excluded from screen capture so that it can see what is behind it, which means it does not appear in screenshots, screen recordings or screen sharing.
• CMYK is a naive conversion without ICC profiles. HDR and wide-gamut colour are not supported; values are 8-bit sRGB.
• Windows 10 version 2004 or later, 64-bit.
• Open source under the MIT License: https://github.com/hideyukiMORI/nene-loupe
```

### Product features

```text
8× loupe of the 7×7 pixels behind the lens, with no smoothing
Copy the centre colour with one click
RGB, HEX, CMYK, HSL and HSV
Dark and light themes, or follow Windows
Always on top, switchable
Per-monitor DPI aware; works across multiple monitors
No network connections, no account, no telemetry
```

### Search terms

```text
colour picker
color picker
screen loupe
magnifier
pixel
hex colour
eyedropper
```

### Screenshot captions

| 画像 | Caption |
| --- | --- |
| `docs/images/store/store-1-loupe-dark.png` | `The loupe magnifies the pixels behind the lens and shows the colour of the centre pixel.` |
| `docs/images/store/store-2-loupe-light.png` | `Dark and light themes. Colours in HEX, RGB, CMYK, HSL or HSV.` |
| `docs/images/store/store-3-settings.png` | `Settings: theme and always on top.` |

## 日本語（ja-JP）

### 製品名

```text
NeNe Loupe
```

### 短い説明

```text
小さな画面ルーペ兼カラーピッカー。レンズの真下の画素を拡大し、中心の画素の色をコピーできます。
```

### 説明

```text
NeNe Loupe は、Windows 用の小さな枠なしの画面ルーペ兼カラーピッカーです。

画面の上の好きな場所へドラッグしてください。レンズは真下の 7×7 画素を 8 倍に拡大して表示し、その横に中心の画素の色を表示します。値をクリックするとコピーできます。

• 画素をそのまま表示。8 倍の表示は補間しないので、画面の 1 画素がはっきりした四角として見えます。
• 5 つの色の形式。RGB、HEX、CMYK、HSL、HSV。形式名をクリックすると切り替わり、右クリックで直接選べます。
• 小さいことが特長。240×64 の窓で、作業の邪魔になりません。ドラッグで移動、Esc で終了します。
• ダークとライトのテーマ、または Windows の設定に従います。
• 常に最前面。スイッチで切り替えられます。
• 高 DPI の表示でもくっきり表示し、複数のモニタでも正しい画素を読みます。
• プライバシー。ネットワークに接続せず、アカウントも利用状況の送信もありません。保存するのは 3 つの設定だけで、端末の中のファイルに置きます。

ご注意

• アプリの表示は英語です。
• ルーペは自分自身を透かして見ます。背面を読むために、自分の窓を画面の取り込みから除外しています。そのため、スクリーンショット・画面の録画・画面共有には映りません。
• CMYK は ICC プロファイルを使わない素朴な換算です。HDR と広色域には対応していません。値は 8 bit の sRGB です。
• Windows 10 version 2004 以降（64 bit）。
• MIT License のオープンソースです: https://github.com/hideyukiMORI/nene-loupe
```

### 製品の特長

```text
レンズの真下の 7×7 画素を、補間なしで 8 倍に表示
中心の色をワンクリックでコピー
RGB・HEX・CMYK・HSL・HSV
ダークとライトのテーマ、または Windows に従う
常に最前面（切り替え可）
モニタごとの DPI に対応。複数モニタで動作
ネットワーク接続なし、アカウントなし、利用状況の送信なし
```

### 検索語

```text
カラーピッカー
スポイト
ルーペ
拡大鏡
画素
色コード
HEX
```

### スクリーンショットの説明

| 画像 | 説明 |
| --- | --- |
| `docs/images/store/store-1-loupe-dark.png` | `レンズの真下の画素を拡大し、中心の画素の色を表示します。` |
| `docs/images/store/store-2-loupe-light.png` | `ダークとライトのテーマ。色は HEX・RGB・CMYK・HSL・HSV で表示できます。` |
| `docs/images/store/store-3-settings.png` | `設定: テーマと常に最前面。` |

画像は 3 枚とも英語の表示で、言語ごとに作り分けていない。見出しは英語のままである。

## そのほかの入力

| 項目 | 入れる内容 | 根拠 |
| --- | --- | --- |
| カテゴリ | 「ユーティリティとツール」を提案する。hide が選ぶ | 色を読む道具であり、開発者専用ではない |
| 価格 | 無料 | hide の決定（無料配布） |
| プライバシーポリシーの URL | `https://github.com/hideyukiMORI/nene-loupe/blob/main/PRIVACY.md` | `PRIVACY.md`（Issue #32） |
| サポートの連絡先 | `https://github.com/hideyukiMORI/nene-loupe/issues` | `PRIVACY.md` の連絡先と同じ |
| Web サイト | `https://github.com/hideyukiMORI/nene-loupe` | |
| 著作権 | `© 2026 Hideyuki Mori` | 設定窓の表示と同じ |
| `runFullTrust` の説明文・審査員向けの注記 | `docs/DEVELOPMENT_WORKFLOW.md` 第 9 節 | hide が確認済み |

## 年齢区分（IARC）のアンケートに答えるための事実

アンケートの設問そのものは Partner Center の画面で出る。ここには、答えの根拠になる事実だけを置く。

| 問われそうなこと | 事実 | 根拠 |
| --- | --- | --- |
| 暴力・性的な内容・恐怖・賭博・薬物・粗野な言葉 | 無い。アプリ自身は内容を持たず、利用者の画面の画素を拡大して見せるだけ | `SPECIFICATION.md`、実装 |
| 利用者同士のやり取り（チャット・投稿・共有） | 無い | 通信をしない（下） |
| インターネットへの接続 | しない。Release の exe が読み込む DLL は `KERNEL32` / `USER32` / `GDI32` / `ADVAPI32` だけ | `PRIVACY.md`、ARC-002 の許可表 |
| 個人情報の収集・第三者への提供 | しない | `PRIVACY.md` |
| 位置情報 | 使わない | 実装に該当の API が無い |
| アプリ内の購入・デジタル商品の販売・広告 | 無い | 実装に該当の機能が無い |
| Web の内容を制限なく表示する機能（ブラウザ） | 無い | 実装に該当の機能が無い |

利用者の画面に映っているものを拡大するので、表示される内容は利用者の画面しだいである。アプリが内容を取り寄せたり保存したりはしない。

## 確かめていないこと

- **入力欄の名前・必須かどうか・字数や件数の上限。** 下調べで公式文書から確かめたのは、説明文が必須で 1 万字まで、
  少なくとも 1 言語ぶんの掲載ページが要ること、スクリーンショットの条件、年齢区分の回答が必須であることだけである。
  短い説明・製品の特長・検索語の上限は確かめていない。Partner Center が受け付けなければ、その場で削る。
- 年齢区分のアンケートの実際の設問。上の表は事実の一覧で、設問との対応は入力のときに hide が見る。
- この掲載文で審査に足りるか。
