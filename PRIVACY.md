# Privacy Policy — NeNe Loupe

Last updated: 2026-10-03 · Publisher: Hideyuki Mori

**NeNe Loupe does not collect, store or transmit any personal information. It never connects to
a network.**

日本語は[このページの後半](#プライバシーポリシー日本語)にあります。

## What the application does with data

| Data | What happens | Where it goes |
| --- | --- | --- |
| Screen pixels | The application reads the 7×7 pixels directly behind its lens to draw the magnified view and show the centre colour. | Kept in memory only while displayed. Never written to disk. Never sent anywhere. |
| Clipboard | When you click the colour value, the application places that text (for example `#33E699`) on the clipboard. It does not read the clipboard. | Your clipboard, on your device. |
| Settings | Three choices are saved: theme, colour format and always-on-top. | One small text file on your device (see below). |
| Windows theme | When the theme is set to follow the system, the application reads the Windows light/dark setting. It does not change it. | Not stored. |

The application has no accounts, no telemetry, no analytics, no advertising, no crash reporting,
no automatic update check of its own, and no third-party services.

## The settings file

The settings file contains only a schema version and the three choices above. It contains
nothing about you, your screen or the colours you picked.

- Portable ZIP version: `%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`. It stays on your device
  until you delete it.
- Microsoft Store version (when available): if that file already exists, the application uses it. Otherwise
  Windows keeps the file inside the application's own package storage, and removes it when you
  uninstall the application.

## Network

The application makes no network connections. It is built without any networking library: the
released executable imports only four Windows system libraries (`KERNEL32`, `USER32`, `GDI32`
and `ADVAPI32`), none of which provides networking, and the build rejects any library outside
its allow-list.

If you install the application from the Microsoft Store, the Store itself — not this
application — handles download, installation and updates. That is covered by the
[Microsoft Privacy Statement](https://privacy.microsoft.com/privacystatement).

## Screen capture

The application's own windows are excluded from screen capture, so they do not appear in
screenshots or screen sharing. This is a display feature. It does not affect what the
application reads, and the application does not record the screen.

## Changes and contact

Changes to this policy are made in this file, and its history is public in the repository.
Questions: open an issue at <https://github.com/hideyukiMORI/nene-loupe/issues>.

---

# プライバシーポリシー（日本語）

最終更新: 2026-10-03 · 発行者: Hideyuki Mori

**NeNe Loupe は個人情報を収集・保存・送信しません。ネットワークには一切接続しません。**

## アプリが扱うデータ

| データ | 何をするか | どこへ行くか |
| --- | --- | --- |
| 画面の画素 | レンズの真下の 7×7 画素を読み、拡大表示と中心の色の表示に使います。 | 表示している間だけメモリにあります。ディスクに書きません。どこにも送りません。 |
| クリップボード | 色の値をクリックすると、その文字列（例: `#33E699`）をクリップボードに置きます。クリップボードの中身は読みません。 | お使いの端末のクリップボード。 |
| 設定 | テーマ・色の形式・常に最前面の 3 つを保存します。 | お使いの端末の小さなテキストファイル 1 つ（下記）。 |
| Windows のテーマ | テーマを「システムに従う」にしたとき、Windows のライト／ダークの設定を読みます。変更はしません。 | 保存しません。 |

アカウント、利用状況の送信、解析、広告、クラッシュ報告、アプリ独自の更新確認、第三者のサービスはありません。

## 設定ファイル

設定ファイルに入るのは、形式の版と上の 3 つの選択だけです。利用者・画面・採取した色についての情報は入りません。

- ZIP 版: `%LOCALAPPDATA%\NeNeLoupe\settings.v1.txt`。削除するまで端末に残ります。
- Microsoft Store 版（公開後）: このファイルがすでにあればそれを使います。無ければ Windows がアプリ専用の保存場所に置き、
  アンインストールすると消えます。

## ネットワーク

アプリはネットワークに接続しません。通信用のライブラリを使わずに作られています。配布している実行ファイルが
読み込む Windows のシステムライブラリは `KERNEL32`・`USER32`・`GDI32`・`ADVAPI32` の 4 つだけで、どれも通信の機能を
持ちません。許可表に無いライブラリを足すとビルドが失敗します。

Microsoft Store から入れた場合、ダウンロード・インストール・更新を行うのは Store であり、このアプリではありません。
その部分は [Microsoft のプライバシーに関する声明](https://privacy.microsoft.com/privacystatement)の対象です。

## 画面の取り込み

アプリ自身の窓は画面の取り込みから除外されるので、スクリーンショットや画面共有には映りません。これは表示上の機能で、
アプリが読むものには影響しません。アプリは画面を録画・記録しません。

## 変更と連絡先

このポリシーの変更はこのファイルに対して行い、履歴はリポジトリで公開されます。
質問は <https://github.com/hideyukiMORI/nene-loupe/issues> へ Issue としてお寄せください。
