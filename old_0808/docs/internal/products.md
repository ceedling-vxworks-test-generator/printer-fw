# 新規 Product 追加手順

本ドキュメントは、RIManager に新しい Product を追加する際の標準手順を説明する。

現在のアーキテクチャでは、

```text
core
    = 製品非依存の実行エンジン

products
    = 製品固有定義
```

である。

新規 Product の追加では、原則として `core` を変更しない。

---

# Product 構成

新しい Product を追加する場合、以下を作成する。

```text
products
└ <product>
    ├ Domain
    ├ DataItem
    ├ CapabilityItem
    ├ Pipeline
    │
    ├ <Product>ProductDefinition.*
    ├ <Product>ProductProfile.*
    ├ <Product>Factory.*
    │
    └ test
```

例:

```text
products
└ scanner_x
    ├ Domain
    ├ DataItem
    ├ CapabilityItem
    ├ Pipeline
    ├ ScannerXProductDefinition.*
    ├ ScannerXProductProfile.*
    ├ ScannerXFactory.*
    └ test
```

---

# 手順一覧

```text
1. Product ディレクトリ作成

2. Domain 定義作成

3. DataItem 定義作成

4. Capability 定義作成

5. Capability Builder 作成

6. Pipeline 定義作成

7. ProductDefinition 作成

8. ProductProfile 作成

9. Product Factory 作成

10. Normalize Function 作成（必要な場合）

11. ProductDefinition テスト追加

12. Functional Test 実行

13. End-to-End Test 実行

14. Performance Test 実行
```

---

# 1. Product ディレクトリ作成

例:

```text
products
└ scanner_x
```

構成:

```text
products
└ scanner_x
    ├ Domain
    ├ DataItem
    ├ CapabilityItem
    ├ Pipeline
    ├ ScannerXProductDefinition.*
    ├ ScannerXProductProfile.*
    ├ ScannerXFactory.*
    └ test
```

---

# 2. Domain 定義作成

製品が扱う Domain を定義する。

例:

```cpp
inline constexpr DomainDefinition
kEnvironmentDomain
{
    "Environment"
};

inline constexpr DomainDefinition
kConsumableDomain
{
    "Consumable"
};

inline constexpr DomainDefinition
kJobDomain
{
    "Job"
};
```

---

# 3. DataItem 定義作成

Product 内で扱う Canonical Data を定義する。

例:

```cpp
inline constexpr DataItemDefinition
kTemperatureSensorA
{
    RI_DATA_TEMPERATURE_SENSOR_A,
    "TemperatureSensorA",
    kEnvironmentDomain.name,
    ValueType::Double
};
```

---

## DataItem 設計ルール

DataItemDefinition は以下を表現する。

```text
ID
Name
Domain
ValueType
```

持たせないもの:

```text
製品ロジック
Capabilityロジック
通知ロジック
UI情報
```

---

# 4. Capability 定義作成

Product が提供する Capability を定義する。

例:

```cpp
inline constexpr CapabilityItemDefinition
kPrintReadyCapability
{
    RI_CAPABILITY_PRINT_READY,
    "PrintReady"
};
```

---

## Capability 設計ルール

CapabilityDefinition は

```text
何の Capability が存在するか
```

を定義する。

Capability の実装は core に存在する。

Capability の構成情報は products に存在する。

---

# 5. Capability Builder 作成

Snapshot から Capability を生成する処理を実装する。

例:

```cpp
bool BuildPrintReady(
    const RIMSnapshot& snapshot,
    PrintReadyCapability& capability);
```

配置例:

```text
CapabilityItem
├ ScannerXCapabilityBuilders.hpp
└ ScannerXCapabilityBuilders.cpp
```

---

## Capability Builder の責務

```text
Snapshot
    ↓
Capability生成
```

Capability の状態判定はここで行う。

---

# 6. Pipeline 定義作成

Capability の組み合わせと依存関係を定義する。

例:

```cpp
inline constexpr PipelineDefinition
kDefaultPipeline
{
    "DefaultPipeline",
    ...
};
```

例:

```text
DefaultPipeline
 ├ Environment
 ├ PrintReady
 ├ Consumable
 └ Job
```

---

## Pipeline の責務

```text
どの Capability を生成するか

どの Data を参照するか

Capability が何に依存するか
```

を定義する。

---

# Capability Dependency 定義

現在の RIManager は Data 変更を起点に影響 Capability のみを再評価する。

例:

```text
TemperatureSensorA
    ↓
Environment

UpperDoorOpen
    ↓
PrintReady

StapleLevel
    ↓
Consumable

JobId
    ↓
Job
```

Capability Dependency は ProductDefinition の一部として定義する。

---

# 7. ProductDefinition 作成

製品全体を構成する。

例:

```cpp
inline constexpr ProductDefinition
kScannerXProductDefinition
{
    kDomains,
    std::size(kDomains),

    kDataItems,
    std::size(kDataItems),

    kCapabilities,
    std::size(kCapabilities),

    kPipelines,
    std::size(kPipelines)
};
```

---

## ProductDefinition の責務

```text
製品に何が存在するか
```

を宣言する。

```text
Domain
DataItem
Capability
Pipeline
Dependency
```

の集約点である。

---

# 8. ProductProfile 作成

core と products の境界。

例:

