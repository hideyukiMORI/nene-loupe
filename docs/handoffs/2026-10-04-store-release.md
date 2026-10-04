# 引き継ぎ書 — NeNe Loupe / 2026-10-04（v1.0.0の公開後）時点

前の引き継ぎ書は [2026-10-04](2026-10-04.md)（Storeへ提出して認定中の時点）。
**再開にはこの文書だけを読めば足りる。** 前の文書から今も有効な部分（統合の手順、CIの版ずれ、MSIXの実測の手順、証拠の場所）は、ここへ写して直した。
製品の実装の要点は [2026-09-06](2026-09-06.md) にある。

## 最初に読む情報

- **NeNe Loupe 1.0.0は、Microsoft StoreとGitHub Releaseの両方で公開済みである（2026-10-04）。**
- **いま進行中の作業は無い。** 次の製品作業は、焦点Issueを作ってから始める。
- 読む順: [日報](../reports/2026-10-04-store-release.md) → [現況](../todo/current.md) → [掲載文と申請の記録](../release/store-listing.md) →
  [提出の手順](../DEVELOPMENT_WORKFLOW.md)（第9節）→ [ADR 0008](../adr/0008-add-msix-store-channel.md)。
  提出までの経緯は[前の日報](../reports/2026-10-04.md)。
- 依頼元は統合リナ（hub）。下調べの指示書は
  `\\wsl.localhost\Ubuntu-22.04\home\xi\docker\_work\handoff-loupe-store-msix-research-2026-10-03-work-order.md`。**hubへの報告は届いていない**（下）。

## いまの状態

2026-10-04の17時ごろに、GitHubと手元で確かめた値。

