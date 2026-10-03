# 開発ワークフロー — NeNe Loupe

> Status: normative（規範）/ 2026-09-06 初版

ワークフローは制約系の一部である。**追跡可能な手順の外で作られた技術的に正しいパッチは、未完成である。**

---

## 1. 作業の単位

production コード・ビルド・依存・方針・利用者向けドキュメントの変更は、すべて 1 つの焦点を持つ Issue から始める。Issue には次を書く。

- 問題と根拠
- 意図する結果
- 影響する規則 ID とモジュール
- 受け入れ条件
- 検証の計画
- **やらないこと**

読み取りだけで、リポジトリを変更しない調査は Issue 無しで行ってよい。

---

## 2. 標準の流れ

1. 焦点を持つ Issue を 1 つ作る／選ぶ
2. 正本のドキュメントと関係する ADR・waiver を読む
3. **編集の前に**正典の実装経路を特定する
4. 最新の `main` からブランチを切る
5. 設計そのものを変えるなら、**先に**方針文書か ADR を変える
6. 最小で完結する変更を実装する
7. テストを足す／更新する
8. 開発中は最も狭い検査を回す
9. PR を **draft** で出す
10. 影響した規則 ID を 1 つずつ自己レビューする
11. **Ready にする直前に `pwsh -NoProfile -File ./eng/check.ps1` を完全に回す**。CI の必須 check が同じコマンドで走る
12. 必須ゲートが通ってから squash merge する
13. 作業木を畳み、ローカル枝を消す（remote 枝はマージで自動的に消える）

---

## 3. ブランチとコミット

Issue・ブランチ・コミット・PR の形は [COMMIT_CONVENTIONS.md](COMMIT_CONVENTIONS.md)（GIT-001〜004）だけが定める。ここには複製しない。
1 つの PR にアーキテクチャ移行・無関係な整理・依存更新・機能追加を混ぜない。

---

## 4. 決定の記録（ADR）

### ADR が要るもの

- 憲章規則・依存方向の変更
- モジュール境界・状態の所有者・隔離区画の変更
- 公開 API の変更
- 保存形式の方針
- 合成／DI の方針
- 並行性・スレッドモデル
- 整形器・静的解析器・テスト枠組み・主要なビルドプラグインの変更
- 対応プラットフォームの変更
- 品質ゲートの重大度・閾値・除外の変更（QLT-010）
- 実行時依存の追加

### ADR が要らないもの

- 承認済みの境界の内側に閉じた実装の詳細
- 文書化された振る舞いを回復するだけの不具合修正
- 方針を変えないテストとドキュメント
- 契約に影響しない小さな依存更新
- **ゲートを強める変更**（弱める変更だけが ADR 事項）

---

## 5. 規則を変える手順

MUST / MUST NOT を変えるには次がそろう必要がある。

1. 矛盾と根拠を書いた Issue
2. 受理された ADR
3. 正本ドキュメントの更新
4. 強制（ゲート）の更新
5. コードの移行
6. **旧経路が拒否されることを示す回帰テスト**

これらは同時に入るのが原則である。段階移行が避けられないときは、期限付きの waiver を作って二重状態を明示する。

`V2` / `Legacy` / `New` / `Alternative` という名の並行実装を作らない。2 つの経路を保つ feature flag には期限と撤去の証明を持つ ADR が要る。

---

## 6. 依存の方針

新しい実行時依存を受け入れるのは、次がすべて満たされるときだけである。

- その機能が具体的にいま必要である
- 自前で書くほうが危険か、保守しにくい
- ライセンスと保守状況が受け入れられる
- 正しい境界の裏に隔離できる
- 版が固定でき再現可能である（QLT-011）
- ADR がある

**便利だから、は理由にならない。**

---

## 7. PR に書くこと

```text
Issue:
目的:
使った正典経路:
規則 ID:
振る舞い・スキーマの変更:
検証（実行したコマンドと結果）:
Waivers: none | WVR-NNNN
残るリスク:
```

レビューは次を拒否する。

- 既存の意味に対する第 2 の経路
- 所有者の書かれていない状態
- 弱められた／飛ばされたゲート
- 範囲の広い抑制
- アダプタ固有の業務ロジック
- 「将来使うかもしれない」公開 API
- 機能変更に紛れ込んだ無関係な整理

---

## 8. ドキュメントの方針

重要な決定を、チャット・Issue のコメント・コミットメッセージ・AI との会話ログだけに残さない。

- 長生きする不変条件 → 正本ドキュメント
- 判断とトレードオフ → ADR
- 一時的な逸脱 → waiver
- いまのタスク状態 → Issue / PR（`docs/todo/current.md` は要約）
- コードのコメント → その場の非自明な理由（方針そのものは書かない）

