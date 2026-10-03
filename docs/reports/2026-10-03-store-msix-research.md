# 調査報告 — MSIX 化と Microsoft Store 申請の前提（2026-10-03）/ NeNe Loupe

記録者: Loupeリナ。[Issue #25](https://github.com/hideyukiMORI/nene-loupe/issues/25) の下調べ。
申請・Partner Center の操作・証明書の購入は行っていない。判断は [ADR 0008](../adr/0008-add-msix-store-channel.md)（提案）。

## 結論

- **Store に MSIX で出すと Microsoft が署名し直す。自前の証明書は要らない。** 公式文書に明記がある。
- **個人の開発者登録は無料。** ただし入口が 1 つに限られる（`storedeveloper.microsoft.com`）。
- **Store から入れたアプリに SmartScreen の警告は出ない。** これも明記がある。
- 実機では、いまの exe を**製品コードを変えずに** MSIX に包めて、入れて、動いた。レンズの採取もキャプチャ除外も通った。
- そのまま申請に進めない点が 4 つ見つかった。
  1. **版が `0.2.0` のままでは受け付けられない。** Store は版の先頭が 0 のパッケージを認めない。
  2. **プライバシーポリシーが必須。** 何も集めないアプリでも、Win32 を包んだ製品は常に必須と書いてある。
  3. **掲載用のスクリーンショットを別に作る必要がある。** 1366×768 以上が要るが、窓は 240×64 DIP で、しかも普通には撮れない。
  4. **設定の保存先が、入れ方によって変わる。** README の記述は Store 版の利用者には当てはまらない。

## A. 一次資料での裏取り

確認日はすべて **2026-10-03**。出典は Microsoft Learn の文書本体と、winget については Microsoft の公式リポジトリ。
ブログと Q&A は根拠にしていない。「更新」は各ページのメタデータ `updated_at` の日付。

| # | 項目 | 結論 | 出典（更新日） |
| --- | --- | --- | --- |
| 1 | 個人アカウントの登録料 | **無料。** 新しい登録の流れでは 19 ドルの登録料が免除される。入口は `https://storedeveloper.microsoft.com` だけで、Partner Center・Visual Studio などから入ると旧い流れになる | [whats-new-individual-developer](https://learn.microsoft.com/en-us/windows/apps/publish/whats-new-individual-developer)（2026-04-18）、[open-a-developer-account](https://learn.microsoft.com/en-us/windows/apps/publish/partner-center/open-a-developer-account)（2026-07-17） |
| 1 | 本人確認の手順と日数 | 政府発行の身分証と自撮りをスマホで撮る。個人の Microsoft アカウントが必須（仕事用の Entra ID は不可）。**個人の所要日数は文書に書かれていない。**「確認が済むとすぐ Partner Center へ進む」とだけある。日数の記載があるのは会社アカウントだけ（手動審査で 2〜5 営業日） | 同上 |
| 2 | MSIX は Store が署名するか | **する。** 認定に通ったあとの公開の工程で、Store が Microsoft の証明書で署名し直し、元の署名を置き換える。CA の証明書・`.pfx`・USB トークンは不要。自前の署名が要るのは Store の外で配る場合 | [get-started](https://learn.microsoft.com/en-us/windows/apps/publish/get-started) FAQ 11（2026-08-24）、[msix/app-package-requirements](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/app-package-requirements)（2026-08-24）、[msix/app-certification-process](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/app-certification-process)（2026-08-24） |
| 3 | MSI/EXE での提出 | **自前の署名が必須。** インストーラとその中のすべての PE ファイルに、Microsoft Trusted Root Program の CA に連なる証明書の署名が要る。Store は署名し直さない。さらに自前でのホスティング（版ごとの HTTPS URL）とサイレントインストールが要る | [msi/app-package-requirements](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msi/app-package-requirements)（2026-08-24） |
| 4 | SmartScreen | **Store から入れたアプリには出ない。**「Store-distributed apps are signed by a Microsoft certificate and are never subject to SmartScreen download warnings.」未署名のファイルは版ごとに評判がゼロから始まる。EV 証明書でも初回は警告が出る | [smartscreen-reputation](https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/smartscreen-reputation)（2026-08-17） |
| 5 | 掲載文 | 説明文が必須（1 万字まで）。少なくとも 1 言語ぶんの掲載ページを埋める | [msix/add-and-edit-store-listing-info](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/add-and-edit-store-listing-info)（2026-08-24） |
| 5 | スクリーンショット | 1 枚が必須（4 枚以上を推奨、デスクトップは 10 枚まで）。PNG、50 MB 以下、**デスクトップは 1366×768 以上** | [msix/screenshots-and-images](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/screenshots-and-images)（2026-08-24） |
| 5 | 年齢区分 | IARC のアンケートへの回答が必須（全問） | [msix/age-ratings](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/age-ratings)（2026-08-23）、Store Policies 11.11 |
| 5 | プライバシーポリシー | 入力欄の説明は「個人情報を扱うなら必須、扱わないなら任意」。しかし規約 10.5.1 は **「Product types that inherently have access to Personal Information must always have privacy policies. These include, but are not limited to, Desktop Bridge and Win32 products.」** と書く。Loupe は Win32 を包んだ製品なので、必須と読む。URL でも、本文を直接入力するのでもよい | [store-policies](https://learn.microsoft.com/en-us/windows/apps/publish/store-policies) 10.5.1（版 7.20、2026-09-15 公開、2026-10-22 発効）、[msix/support-info](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/support-info)（2026-04-21） |
| 6 | 審査: 画面の画素を読むこと | **規約に記載が無い。** Store Policies 7.20 の全文に、画面の取り込み・画素・クリップボードを指す条項は見つからなかった。`graphicsCapture` 系の capability は `Windows.Graphics.Capture` API のためのもので、GDI についての記述は無い。禁止とも許可とも書かれていないので、通るかどうかは文書からは断定できない | store-policies（同上）、[app-capability-declarations](https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/app-capability-declarations)（2026-09-08） |
| 6 | 審査: `runFullTrust` | 制限付き capability。包んだ Win32 アプリは宣言が必要。アップロード時に検出され、**提出オプションのページで用途の説明を書く**。審査員がそれを読んで判断し、審査が長引くことがある。更新のたびにやり直す必要は原則ない。一覧の `runFullTrust` の行に「ほぼ承認されない」という注意書きは無い（他の capability には付いているものがある） | app-capability-declarations（同上）、[msix/manage-submission-options](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/manage-submission-options)（2026-08-17） |
| 6 | 審査: その他 | 審査は 3 営業日まで。事前に Windows App Certification Kit を通すことを求めている。昇格（管理者権限）が要るアプリは受け付けない（Loupe は不要）。宣言した capability は機能と正当に関係していること（10.6） | msix/app-certification-process、[desktop-to-uwp-prepare](https://learn.microsoft.com/en-us/windows/msix/desktop/desktop-to-uwp-prepare)（2025-06-10）、store-policies 10.6 |
| 7 | 発行者の表示名の変更 | **公式文書どうしが食い違う。** Windows の公開手順の目次が指すページは「登録後は発行者の表示名・アカウント種別・国を変えられない。変えるならサポートへ連絡し、新しいアカウントが要ることもある」と書く。別のページ（Marketplace 向け）は「Update のリンクから発行者の表示名を変えられる」と書く。**変えられない前提で最初の名前を決めるのが安全** | [partner-center-account-setup](https://learn.microsoft.com/en-us/partner-center/account-settings/partner-center-account-setup) FAQ（2026-09-01）、[manage-account](https://learn.microsoft.com/en-us/partner-center/account-settings/manage-account)（2025-09-25） |
| 7 | 個人名で出すこと | 個人アカウントは「自分の名前で公開する」ためのもの。プロフィールは身分証から自動入力され、必要なら直せる。表示名が身分証の氏名と一致しなければならないか、は書かれていない。**事業体と受け取られる発行者名には会社アカウントが要る**（10.14）。個人から会社への切り替えはできない | open-a-developer-account、store-policies 10.14 |
| 8 | winget: 未署名の MSIX | **受け付けない。**「MSIX installers must be signed to be included in the Microsoft community package repository.」 | [winget-pkgs installer.md 1.12.0](https://github.com/microsoft/winget-pkgs/blob/master/doc/manifest/schema/1.12.0/installer.md) |
| 8 | winget: 未署名の ZIP | 形式としては対応している（`zip` は 1.5 から、`portable` は 1.3 から）。exe や ZIP に署名を求める文は見つからなかった。ただし検証で複数のウイルス対策エンジンと Defender の走査、ダウンロード URL の SmartScreen 評判の確認があり、`Portable-Archive` / `Zip-Binary` という審査ラベルは「止める扱い」と書かれている（付く条件は書かれていない）。**出せる可能性はあるが、通るとは文書から言えない** | 同上、[winget-pkgs doc](https://github.com/microsoft/winget-pkgs/tree/master/doc)（Policies.md / Validation.md / Moderation.md）、[package/repository](https://learn.microsoft.com/en-us/windows/package-manager/package/repository)（2026-07-14） |
| 8 | winget と Store の関係 | winget は `msstore` を既定のソースとして持つ。Store に出たアプリは `winget install <ID> -s msstore` で入る | [winget/source](https://learn.microsoft.com/en-us/windows/package-manager/winget/source)、[winget/install](https://learn.microsoft.com/en-us/windows/package-manager/winget/install) |

### 依頼に無かったが、申請を左右する発見

| 項目 | 内容 | 出典 |
| --- | --- | --- |
| 版の先頭は 0 にできない | 「the last (fourth) section of the version number is reserved for Store use and must be left as 0 … (except for the first section, which cannot be 0)」。**いまの `0.2.0` は Store に出せない。** 手元では `0.2.0.0` でも入る（実測）ので、気付かずに進むと提出で止まる | [msix/app-package-requirements](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/app-package-requirements)「Package version numbering」 |
| 個人アカウントの対象 | 個人アカウントは「事業・商売・職業と関係しない」配布のためのもの、と書いてある。副業の届け出の確認と同じ論点 | open-a-developer-account |
| 身元の値は Partner Center が決める | マニフェストの `Identity/Name`・`Identity/Publisher`・`PublisherDisplayName` は、Partner Center が示す値と一字一句同じにする | [view-app-identity-details](https://learn.microsoft.com/en-us/windows/apps/publish/view-app-identity-details) |

### 文書から分からなかったこと

- 個人の本人確認にかかる日数。
- 画面の画素を読むアプリが審査でどう扱われるか（規約に条項が無い）。
- `runFullTrust` の説明に何を書けば足りるか（判断基準は公開されていない）。
- 発行者の表示名を実際に変えられるか（文書が食い違う）。
- 規約 7.20 は 2026-10-22 発効で、確認日にはまだ発効前だった。それ以前の版との差分は読んでいない。

## B. 手元で MSIX を作って入れた実測

### 環境と材料

- Windows 11 Pro 10.0.26200、Windows SDK 10.0.26100.0 の `makeappx.exe` / `signtool.exe`。表示は 150%（144 DPI）。
- exe は main `5b6a362` を `eng/package-release.ps1` で作った Release（420,352 バイト、
  SHA-256 `47FE059F5714BE7B60BE7B7841DD97159D7F76328BF59264684F56462BF23CA1`）。製品コードは変えていない。
- MSIX は `eng/package-msix.ps1`（この PR で追加・ゲートの外）で作った。中身は exe、`AppxManifest.xml`、ロゴ 3 枚の計 5 ファイル。
- 署名はテスト用の自己署名（コード署名専用・14 日で失効・秘密鍵は持ち出し不可で証明書ストアの中だけ）。
  **証明書も鍵もファイルとしてリポジトリに置いていない。** 実測のあとストアからも消した。

### マニフェストに何が要るか

| 試したこと | 結果 |
| --- | --- |
| `runFullTrust` を外す | `makeappx` が拒否（`80080204`: requires "runFullTrust" capability） |
| `EntryPoint="Windows.FullTrustApplication"` を外す | `makeappx` が拒否（Executable があるなら EntryPoint が必須） |
| ロゴのファイルを入れない | `makeappx` が拒否。`StoreLogo` / `Square150x150Logo` / `Square44x44Logo` の 3 枚が必要 |
| 版を `0.2.0.0` にする | 手元では作れて、入る（Store は拒否する。上表） |
| 版を `1.0.0.1` にする | 手元では作れて、入る（Store は 4 番目を 0 にすることを求める） |
| `resources.pri` を入れない | 無しで作れて、入る |

### 入れる

| 試したこと | 結果 |
| --- | --- |
| 署名しただけ（証明書をどこにも信頼させない） | 拒否 `0x800B0109` |
| 証明書を「現在のユーザー → 信頼されたユーザー」に入れる | 拒否 `0x800B0109` |
| 証明書を「ローカル コンピューター → 信頼されたユーザー」に入れる（管理者権限が要る） | **入った。** `SignatureKind: Developer` |

自己署名で配る経路は、利用者に管理者権限で証明書を入れさせることになる。配布の手段にはならない。

### 動かす

AUMID（`shell:AppsFolder\…!NeNeLoupe`）から起動し、既存の `eng/verify-window.py` の検査関数をそのまま当てた。

| 観点 | 結果 |
| --- | --- |
| 起動 | 窓が出た。プロセスは `C:\Program Files\WindowsApps\NeNeLoupe.LocalTest_0.2.0.0_x64__…\NeNeLoupe.exe`、`GetPackageFullName` は成功（パッケージの身元を持って動いている） |
| 窓の大きさ | 240×64 DIP（144 DPI で 360×96 画素） |
| レンズの採取 | 背面に置いた 4 色（`#000000` / `#FFFFFF` / `#FF8000` / `#00010F`）を正しく読んだ。1 画素ずらすと隣の画素を読んだ |
| キャプチャ除外（ADR 0005） | `GetWindowDisplayAffinity` = `0x11`（`WDA_EXCLUDEFROMCAPTURE`）。包んでも効いている |
| クリップボード | 値のクリックで `#FF8000`、形式を切り替えて `0, 50, 100, 0` を複写できた |
| 当たり判定・終了 | 4 か所の当たり判定が一致。`WM_CLOSE` から 5 秒以内に終了 |

### 設定の保存先

製品は環境変数 `LOCALAPPDATA` の下の `NeNeLoupe\settings.v1.txt` を読み書きする。包んだあとの行き先は、**始めた時点で何があるかで変わった。**

| 場面 | 読んだ場所 | 書いた場所 |
| --- | --- | --- |
| S1: ZIP 版の設定がすでにある（`%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`） | 本物の場所（ZIP 版の設定を引き継いだ） | **本物の場所**（ZIP 版の設定ファイルを書き換えた） |
| S2: 何も無い（Store から初めて入れる人） | — | **パッケージ専用の場所** `%LOCALAPPDATA%\Packages\<PackageFamilyName>\LocalCache\Local\NeNeLoupe\settings.v1.txt`。本物の場所には何も作られない |
| S3: 両方にある | パッケージ専用の場所 | パッケージ専用の場所（本物の場所は変わらない） |

これは公式の説明どおりの動きである（Windows 10 1903 以降: 新しく作るファイルは専用の場所へ、既存のファイルの変更はその場で。
開くときは専用の場所を先に見る。[desktop-to-uwp-behind-the-scenes](https://learn.microsoft.com/en-us/windows/msix/desktop/desktop-to-uwp-behind-the-scenes)）。

- 製品コードを変えなくても、設定は読めて、書けて、再起動後も残る。**動かすために直す箇所は無い。**
- README と `docs/release/README.txt` の「設定は `%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`」は、Store から初めて入れた人には当てはまらない。
- ZIP 版と Store 版を同じ PC で併用すると、S1 の人は設定を共有し、S3 の人は別々になる。

### アンインストール後の残留

| 対象 | 結果 |
| --- | --- |
| `C:\Program Files\WindowsApps\…` の本体 | 消えた |
| `%LOCALAPPDATA%\Packages\<PackageFamilyName>\`（S2・S3 の設定を含む） | 消えた |
| `%LOCALAPPDATA%\NeNeLoupe\`（ZIP 版の設定。S1 で書き換えた内容を含む） | **残った** |

Store から入れただけの人には何も残らない。ZIP 版を使っていた人の設定フォルダは残る（ZIP 版のものなので正しい）。

### 実測の副作用と後片付け

- S1 で hide の本物の設定（`format=rgb`）が `format=hex` に書き換わった。実測の前に取った控えから戻し、
  SHA-256 が元と同じであることを確かめた。S2 では本物のフォルダを一時的に別名にし、終わってから戻した。
- 実測のあいだ、hide が使っていた別のルーペ（`build\NeNeLoupe.exe`）が動いていた。それには触れていない。
- テスト用のパッケージと証明書（3 つのストアすべて）は消した。

### 測っていないこと

- **Store が署名したパッケージの挙動。** 提出しないと手に入らない。今回測ったのは自己署名のもの。
- Windows App Certification Kit。管理者権限でアプリを何度も起動する検査なので、今回は回していない。申請の前に必ず回す。
- Windows 10、100% 表示、複数モニタ。今回は Windows 11・144 DPI・1 枚目のモニタだけ。
- 設定窓・右クリックメニュー・テーマ切り替えを、包んだ状態で操作すること。
- 版を上げたときに設定が引き継がれること（文書には「更新では保たれ、削除で消える」とある）。
- スタートメニューとタスクバーでのアイコンの見え方。今回のロゴは ICO を 3 寸法へ縮めただけで、
  倍率別・`targetsize` 別の画像と `resources.pri` は作っていない。

## 申請までに要ること（hide が進めると決めた場合）

| 要ること | 誰が | 備考 |
| --- | --- | --- |
| 副業・個人活動の届け出の確認 | hide | 個人アカウントは「事業・職業と関係しない」配布が対象 |
| 開発者アカウントの登録と本人確認 | hide | 入口は `storedeveloper.microsoft.com`。発行者の表示名は変えられない前提で決める |
| アプリ名の予約 | hide | 予約すると `Identity/Name` と `Identity/Publisher` が決まる |
| 版を 1.0.0 以上にする | 別 Issue | 版の入力は `CMakeLists.txt` の 1 か所（DEVELOPMENT_WORKFLOW 第 9 節）。MSIX だけ別の版にすると第 2 の経路になる |
| プライバシーポリシーを書く | 別 Issue | 「画面の画素を読むが、保存も送信もしない。通信しない」を書く。リポジトリに置けば URL になる |
| 掲載用のスクリーンショット | 別 Issue | 1366×768 以上。窓は小さく、しかも除外で撮れないので、ADR 0007 の撮影経路の絵を大きな背景に置く必要がある |
| `runFullTrust` の説明文と審査員向けの注記 | 別 Issue | 「Win32 アプリを包んだため」「窓が画面の取り込みに映らないのは仕様」を書く |
| ロゴ一式と `resources.pri` | 別 Issue | 倍率別・`targetsize` 別。300×300 の掲載用アイコンも推奨 |
| Windows App Certification Kit | 別 Issue | 申請の前に通す |
| README の設定の場所の記述 | 別 Issue | Store 版では場所が違うことを書く |

## NeNeClock へ横展開するときに効きそうな差

Clock には触れていない。Loupe の結果から言えることだけを書く。

- **署名の前提が変わる。** Clock の ADR 0013 は「MSIX / Store は署名が前提」で却下したと聞いている。Store に出すなら自前の署名は要らない（表の 2）。
- **版。** Clock は v0.2.6 なので、同じく先頭の 0 で止まる。
- **設定の保存先。** `AppData` の下に新しく作るファイルはパッケージ専用の場所へ行く。`AppData` の外（たとえばユーザーフォルダ直下）に書いているなら、
  行き先は変わらず、アンインストールしても残る。Clock がどこに書いているかを先に確かめること。
- **インストール先には書けない。** `C:\Program Files\WindowsApps\` は読み取り専用。jpackage の app-image が実行時に自分のフォルダへ
  ログやキャッシュを書く設定になっていると、包んだあとで失敗する。
- **jpackage は MSIX を作れない。** app-image（フォルダ）を作り、それを `makeappx` で包む。ファイルが多いだけで手順は同じ。
  起動用の exe は Win32 なので `runFullTrust` が要る点も同じ。
- **プライバシーポリシーは Clock にも必須**（Win32 を包んだ製品は常に必須）。

## 厳格規約の雛形へ還流すべき点

- **版は 1.0.0 から始めるか、Store に出す可能性があるなら先頭を 0 にしない。** 公開したあとで気付くと、版の付け直しになる。
- **配布経路の ADR は、署名や費用の前提を一次資料で確かめてから却下理由に書く。** 今回は「Store は署名が前提」という理解が、Store 配布については当てはまらなかった。
- **設定の保存先を文書に書くときは、包んだ場合に場所が変わることを前提にする。** 絶対の場所を README に言い切らない。
- **Win32 のアプリを Store に出すなら、何も集めなくてもプライバシーポリシーを用意する。** 雛形に置き場を作っておくとよい。
- **検証用の証明書はファイルにしない。** 証明書ストアに持ち出し不可の鍵で作り、拇印で指す。そうすれば「リポジトリに入れない」を仕組みで守れる。
