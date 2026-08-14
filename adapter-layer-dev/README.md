# adapter-layer-dev

`構成変更/0808/Rim_new/librim/RIManager/` の実装のうち、**Adapterレイヤに関係する
コードだけ**を抜き出し、単独でビルド・in/outテスト・デバッグができるようにした
開発環境。Capability/Publisher/Storage(RIS)/DataAccessor等、Adapterと
無関係なレイヤのコードは一切含めない。

## 何を「正」としたか

0808フォルダの**実装(コード)をそのまま正**として持ち込んでいる。
なお、`01.設計/Adapter/`の設計書(`postFaultInput`/`postGeneralInput`/
`postCurrentValueInput`と`RawDataInput`/`Rule`/`RuleResolver`という構成)は
**まだ実装されておらず**、実際のコードは`AdapterDispatcher::Dispatch(...)`
という1本の受理点と、`ProductDefinition`/`DataItemDefinition::normalize`経由の
正規化という、設計書とは別の(より古い)方式のままだった。この方式をベースに
している。設計書通りに新規実装したい場合は別途相談してほしい。

## IN引数へのcontext追加(このdevツリーでの変更点)

0808の`Dispatch()`はIN引数が`{id, value}`のみで、単位(摂氏/華氏など)を
区別する情報を持っていなかった。これを解消するため、第3引数として
`context`を追加した。

- **id** … 何のデータか(温度センサ等)を表す識別子
- **value** … 生の数値(例: 25)
- **context** … valueの解釈に必要な補足情報(例: 華氏か摂氏か)。**省略可・
  nullptr可**。nullptrの場合は「既に正規化済み」として扱い、単位変換は行わない

`id`と`context`をセットで`normalize()`(機種依存の変換ルール)へ渡すことで、
変換ルールを解決できるようにしている。この変更にあたり、以下の3点は
利用者の判断をそのまま踏襲している。

1. **context型** … 新しい型は起こさず、0808に既にある汎用の`RIMContext`
   (`std::array<ContextEntry, 8>`、`ContextEntry{key, value}`)をそのまま使う
2. **ルール解決の形** … id毎に1つのnormalize関数があり、その内部で
   contextの値によって分岐する、という現状の形(`TemperatureRule`パターン)を
   維持する。id+context のペアごとに別関数を引くような、より複雑な
   ルックアップ方式は採らない
3. **ルール定義場所** … 変換ルールは機種依存のため、テストコードや共通層には
   置かず、機種拡張目的のフォルダ(`products/printer_a/`)に切り出して定義する

## ディレクトリ構成

```
adapter-layer-dev/
├── core/adapter/                Adapterレイヤ本体(機種非依存)
│   ├── include/
│   │   ├── AdapterDispatcher.hpp   Dispatch(id, value, context=nullptr) の受理点
│   │   ├── IHardwareAdapter.hpp    Initialize/Poll/Shutdown インターフェース
│   │   ├── PrinterAdapter.hpp      IHardwareAdapter実装(ポーリング型)
│   │   └── MockAdapter.hpp         IHardwareAdapter実装(試験用スタブ)
│   └── src/
│       ├── AdapterDispatcher.cpp   FindDataItemでルールを引き、
│       │                          normalize(value, context)して
│       │                          StoreInputQueueへPush
│       └── PrinterAdapter.cpp      Poll()内でDispatch(id, value)を呼ぶ
├── core/common/include/          Adapterがコンパイルに必要とする最小限の
│   │                             共通データモデル
│   ├── RIMContext.hpp            IN補足情報の型(0808から無改変)
│   ├── RIMContextAccessor.hpp    RIMContextからkeyでvalueを引く汎用ヘルパ
│   │                             (★新規作成。機種非依存の共通インフラ)
│   ├── RIMDataItem.hpp           OUT: {id, valueType, value}
│   ├── RIMValue.hpp / ValueType.hpp
│   ├── BinaryStoreValue.hpp / BinaryInput.hpp
│   └── DataItemDefinition.hpp    id→normalize関数の対応レコード型
│                                 (normalize契約: RIMValue(*)(const RIMValue&,
│                                  const RIMContext*))
├── core/storage/include/RIMValueFactory.hpp   （0808から無改変）
├── core/store/include/StoreInputQueue.hpp     OUTの受け皿(std::queue+mutex)
├── include/{rim_data_id.h, rim_capability_id.h}  Cミラー enum（0808から無改変）
├── products/common/
│   ├── ProductDefinition.hpp          ProductDefinition構造体の宣言のみ
│   │                                  （0808から無改変。Find*関数の本物の
│   │                                   実装はCapability/Route/Pipeline定義
│   │                                   まで引き込むため含めない）
│   └── ProductDefinitionLookup.cpp    ★新規作成。Adapterが実際に使う
│                                      FindDataItem(id引数のみ)だけを実装した、
│                                      機種非依存の汎用ロジック
├── products/printer_a/           ★新規作成。PrinterA機種の変換ルール一式
│   │                             (機種拡張目的のフォルダ。新機種を追加する
│   │                              場合は products/<機種名>/ に同様のものを置く)
│   ├── PrinterAContextKeys.hpp        contextのkey/単位enum定義
│   │                                  (kContextKeyUnit, kUnitCelsius等)
│   ├── PrinterANormalizationFunctions.hpp/.cpp
│   │                                  NormalizeTemperatureSensorA(単位変換)、
│   │                                  NormalizeStapleLevel(クランプ)の実体
│   └── PrinterADataItems.hpp          id→変換ルール対応表(kPrinterADataItems)
│                                      とkPrinterAProduct(ProductDefinition)
└── test/
    └── AdapterInOutTest.cpp      IN/OUTを検証する実行ファイル
```