設定ファイルとドキュメントが食い違ったら、**ドキュメントが決定の記録**である。ただし両方がそろうまで merge は止める。

**事故が起きたら、直すだけでなく再発を機械で塞ぐ規則を足し、Issue 番号と実測を規則本文に残す。**

---

## 9. リリース

製品の版入力は`CMakeLists.txt`の`project(... VERSION ...)`だけとする。設定画面、Win32 manifest、
VERSIONINFO、配布物名はCMake configureで同じ値から導出する。Release x64の正規生成経路は次である。

```powershell
pwsh -NoProfile -File ./eng/package-release.ps1
```

このコマンドは固定toolchainと既存CMake targetをRelease構成でclean buildし、CTestと実build graphを
検査して、`out/release/`へportable ZIPと`SHA256SUMS`を作る。ZIP直下は`NeNeLoupe.exe`、
利用者向け`README.txt`、正本`LICENSE`だけとする。このRelease検査はPR最終HEADで行う全体ゲートの
代わりではない。

公開はsquash統合後のcleanな`main`から同じコマンドで作り直し、x64、埋込版、manifest、ICON、
静的runtime、ZIP内容、checksum、短い実Windows起動を確認してから行う。`v<PROJECT_VERSION>`のtagと
同名GitHub ReleaseへZIPと`SHA256SUMS`を添付する。署名、インストーラ、自動更新は別の焦点Issueとする。

### Microsoft Store への提出（ADR 0008）

Store へは、ZIP で配るのと同じ exe を MSIX に包んで出す。提出用の MSIX を作る経路は次の 1 つである。

```powershell
pwsh -NoProfile -File ./eng/package-release.ps1 -StoreMsix   # out/msix/NeNeLoupe-v<版>-windows-x64-store.msix
```

- **身元**は `eng/store-identity.json` の 1 か所にある。Partner Center の「製品 ID の表示」の値と一字一句同じにする。
  値が変わったら、このファイルだけを直す。
- **署名しない。** 認定のあと Store が署名する。証明書と鍵のファイルはリポジトリに置かない（CNF-009）。
- **版**は製品の版に `.0` を足した 4 つ組になる。Store は先頭が 0 の版を受け付けず、4 つ目は Store が使うので 0 のままにする。
- スクリプトは、stage・ZIP の中・MSIX の中の exe の SHA-256 が同じでなければ失敗する。この検査は全体ゲートの外にある。

提出の順番:

1. 版を上げる PR を統合する（版の入力は `CMakeLists.txt` だけ）。
2. clean な `main` で上のコマンドを実行する。
3. Windows App Certification Kit を通す。提出用の MSIX は未署名で入れられないので、同じ exe から作った検証用パッケージ（`eng/package-msix.ps1 -CertificateThumbprint …`）を入れて回す。
   手順と 2026-10-03 の結果（PASS 23・WARNING 1・FAIL 0）は [quality/gate-proofs.md](quality/gate-proofs.md) 第 16 節。
   高 DPI の警告は原因が分かっていない（Issue #56）。
4. Partner Center で申請を始め、パッケージ・掲載文・スクリーンショット・年齢区分の回答・プライバシーポリシーの URL を入れる。
   掲載文と年齢区分の根拠の正本は [release/store-listing.md](release/store-listing.md)、画像は `docs/images/store/` にある。
   申請オプションに、`runFullTrust` が必要な理由と認定の注意書きを入れる（文面の場所は下）。
5. 申請の直前に Store Policies を読み直す（下調べで読んだのは版 7.20・2026-10-22 発効）。
6. **Store の認定が出てから**、提出したのと同じ commit に `v<PROJECT_VERSION>` の tag を打ち、GitHub Release へ ZIP と `SHA256SUMS` を公開する。
   公開する ZIP は、提出用の MSIX と同じ実行で作ったもの（exe の SHA-256 が同じもの）を使い、作り直さない。
   そのため、提出のときに ZIP・`SHA256SUMS`・MSIX を `out/` の外へ控えておく。ビルドがバイト単位で再現するかは確かめていない。
   審査で製品の修正を求められた場合は、版を上げて 1 からやり直す。認定の前に ZIP を出さないのは、ZIP と Store の版をずらさないためである
   （hide の決定・2026-10-03）。

申請オプションに貼る文面（`runFullTrust` が必要な理由と、認定の注意書き）の正本は [release/store-listing.md](release/store-listing.md) の「申請オプションに貼る文面」にある。hide が 2026-10-03 に確認した文面で、
貼りやすいよう段落の途中に改行を入れていない。ここには複製しない。
