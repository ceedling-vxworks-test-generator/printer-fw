# printer-fw — ReactiveInfoManager (RIM)

印刷機の組込みソフトウェア向け **共通フレームワーク**。複数機種へ流用できる、
長期運用可能なライブラリ。機種依存部(`products/`)と機種非依存部(`core/`)を
明確に分離する。

- **実装言語**: 組込み向け C++17。例外・RTTI・動的確保を使わない
  (`-fno-exceptions -fno-rtti`、定常運転中の動的確保ゼロを実測で確認)。
- **公開 API**: `extern "C"` の C ABI(`include/rim_api.h`)。C 言語の mainFW から
  そのままリンク・呼び出しできる。
- **スレッドモデル**: RIM 内で完結する。`RIManager_Start()` でワーカスレッドを
  3本(Datastore/Capability/Publisher)起動し、呼び出し側は `Push` するだけでよい。

過去の設計変遷は `old/`(最初期の5レイヤ+Observer方式)と
`移行計画.md`(現行アーキテクチャへ至る過程の記録)を参照。

## アーキテクチャ

```
Adapter(L1) → Datastore(L2) → Capability(L3) → Publisher(L4)
                                    │
                                    └→ Accessor(隣接・Pull参照)
```

### L1 Adapter — `core/adapter/`

外部から入る受理点は `RawDataInput::Push(id, RawValue, ctx?)` の1本のみ。
`RawValue` はスカラ/構造体/配列を非所有参照で受ける(コピーしない)。
`IRuleResolver::SelectRule(id)` が機種側のテーブルから規則を引き、
`IRule::Convert` が単位換算・クランプ等の正規化を行う。

同じ `id` へ摂氏でも華氏でも投入でき、どちらで来たかは
`DataContext::unit` で判別する(id を単位ごとに増やさない)。

### L2 Datastore — `core/store/` `core/storage/`

`ICentralInputPort::Post` で受けた項目を、`DataContext::faultState` の
有無で「現在値」か「異常報告」かに振り分ける(判別は L2 の責務)。
現在値は `ValueStore`(id 添字の固定長配列)へ、異常報告は
`FaultApplier` 経由で `ErrorRepository` へ入る。レーンは1本のまま。

`ValueStore` から作った `RIMSnapshot` を Capability 段のキューへ送る。

異常の**全件スナップショット**(FaultInfoList 相当)を一括反映したい場合は、
`RIManager_PushFaultSnapshot()` を使う。1件ずつの `Push`/`AddError` とは
別経路として**併存**し、呼ぶたびに「今存在する異常の全件」を渡す(差分ではない)。
一覧に無いコードは削除される(回復扱いにはしない)。反映は
`FaultApplier::ApplySnapshot` が `ErrorRepository` を直接書き換える形で行い、
`DataStoreWorker` の同じスレッドが処理するため、書き込みスレッドは単発の
異常報告と変わらず1本のままである。

### L3 Capability — `core/capability/`

Capability は **型消去**されている(`CapabilityId` + `CapabilityPayload` の
不透明バイト列)。Core は Capability の中身を一切知らない。規則
(`ICapabilityRule`)は機種側(`products/<機種>/CapabilityItem/`)が持ち、
`ICapabilityRuleProvider` 経由で Core の `CapabilityEvaluator` へ登録される。

生成した値は `CapabilityStore` に保持され、`CapabilityChangeTracker` が
前回値とのバイト比較で変化を検知する(**値が変化したときだけ配信**する。
生値が動いても Capability が変わらなければ配信は起きない)。

### L4 Publisher — `core/publisher/`

`SubscriberMailbox`(ポーリング購読用、固定長リング)と
`CallbackSubscriptionRegistry`(コールバック購読用)の両方に配信する。
コールバックは `std::function` ではなく関数ポインタ + `userData`
(動的確保も例外も経路に持ち込まない)。

### core ↔ products の接続

`RIMDataId`(データ種別)と Capability の規則は **機種側にしかない**。
Core は `#include "RIMDataId.hpp"` のように名前だけを知っており、実体は
ビルド時のインクルードパス解決で機種側から供給される(ヘッダ注入。
`FreeRTOSConfig.h` と同じ考え方)。関数の解決は
`core/common/include/ProductBinding.hpp` で宣言だけ行い、定義は
`products/<機種>/Pipeline/<機種>ProductBinding.cpp` に置いてリンク時に
解決する。**`core/` のどのファイルにも機種名は一切現れない。**

新機種を追加するときは `products/skeleton/` を雛形にする。

## ディレクトリ構成

```
core/                  機種非依存の共通ロジック
products/printer_a/    PrinterA 固有の定義・規則
products/skeleton/     新機種を起こすときの雛形
include/               公開 C API(rim_api.h 等)
test/                  gtest ベースの単体・結合・E2E・性能試験
test/capi/             C から実際にリンクして実行する試験(gtest 不使用)
old/                   最初期の設計(5レイヤ+Observer方式)。参考用に保持
```

## ビルド

```sh
cmake -S . -B build -DRIMANAGER_BUILD_TESTS=OFF
cmake --build build --target rim_capi_smoke
ctest --test-dir build
```

`RIMANAGER_BUILD_TESTS`(既定: トップレベルビルドなら ON)を有効にすると
gtest ベースの試験(`rimanager_functional_test` / `rimanager_performance_test`)
もビルドする。gtest は CMake の `FetchContent` で取得するため、ネットワーク制限
下では `OFF` にすること(`rim_capi_smoke` はこの設定と独立にビルドできる)。

### mainFW への組み込み

```sh
cmake --install build --prefix /path/to/prefix
```

`find_package(rimanager)` または `pkg-config rimanager` のどちらからでも
利用できる。

```cmake
find_package(rimanager REQUIRED)
target_link_libraries(mainFW PRIVATE rimanager::rimanager)
```