| 対象 | 状態 |
| --- | --- |
| main | この引き継ぎ書を入れるPRの親は`96c21dc`。開いているIssueとPRは、このPR（Issue #72）のほかに無い |
| CI | 緑。固定した`cl`は19.44.35229.0（`eng/tool-versions.json`） |
| 製品の版 | mainは1.0.0。tagは`v0.2.0`と`v1.0.0`。`v1.0.0`は`0c26ed4da205cc3152355bc0b6be6f10df5061c6` |
| GitHub Release | [v1.0.0](https://github.com/hideyukiMORI/nene-loupe/releases/tag/v1.0.0)が最新。ZIP（SHA-256 `EBE65AA1…150B36DD`）と`SHA256SUMS` |
| Microsoft Store | 公開中。<https://apps.microsoft.com/detail/9N6HZB1SXKBM>（Submission 1）。hideが貼ったPartner Centerの画面で読んだ |
| 身元 | `eng/store-identity.json`。`HideyukiMori.NeNeLoupe` / `CN=C37230AA-B52D-403B-9BFD-E7980F088422` / `Hideyuki Mori`、Store ID `9N6HZB1SXKBM` |
| 配っているexe | StoreのパッケージとZIPで同じ（SHA-256 `587268D1…D2612FDA`）。hideのPCに入ったStoreのパッケージで読んだ |
| 年齢区分 | IARCの確定の通知が2026-10-04に届いた。Global Rating IDはhideのメールにある（リポジトリには書かない） |
| hideのPC | Storeから入れた1.0.0（`SignatureKind: Store`）が入っていて、hideが使っている。検証用のパッケージと証明書はどのストアにも無い。設定ファイルのSHA-256は`D29B6BC6…2D73`のまま。パッケージ専用の場所に設定ファイルは無い |
| 元のリポジトリ `C:\Users\info\WORKS\NeNeLoupe` | 枝`main`、originと同期済み。ローカルに古い枝が3つ残っている（`docs/25-msix-store-research`、`feat/21-readme-screenshots`、`feat/3-first-loupe-slice`） |
| 作業木 | `D:\NeNeLoupe\wt-25-msix`（枝は統合済み）だけが残っている。このセッションの作業は元のリポジトリの枝で行った |

## 次に版を出すとき

更新の申請は一度も行っていない。分かっているのは次のことである。

1. 版を上げるPRを統合する（版の入力は`CMakeLists.txt`だけ）。
2. **年齢区分のアンケートの答えが変わるかを確かめる。** 通信・利用者同士のやり取り・購入や広告・Webの内容の表示のどれかを足したなら、
   アンケートを答え直す（`docs/release/store-listing.md`の「答え直す条件」）。
3. cleanな`main`で`pwsh -NoProfile -File ./eng/package-release.ps1 -StoreMsix`を実行し、MSIX・ZIP・`SHA256SUMS`を`out/`の外（`D:\NeNeLoupe\release-<版>\`）へ控える。
4. WACKを通し、hideがPartner Centerの「更新の開始」から提出する。手順は`DEVELOPMENT_WORKFLOW`第9節。
5. **認定が出てから**、提出したのと同じcommitにtagを打ち、控えたZIPをGitHub Releaseへ出す。ZIPを作り直さない。
   tagとReleaseの公開は外へ出る操作なので、**打つ前にhideへ確かめる**。PRの統合の許可には含まれない。
6. 公開のあと、Storeから入れたパッケージのexeのSHA-256がZIPの中と同じことを読む（`gate-proofs.md`第17節の方法）。

`docs/release/README.txt`は次の版のZIPから入る。後で変わること（公開の状態など）を書かない。

## 気を付けること

### 1. 統合の前に、headのSHAで最新の`check`の成功を読む

2026-10-03にPR #24を、CIが実行されていないheadで統合した（Issue #30）。穴は塞いだが、`mergeStateStatus`だけを根拠にしない。

1. Draftのままpushする。Draftのイベントのrunは`QLT-012`で**失敗するのが正しい**。Draftを開いただけ（`opened`）ではCIは起動しない。
2. Readyにする。
3. headのSHAの**最新の**check runが`completed`で`success`であることを読んでから統合する。Readyの直後は`check`がまだ登録されておらず、結果が空で返ることがある。空は成功ではない。

```powershell
$sha = gh pr view <N> --json headRefOid --jq .headRefOid
gh api repos/hideyukiMORI/nene-loupe/commits/$sha/check-runs --jq '.check_runs | sort_by(.started_at) | reverse | .[] | [.name, .status, .conclusion] | @tsv'
```

この手順をまとめたスクリプトが`D:\NeNeLoupe\ship-pr.sh`にある（Git対象外。無ければ上の手順を手で行う）。PRの統合はリナが行ってよい（hideの決定）。
hideの名前で外へ出る文書（README、プライバシーポリシー、Storeの掲載文）を変えるPRは、スクリプトを使わずにReadyで止め、hideが読んでから統合する（リナの解釈）。
squash統合でリモートの枝は自動で消える。

### 2. runnerのVisual Studioが上がると、CIは`QLT-011`で落ちる

再発する。失敗文の`actual:`でrunnerの版を読み、手元のBuild Toolsを同じ版へ更新し（hideの操作・管理者権限。Visual Studio Installerの「更新」）、
`eng/tool-versions.json`を上げる。**版の検査を外したり緩めたりして通さない。** 前例はIssue #27。

### 3. PR本文は雛形の欄の形で書く

CIは`目的:`・`使った正典経路:`・`規則 ID:`・`検証`・`Waivers:`・`残るリスク:`と`Closes #N`の文字列を探す（`eng/validate-git.ps1`）。
見出し（`## 目的`）で書くと`GIT-004`で落ちる。手元のゲートはPR本文を見ないので、CIでしか分からない。

### 4. hideの画面を見ずに、Partner Centerの操作を言い切らない

2026-10-03の案内は3か所で実際の画面と違った。分かっている事実は`docs/release/store-listing.md`にある。それ以外は、画面を見せてもらってから案内する。
貼る文面は、端末の表示からではなく、ファイルをエディタで開いてコピーしてもらう。

### 5. 実機の実測は、控えを取ってから。場面の条件は一字ずつ読む

- `%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`: 包んだアプリは、このファイルかフォルダがあるとその場に書く。実測の前に控えを取り、後でSHA-256と更新時刻を比べる。
- 「設定が無い」場面は、**フォルダごと**退避する。ファイルだけを退避すると別の場面になる（2026-10-04にリナが間違えた）。
- パッケージ専用の場所に設定ファイルを残さない。残すと、hideのStore版は以後そちらを使う。
- クリップボード: 値をクリックする操作を送ると書き換わる。
- 昇格（UAC）: hideがPCの前にいないと取り消しで戻る。2回戻ったら、それ以上出さずにhideへ確かめる。
- **hideはStore版のルーペを普段使っている**（同じ窓クラス`NeNeLoupe.Window`）。探針は、起動の前からある窓を除いて新しい窓だけを操作する。取り違えない。

## hubへの報告（届いていない）

下調べのセッションでも、提出のセッションでも、このセッションでも、`ListAgents`に統合リナの席が無かった。違う席には送っていない。次のセッションで席があれば、これを送る。

```text
NeNe Loupeリナからの報告（2026-10-04・公開後）

1. Store申請の下調べ（指示書 handoff-loupe-store-msix-research-2026-10-03-work-order.md）は完了し、その先の公開まで進んだ。
   NeNe Loupe 1.0.0 は 2026-10-03 に提出、2026-10-04 に Microsoft Store で公開された（差し戻しなし）。
   https://apps.microsoft.com/detail/9N6HZB1SXKBM
   同じ日に v1.0.0 の tag と GitHub Release も出した。Store と ZIP の exe は同じファイル。
2. hubの前提4点（Microsoftが署名する・個人登録は無料・SmartScreenの警告が消える・MSI/EXEは自前署名が要る）は公式文書で合っていた。
   そのままでは申請に進めなかった点: 版の先頭0は不可、プライバシーポリシーが必須、掲載用の画像（1366×768以上）が要る、設定の保存先が入れ方で変わる。
3. 読むもの: docs/reports/2026-10-04-store-release.md と docs/reports/2026-10-04.md（日報）、docs/handoffs/2026-10-04-store-release.md（引き継ぎ書）、
   docs/adr/0008-add-msix-store-channel.md、docs/reports/2026-10-03-store-msix-research.md（裏取りと実測）、
   docs/release/store-listing.md（掲載文と申請の記録）、docs/quality/gate-proofs.md 第17節（Storeが署名したパッケージの実測）。
4. 他のリポジトリにも効く点:
   - GitHubのrunnerのVisual Studioが17.14.41へ上がった。MSVCの版を完全一致で固定しているリポジトリは同じ理由でCIが落ちる。
   - 必須checkをjobのifでスキップさせると、スキップが通過として扱われ、CIが実行されていないPRを統合できる。
   - 包んだアプリの設定の保存先は、AppDataの下のフォルダが既にあるかで決まる（ファイルの有無ではない）。
   - WACKの高DPIの警告が1件あっても認定された。審査は提出の翌日には終わっていた。
   - IARCの年齢区分は、アンケートの答えが変わる更新のときに答え直しが要る。
   - NeNeClockへ横展開するときの差は、調査報告の「NeNeClockへ横展開するときに効きそうな差」にある。Clockには触れていない。
   - 雛形へ還流すべき点11個は、日報2つの末尾にある。雛形には書いていない。
5. 指示書の「main d33f9e4」は誤りで、当時のmainは5b6a362だった。
```

## 確認済みと未確認の境界

確認済み（Windows 11 Pro 10.0.26200）:

- Storeの認定と公開（hideが貼ったPartner Centerの画面で）。IARCの通知（hideが貼ったメールで）。
- GitHub Release v1.0.0のZIPが、控えとバイト単位で同じこと。
- Storeが署名したパッケージで（2026-10-04、144 DPI）: 素性、manifestの身元、exeのSHA-256、起動、窓の寸法、常に最前面、キャプチャ除外、採取、当たり判定、複写、終了、
  設定の保存先（4つの場面）、シェルが返すアイコン（16・32・48・256pxがロゴと一致）。
- 自己署名の検証用パッケージで（2026-10-03）: 上に加えて、アンインストール後の残留、WACK（PASS 23・WARNING 1・FAIL 0）。
- 全体ゲートは、統合したすべてのPRの最終の内容で終了0。

未確認:

- Storeのページに、掲載文と画像が入力したとおりに出ているか（リナは見ていない）。認定された時刻。
- Storeが署名したパッケージでの、アンインストール後の残留、版を上げたときの設定の引き継ぎ、WACK。
- スタートメニューとタスクバーの実際の画面。24pxのアイコンでシェルが使う候補。
- WACKの高DPIの警告の原因。
- `Failed`・`No position`・`No capture`・起動失敗のメッセージ・設定の読み書き失敗の英語の文言の見え方（失敗を実機で起こしていない）。
- コピーの通知の144 DPI・168 DPIでの見え方。Windows 10、100%表示。包んだ状態での設定窓・右クリックメニュー・テーマ切り替えの操作。
- 更新の申請の流れ。同じ版を再提出できるか。日本語のキーワードの「21単語まで」の数え方。規約7.20（2026-10-22発効）より前の版との差分。
- ビルドがバイト単位で再現するか。
- S1（本物の場所にファイルがある場面）を、今のmainの`eng/verify-window.py`で測ること（0.2.0の時点のもので測った）。

## Storeが署名したパッケージの実測をやり直す手順

昇格は要らない。hideのPCにStoreから入れたパッケージがあることが前提。探針は`D:\NeNeLoupe\probe\`にある（Git対象外。無ければ`gate-proofs.md`第17節から作り直す）。

```powershell
# 0. hide の設定ファイルの控えを取る（Copy-Item は更新時刻を保つ）
Copy-Item "$env:LOCALAPPDATA\NeNeLoupe\settings.v1.txt" D:\NeNeLoupe\probe\settings-backup-<日付>\

# 1. 素性と exe の SHA-256
Get-AppxPackage HideyukiMori.NeNeLoupe | Select-Object PackageFullName, SignatureKind, Status
Get-FileHash "$((Get-AppxPackage HideyukiMori.NeNeLoupe).InstallLocation)\NeNeLoupe.exe"

# 2. 起動と窓の検査（新しい窓だけを操作する。形式を切り替えるので設定ファイルが書き換わる）
python -B D:/NeNeLoupe/probe/store-probe.py --aumid 'HideyukiMori.NeNeLoupe_3sft4ch5kzywy!NeNeLoupe' --label <場面> --cycle-format

# 3. 場面の前後で、2 か所の設定ファイルを読む
pwsh -NoProfile -File D:/NeNeLoupe/probe/store-settings-state.ps1 -Label <場面>

# 4. シェルが返すアイコン（画面もポインタも操作しない）
python -B D:/NeNeLoupe/probe/shell-icon-probe.py 'HideyukiMori.NeNeLoupe_3sft4ch5kzywy!NeNeLoupe' src/app/msix D:/NeNeLoupe/probe/store-icons

# 5. 片付ける。パッケージ専用の場所の設定ファイルを消し、控えから戻して SHA-256 と更新時刻を比べる
```

自己署名の検証用パッケージで測り直す手順（証明書の導入に管理者権限が要る）は、[前の引き継ぎ書](2026-10-04.md)の「MSIXの実測をやり直す手順」にある。今も有効である。
WACKの手順と結果は`docs/quality/gate-proofs.md`第16節にある。

## 証拠の場所

リポジトリの外の置き場は`D:\NeNeLoupe\`（Git対象外。ここだけを正本にしない）。

| 場所 | 中身 |
| --- | --- |
| `D:\NeNeLoupe\release-1.0.0\` | 提出したMSIX、公開したZIPと`SHA256SUMS`。ZIPと`SHA256SUMS`はGitHub Releaseに同じものがある。MSIXの控えはここだけ |
| `D:\NeNeLoupe\release-1.0.0-redownload\` | 公開のあとにGitHubから取り直したZIPと`SHA256SUMS`。控えと同じ中身なので、消してよい |
| `D:\NeNeLoupe\release-1.0.0-notes.md` | GitHub Releaseの説明文の元 |
| `D:\NeNeLoupe\probe\` | 探針（`store-probe.py`、`store-settings-state.ps1`、`probe-msix.py`、`shell-icon-probe.py`、`toast-probe.py`）、結果（`result-Store-*.json`、`store-icons\`）、hideの設定の控え（`settings-backup-2026-10-04-store\`ほか） |
| `D:\NeNeLoupe\wack\` | WACKの報告とログ |
| `D:\NeNeLoupe\check-issue*.log` / `package-release-*.log` / `capture-*.log` | 全体ゲート・Release・撮影のログ |
| `D:\NeNeLoupe\ship-pr.sh` | 統合の手順をまとめたスクリプト |
| `D:\NeNeLoupe\wt-25-msix\` | 下調べの作業木。`out/`に0.2.0のMSIXと配置。消してよいかはhideが決める |
| `D:\NeNeLoupe\`のそのほか（`edit-issue*.py`、`pr*-body.md`、`issue*.md`など） | 作業用のファイル。内容はPRとIssueに入っているので、消してよい |

`result-Store-S2-no-real-settings.json`は、名前と違って「フォルダはあるがファイルが無い」場面（S2b）の結果である。「何も無い」場面は`result-Store-S2-nothing-exists.json`。

## hideの手番

- hubへ報告を渡す（上の文面）。
- GitHub Releaseの説明文を読む（リナが、hideが読む前に公開した）。直したい所があればリナへ。
- Storeのページで、掲載文と画像が入力したとおりに出ているかを見る。
- `D:\NeNeLoupe\wt-25-msix`、`D:\NeNeLoupe\release-1.0.0-redownload\`、`D:\NeNeLoupe\`の作業用のファイル、ローカルの古い枝3つを消してよいかを決める。

## Claude Code

このセッションのモデルはClaude Opus 5.5（`claude-opus-5-5`）。Windows側のリポジトリ`C:\Users\info\WORKS\NeNeLoupe`の枝で作業し、ログと控えは`D:\NeNeLoupe\`に置いた。
背景の席は使っていない。

## 規則・保留

Issue #66・#68・#70・#72。新設・変更した規則なし。QLT-012はplannedのまま。Waivers: none。
ゲートの緩和・新しい除外なし。設定スキーマ変更なし。製品コードの変更なし。NeNeClockには触れていない。
