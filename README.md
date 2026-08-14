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
├── old_v2/               このポート作業で退避した「直前までのメイン開発フォルダ」
│                         (DeviceEvent方式のAdapter、rim_api.cpp等)。過去の
│                         意思決定・実装の参照用に保持
├── old/                  さらに古い試作(old_v2よりさらに前の世代)
│
├── adapter-layer-dev/    Adapterレイヤ単体のin/out開発・試験環境
│                         (0808実装ベース、機種拡張フォルダ含む)
├── mainfw-mock/           旧(old_v2)アーキテクチャ向けに作成した、ライブラリ
│                         手動配置方式のmainFWモック(参考として残置)
│
└── 構成変更/0808/Rim_new/ 0808として実際に提出されたツリー(設計書一式含む)。
                          読み取り専用のオリジナル。ここからのコピーが上記各所
```

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
- 直前の実装(DeviceEvent方式のAdapter等)からの移行判断: `old_v2/`配下の各種md
  (`移行計画.md`・`変更方針.md`・`依存関係.md`・`class.md`等)
