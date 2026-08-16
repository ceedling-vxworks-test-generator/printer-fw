# printer-fw (RIManager)

プリンタFW向けライブラリ`RIManager`。C++17実装 + Cの公開API(`extern "C"`)を持ち、
`.so`/`.a`としてmainFWから利用される想定。

## 構成(2026-08-14〜: 0808を正としたポート後)

`構成変更/0808/Rim_new/`で正式に設計・実装されたアーキテクチャ(Adapter→
DataStore→Capability→Publisherの4層構成 + RIS/Storage)を、**このリポジトリの
メイン開発フォルダとして採用**した。

```
printer-fw/
├── core/                 RIManagerの実装本体(common/adapter/store/storage/
│                         capability/publisher)。0808/librim/RIManager/coreを移植
├── products/             機種依存の定義(printer_a/common/skeleton)
├── include/              Cミラーヘッダ(rim_data_id.h等)
├── test/                 gtestベースの機能・統合・性能・スレッド試験
├── docs/                 0808側のドキュメント
├── samples/
├── mainfw_mock/          0808側のmainFWモック(手動ライブラリ配置方式)
├── CMakeLists.txt        トップレベルビルド定義(0808から移植)
│
└── 構成変更/0808/Rim_new/ 0808として実際に提出されたツリー(設計書一式含む)。
                          読み取り専用のオリジナル。ここからのコピーが上記各所
```

`old_v2/`(直前までのメイン開発フォルダ)・`old/`(さらに古い試作)・
`adapter-layer-dev/`(Adapter単体の別開発環境)・`mainfw-mock/`(旧アーキテクチャ
向けモック)は、0808を唯一のコードベースとする方針のもとで削除した
(過去の実装は git 履歴からのみ参照できる)。

## ビルド

トップレベルでの単体ビルドは`RIMANAGER_BUILD_TESTS`で自身のgtestベース試験の
有無を切り替えられる(既定はトップレベルならON)。ネットワーク制限下でgtestの
`FetchContent`が失敗する場合は`OFF`にすることでライブラリ本体
(`rim-core`/`rim-product-printer-a`)のみをビルドできる。

```sh
cmake -S . -B build -DRIMANAGER_BUILD_TESTS=OFF
cmake --build build
```

## 履歴・設計の参照先

- 現行実装の設計根拠・レビュー記録: `構成変更/0808/Rim_new/01.設計/`
- 直前の実装(DeviceEvent方式のAdapter等)からの移行判断は、削除済みの
  `old_v2/`配下にあった各種md(`移行計画.md`・`変更方針.md`・`依存関係.md`・
  `class.md`等)を参照。内容は git 履歴(このブランチの移行前コミット)から辿れる