```cpp
const ProductProfile
kScannerXProductProfile
{
    kScannerXProductDefinition
};
```

現在の ProductProfile は以下のみを保持する。

```cpp
struct ProductProfile
{
    const ProductDefinition& definition;
};
```

---

## ProductProfile の責務

```text
ProductDefinition を公開する
```

---

# 9. Product Factory 作成

Product 固有構成を生成する。

例:

```cpp
class ScannerXFactory
{
public:

    ProductRuntime Create() const;
};
```

配置例:

```text
Pipeline
├ ScannerXFactory.hpp
└ ScannerXFactory.cpp
```

---

## Factory の責務

```text
CapabilityManager

SnapshotReader

Pipeline

Worker構成
```

など Product 実行構成を組み立てる。

---

# 10. Normalize Function 作成（必要な場合）

値変換が必要な場合のみ作成する。

例:

```cpp
double KelvinToCelsius(
    double value);
```

配置場所:

```text
products/common
```

または

```text
products/<product>
```

---

## Normalize Function の責務

```text
Raw Value

↓

Canonical Value
```

変換。

例:

```text
300.15 Kelvin
    ↓
27.0 Celsius
```

---

# ProductDefinition Validation

最低限以下を満たすこと。

---

## Domain 数

```cpp
EXPECT_GT(
    definition.domainCount,
    0u);
```

---

## DataItem 数

```cpp
EXPECT_GT(
    definition.dataItemCount,
    0u);
```

---

## Capability 数

```cpp
EXPECT_GT(
    definition.capabilityCount,
    0u);
```

---

## Pipeline 数

```cpp
EXPECT_GT(
    definition.pipelineCount,
    0u);
```

---

## Capability Dependency 数

```cpp
EXPECT_GT(
    definition.dependencyCount,
    0u);
```

---

## DataItem ID 重複なし

```text
RIDataId 重複禁止
```

---

## DataItem 名重複なし

```text
Name 重複禁止
```

---

## Capability ID 重複なし

```text
CapabilityId 重複禁止
```

---

## Pipeline 名重複なし

```text
Pipeline 名重複禁止
```

---

# ErrorList サポート

ErrorList を提供する Product は以下を定義する。

```text
RI_DATA_ERROR_LIST
```

ValueType:

```cpp
ValueType::Binary
```

公開 API:

```cpp
RIM_SetErrorList()
RIM_GetErrorList()
```

通知対象:

```text
RI_TARGET_DATA
```

---

# Notification 検証

新規 Product では以下の通知確認を行う。

---

## Data Notification

```text
Data Changed
    ↓
Notification
```

確認項目:

```text
Callback

Mailbox

Notification Message
```

---

## Capability Notification

```text
Capability Changed
    ↓
Notification
```

確認項目:

```text
Callback

Mailbox

Notification Message
```

---

## ErrorList Notification

ErrorList を採用する Product は以下を検証する。

```text
SetErrorList
    ↓
Store
    ↓
Publish
    ↓
Callback
    ↓
Mailbox
    ↓
GetErrorList
```

---

# 性能検証

最低限以下を確認する。

```text
Capability Evaluation

Snapshot Build

Notification Delivery

Callback Latency
```

現在のベンチマーク実績:

```text
Capability Evaluation

10000 Evaluations
Average : 0.61 us

Snapshot Build

10000 Builds
Total : 2149 us

Callback Latency

Min : 37 us
Avg : 49 us
P95 : 95 us
P99 : 100 us
```

---

# 禁止事項

## core に製品知識を入れない

禁止:

```cpp
PrinterA

ScannerX

TemperatureSensorA

UpperDoorOpen
```

判断基準:

```text
特定製品を知っているか？
```

YES

```text
products
```

NO

```text
core
```

---

## Product ごとの Worker を作らない

禁止:

```cpp
PrinterAWorker

ScannerXWorker
```

推奨:

```cpp
DataStoreWorker

CapabilityWorker

PublisherWorker
```

---

## Product ごとの Store を作らない

禁止:

```cpp
PrinterAStore

ScannerXStore
```

推奨:

```cpp
ValueStore

CapabilityStore
```

---

## Data 定義を重複させない

Data の定義源は

```text
DataItemDefinition
```

のみ。

作ってはいけないもの:

```text
DataDefinitionProvider

IDataDefinitionProvider

RIMDataDefinition
```

---

## Capability 定義を重複させない

Capability の定義源は

```text
CapabilityItemDefinition
```

のみ。

---

## Notification 定義を重複させない

通知対象の定義源は

```text
ProductDefinition
```

のみ。

---

# 完了条件

以下が満たされれば Product 追加完了。

```text
✅ DomainDefinition 作成

✅ DataItemDefinition 作成

✅ CapabilityDefinition 作成

✅ CapabilityBuilder 作成

✅ PipelineDefinition 作成

✅ Capability Dependency 定義

✅ ProductDefinition 作成

✅ ProductProfile 作成

✅ ProductFactory 作成

✅ Validation Test 追加

✅ Functional Test Green

✅ End-to-End Test Green

✅ Performance Test Green
```

---

# 一言で表すと

```text
1. Domain を定義する

2. DataItem を定義する

3. Capability を定義する

4. Dependency を定義する

5. Pipeline を定義する

6. ProductDefinition に集約する

7. ProductProfile から公開する

8. Factory で組み立てる

9. core は変更しない
```