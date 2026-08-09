# adapter-layer-dev

`構成変更/0808/Rim_new/librim/RIManager/` の実装のうち、**Adapterレイヤに関係する
コードだけ**を抜き出し、単独でビルド・in/outテスト・デバッグができるようにした
開発環境。Capability/Publisher/Storage(RIS)/DataAccessor等、Adapterと
無関係なレイヤのコードは一切含めない。

## 何を「正」としたか

0808フォルダの**実装(コード)をそのまま正**として無改変で持ち込んでいる。
なお、`01.設計/Adapter/`の設計書(`postFaultInput`/`postGeneralInput`/
`postCurrentValueInput`と`RawDataInput`/`Rule`/`RuleResolver`という構成)は
**まだ実装されておらず**、実際のコードは`AdapterDispatcher::Dispatch(DeviceEvent)`
という1本の受理点と、`ProductDefinition`/`DataItemDefinition::normalize`経由の
正規化という、設計書とは別の(より古い)方式のままだった。この方式をそのまま
持ち込んでいる。設計書通りに新規実装したい場合は別途相談してほしい。

## ディレクトリ構成

```
adapter-layer-dev/
├── core/adapter/            Adapterレイヤ本体(0808から無改変でコピー)
│   ├── include/
│   │   ├── AdapterDispatcher.hpp   Dispatch(DeviceEvent) の受理点
│   │   ├── IHardwareAdapter.hpp    Initialize/Poll/Shutdown インターフェース
│   │   ├── PrinterAdapter.hpp      IHardwareAdapter実装(ポーリング型)
│   │   └── MockAdapter.hpp         IHardwareAdapter実装(試験用スタブ)
│   └── src/
│       ├── AdapterDispatcher.cpp   正規化(ProductDefinition経由)して
│       │                          StoreInputQueueへPush
│       └── PrinterAdapter.cpp
├── core/common/include/      Adapterがコンパイルに必要とする最小限の
│   │                         共通データモデル(0808から無改変)
│   ├── DeviceEvent.hpp       IN: {RIDataId, int}
│   ├── RIMDataItem.hpp       OUT: {id, valueType, value}
│   ├── RIMValue.hpp / ValueType.hpp
│   ├── BinaryStoreValue.hpp / BinaryInput.hpp
│   └── DataItemDefinition.hpp  id→normalize関数の対応レコード型
├── core/storage/include/RIMValueFactory.hpp   （0808から無改変）
├── core/store/include/StoreInputQueue.hpp     OUTの受け皿(std::queue+mutex)
├── include/{rim_data_id.h, rim_capability_id.h}  Cミラー enum（0808から無改変）
├── products/common/ProductDefinition.hpp      ProductDefinition構造体の宣言のみ
│                                              （0808から無改変。Find*関数の
│                                               本物の実装はCapability/Route/
│                                               Pipeline定義まで引き込むため含めない）
└── test/
    ├── TestProductCatalog.hpp/.cpp  ★新規作成。テスト専用の最小製品カタログ
    │                                (温度センサ・ステープルレベルの2件)と
    │                                FindDataItem()のテスト用実装
    └── AdapterInOutTest.cpp         ★新規作成。IN/OUTを検証する実行ファイル
```

`★新規作成`の2ファイル以外はすべて0808フォルダからの無改変コピー。

## なぜ本物のProductDefinitionを使わなかったか

`products/printer_a/ProductDefinition.cpp`(本物のFind*実装)は
`DomainDefinition`/`RouteDefinition`/`CapabilityItemDefinition`/
`PipelineDefinition`まで参照するため、これを含めるとCapability・Route・
Pipelineの定義一式を巻き込んでしまい、「Adapterレイヤだけ」から外れる。
そのため、`AdapterDispatcher.cpp`が実際に呼ぶ
`FindDataItem(const ProductDefinition&, RIDataId)`だけを、
テスト専用の最小カタログ(温度センサ=正規化なし、ステープルレベル=
0〜100クランプ+int32化 の2件)向けに新規実装した。

## ビルド・実行

```sh
cmake -S . -B build
cmake --build build
./build/adapter_inout_test
```

`ctest --test-dir build` でも実行できる。

## テスト内容(`test/AdapterInOutTest.cpp`)

実際にビルド・実行して確認済み(全5件成功)。

1. **KnownTemperatureIsForwarded** — 既知id(温度)を投入 → 正規化なしで
   doubleのままStoreInputQueueに積まれることを確認
2. **StapleLevelIsClampedAndConverted** — 範囲外の値(150, -10)を投入 →
   0〜100へクランプされ、int32へ変換されて積まれることを確認
3. **UnknownIdIsRejectedWithoutForwarding** — カタログに無いidを投入 →
   `Dispatch()`が`false`を返し、何もキューに積まれないことを確認
4. **PrinterAdapterPollDispatchesThroughSameQueue** — `IHardwareAdapter`
   インターフェース経由(`PrinterAdapter::Poll()`)でも同じ経路を通ることを確認
5. **MockAdapterSatisfiesInterface** — `MockAdapter`が`IHardwareAdapter`を
   満たすことを確認

## デバッグ

`.vscode/launch.json`の「Debug adapter_inout_test」構成でF5すれば、
`AdapterDispatcher::Dispatch()`内部にもステップイン・ブレークポイント
設置ができる。実際にgdbで`AdapterDispatcher.cpp`内にブレークポイントを
張り、正しく停止してバックトレース・引数の参照(`event.id`)ができることを
確認済み。

## 注意

- `build/`はビルド生成物のため`.gitignore`対象。
- gtestは使わない(ネットワーク制限下で取得できないため)。標準ライブラリの
  みに依存する単独実行ファイルとして実装し、実際にビルド・実行して確認する
  方式(このリポジトリの`rim_adapter_nfr_test`等と同じ方針)。
- `PrinterAdapter::Poll()`は温度センサの固定値(30)をハードコードしている
  (0808の実装のまま)。実機ポーリングにする場合は別途実装が必要。