## なぜ本物のProductDefinitionを使わなかったか

`products/printer_a/ProductDefinition.cpp`(0808本物のFind*実装)は
`DomainDefinition`/`RouteDefinition`/`CapabilityItemDefinition`/
`PipelineDefinition`まで参照するため、これを含めるとCapability・Route・
Pipelineの定義一式を巻き込んでしまい、「Adapterレイヤだけ」から外れる。
そのため、`AdapterDispatcher.cpp`が実際に呼ぶ
`FindDataItem(const ProductDefinition&, RIDataId)`だけを、
`products/common/ProductDefinitionLookup.cpp`として機種非依存の汎用ロジックで
新規実装し、機種依存のデータ(`kPrinterAProduct`)は`products/printer_a/`側に
分離した。

## ビルド・実行

```sh
cmake -S . -B build
cmake --build build
./build/adapter_inout_test
```

`ctest --test-dir build` でも実行できる。

`core/adapter`(Adapter本体、機種非依存)と`products/`(機種依存データ)は
互いのシンボルを参照し合う(`AdapterDispatcher.cpp` → `FindDataItem`、
`PrinterADataItems.hpp` → `NormalizeXxx`)。両方をSTATICアーカイブのまま
別ターゲットにすると、リンカがアーカイブを1パスしか処理しないため未解決
シンボルが残ることがある(実際に発生することを確認済み)。そのため両方を
OBJECTライブラリにし、最終的に`adapter_inout_test`という1つの実体へまとめる
ことで、アーカイブ間の解決順という問題自体を無くしている
(`CMakeLists.txt`参照)。

## テスト内容(`test/AdapterInOutTest.cpp`)

実際にビルド・実行して確認済み(全6件成功)。

1. **KnownTemperatureWithoutContextIsTreatedAsNormalized** — 既知id(温度)を
   context無しで投入 → 「既に正規化済み」として単位変換されず、doubleの
   ままStoreInputQueueに積まれることを確認
2. **TemperatureContextSelectsUnitConversion** — context(単位=摂氏/華氏)を
   指定して温度を投入 → 摂氏0度・華氏32度のどちらもケルビン(273.15)へ
   正規化されて積まれることを確認(id+contextから変換ルールへ辿り着く
   振る舞いの確認)
3. **StapleLevelIsClampedAndConverted** — 範囲外の値(150, -10)を投入 →
   0〜100へクランプされ、int32へ変換されて積まれることを確認(単位を
   持たないデータでcontext省略可であることの確認も兼ねる)
4. **UnknownIdIsRejectedWithoutForwarding** — カタログに無いidを投入 →
   `Dispatch()`が`false`を返し、何もキューに積まれないことを確認
5. **PrinterAdapterPollDispatchesThroughSameQueue** — `IHardwareAdapter`
   インターフェース経由(`PrinterAdapter::Poll()`)でも同じ経路を通ることを確認
6. **MockAdapterSatisfiesInterface** — `MockAdapter`が`IHardwareAdapter`を
   満たすことを確認

## デバッグ

`.vscode/launch.json`の「Debug adapter_inout_test」構成でF5すれば、
`AdapterDispatcher::Dispatch()`内部にもステップイン・ブレークポイント
設置ができる。実際にgdbで`AdapterDispatcher.cpp`内にブレークポイントを
張り、正しく停止してバックトレース・引数の参照(`id`/`value`/`*context`、
context無し呼び出しでは`context == nullptr`)ができることを確認済み。

## 注意

- `build/`はビルド生成物のため`.gitignore`対象。
- gtestは使わない(ネットワーク制限下で取得できないため)。標準ライブラリの
  みに依存する単独実行ファイルとして実装し、実際にビルド・実行して確認する
  方式(このリポジトリの`rim_adapter_nfr_test`等と同じ方針)。
- `PrinterAdapter::Poll()`は温度センサの固定値(30)をハードコードしている
  (0808の実装のまま)。実機ポーリングにする場合は別途実装が必要。
