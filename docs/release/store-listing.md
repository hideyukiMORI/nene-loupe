# Microsoft Store の掲載文と年齢区分の根拠 — NeNe Loupe

> Issue #51 / ADR 0008。Partner Center への入力は hide が行う。ここは入力する文面の正本であり、
> 入力欄の上限と、実際に入力して分かったことは「2026-10-03 の入力で分かったこと」にある。

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
| `runFullTrust` が必要な理由・認定の注意書き | 下の「申請オプションに貼る文面」 | hide が確認済み |
| 公開の保留オプション | 「認定されたらすぐに公開する」 | tag と ZIP は認定の後に出す（DEVELOPMENT_WORKFLOW 第 9 節） |
| デバイス ファミリ | 「Windows 10/11 Desktop」だけ | 包んだ Win32 のアプリ |

## 申請オプションに貼る文面

どの段落も 1 行で、途中に改行を入れていない。端末の表示からではなく、このファイルをエディタで開いてコピーすること
（端末からコピーすると、折り返しの位置に改行や空白が入る）。

### 認定の注意書き（Notes for certification）

入力欄は「申請オプション」のページには無い。そのページの「認定の注意書き」の説明文にある「追加のテスト情報」のリンクを押し、開いた別のページで入れる（2026-10-03 に hide の画面で確かめた）。先に申請オプションを保存してから移ること。

```text
The application window is intentionally excluded from screen capture (SetWindowDisplayAffinity with WDA_EXCLUDEFROMCAPTURE) so that the loupe can see what is behind it. Because of this, the window does not appear in screenshots or screen recordings. This is by design, not a rendering failure.

To test: launch the app and drag it over any content. The left pane shows the 7x7 pixels behind the lens magnified 8x; the value on the right is the colour of the centre pixel. Click the value to copy it. Click the format label to cycle RGB / HEX / CMYK / HSL / HSV. The gear opens the settings. Press Esc to close. No account or sign-in is needed.
```

### `runFullTrust` が必要な理由

入力欄の上限は 500 文字である（2026-10-03 に、540 文字の文面が 500 文字で切れた）。下の文面は 468 文字。

```text
NeNe Loupe is an existing Win32 desktop app (C++, no UWP components) packaged with MSIX. runFullTrust is required because the packaged executable is a classic Win32 process (EntryPoint "Windows.FullTrustApplication"). It reads the screen pixels behind its own window through GDI to show a magnified view and the colour value, copies text to the clipboard on click, and saves three settings to a local file. No elevation, no network connections, no services or drivers.
```

確かめていないこと: 審査員がリモート接続や仮想マシンの画面越しに試した場合に、窓が見えるかどうか。
見えない場合の逃げ道は、除外を外す診断用の起動引数 `--allow-screen-capture`（ADR 0007）だが、包んだアプリに引数を渡す手順は確かめていない。

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

**答え直す条件:** 更新で上の表の事実が変わり、アンケートの答えが変わるときは、アンケートを答え直して区分を取り直す（IARC の通知の条項。下の「2026-10-03 の入力で分かったこと」）。
いまの区分は「設問はすべて『いいえ』」に基づくので、通信・利用者同士のやり取り・購入や広告・Web の内容の表示のどれかを足す変更が、その合図である。

## 2026-10-03 の入力で分かったこと

hide が Partner Center で申請（Submission 1）を入力した。画面の文言は hide が貼ったものを読んだ。

| 項目 | 結果 |
| --- | --- |
| パッケージの検証 | `NeNeLoupe-v1.0.0-windows-x64-store.msix` は `Validated`。`v1.0.0.0`・X64・`Windows.Desktop min version 10.0.19041.0`。身元・発行者・版のエラーは出なかった |
| パッケージの警告 | `The following restricted capabilities require approval before you can use them in your app: runFullTrust.`（想定どおり。申請オプションで理由を書く） |
| 年齢区分 | アプリの種類は「その他のすべてのアプリの種類」、設問はすべて「いいえ」。結果は IARC 3+ / Microsoft 3+ / ESRB E / PEGI 3 / USK Everyone / DJCTQ L / CCC TE / PCBP 0（IARC バージョン 10.3） |
| 年齢区分の確定の通知 | 2026-10-04 に IARC から「Live Rating Notice」のメールが hide に届いた（Rating Date は 2026-10-03、Storefront は Microsoft）。区分が有効になったという通知で、Store の審査の結果ではない。メールに区分の値は書かれていない。Global Rating ID は hide のメールにあり、ここには写さない（IARC と契約している別のストアへ出すときに入れると、同じ区分を使える） |
| プライバシーポリシーの設問 | 「個人情報へのアクセス、収集、または送信を行いますか」には「はい」と答え、URL を入れる（画面の画素を読むことと、規約 10.5.1 のため） |
| 製品の機能 | 最大 20 件 |
| キーワード | 最大 7 個、各 40 文字以内、全体で 21 単語まで |
| 短い説明 | 推奨 270 文字以下 |
| `runFullTrust` が必要な理由 | 上限 500 文字 |
| スクリーンショット | 1 つ以上が必須。4 つ以上を推奨。1366×768 以上を推奨。PNG・50 MB 未満・最大 30 ファイル |
| アプリ タイル アイコン | 300×300 の PNG（`docs/images/store/store-logo-300.png`） |

## 審査の結果（2026-10-04）

Submission 1 は認定され、Store で公開された。2026-10-04 に hide が貼った Partner Center の画面を読んだ（リナは画面を直接見ていない）。

| 項目 | 結果 |
| --- | --- |
| 状態 | 「Microsoft Store で取り扱い中」。「Microsoft Store のプレゼンス (Submission 1: 最終変更日 2026/10/03)」 |
| 差し戻し・追加の質問 | 無かった。この掲載文、`runFullTrust` の理由、認定の注意書きで審査を通った。WACK の高 DPI の警告（WARNING 1）があっても認定された |
| Store の URL | <https://apps.microsoft.com/detail/9N6HZB1SXKBM>（ディープ リンクは `ms-windows-store://pdp/?productid=9N6HZB1SXKBM`） |
| 身元の値 | 画面の Package/Identity/Name・Publisher・PublisherDisplayName・Package Family Name は `eng/store-identity.json` と README の記述と同じだった |
| GitHub Release | 同じ日に `v1.0.0` の tag を main `0c26ed4` に打ち、提出のときに控えた ZIP と `SHA256SUMS` を公開した（Issue #68） |

認定された時刻は分かっていない（提出は 2026-10-03、hide が公開を確認したのは 2026-10-04）。

## 確かめていないこと

- 日本語のキーワードで「全体で 21 単語まで」がどう数えられるか。
- Store のページに、掲載文と画像が入力したとおりに出ているか。
- Store が署名したパッケージの挙動（設定の保存先、アイコンの見え方、起動）。
