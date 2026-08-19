# 新規 Product 追加手順

本ドキュメントは、RIManager に新しい Product を追加する際の標準手順を説明する。

現在のアーキテクチャでは、

```text
core
    = 製品非依存の実行エンジン

products
    = 製品固有の定義
```

である。

新規 Product の追加では、基本的に `core` を変更しない。

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

7. Snapshot Reader 作成

8. ProductDefinition 作成

9. ProductProfile 作成

10. Product Factory 作成

11. Normalize Function 作成（必要な場合）

12. ProductDefinition テスト追加

13. Functional Test 実行
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
    RIMDataId::kTemperatureSensorA,
    "TemperatureSensorA",
    kEnvironmentDomain.name,
    ValueType::kDouble
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
├ PrinterACapabilityBuilders.hpp
└ PrinterACapabilityBuilders.cpp
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

Capability の組み合わせを定義する。

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
 ├ PrintReady
 ├ Consumable
 └ Job
```

---

## Pipeline の責務

```text
どの Capability を
どの順番で生成するか
```

を定義する。

---

# 7. Snapshot Reader 作成

Product 用 SnapshotReader を作成する。

例:

```cpp
class ScannerXSnapshotReader
    : public IRIMSnapshotReader
{
};
```

配置例:

```text
Pipeline
├ ScannerXSnapshotReader.hpp
└ ScannerXSnapshotReader.cpp
```

---

## Snapshot Reader の責務

```text
ValueStore
    ↓
Snapshot生成
```

ProductDefinition を参照して Snapshot を構築する。

---

# 8. ProductDefinition 作成

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
```

の集約点である。

---

# 9. ProductProfile 作成

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

# 10. Product Factory 作成

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
SnapshotReader
CapabilityBuilder
Pipeline
```

など Product の構成を組み立てる。

---

# 11. Normalize Function 作成（必要な場合）

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

## DataItem ID 重複なし

```text
RIMDataId 重複禁止
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

# 禁止事項

## core に製品知識を入れない

禁止:

```cpp
TemperatureSensorA

PrinterA

DoorOpen

ScannerX
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
StoreWorker

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

MachineCapabilityStore
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
RIMDataDefinition

IDataDefinitionProvider

DataDefinitionProvider
```

---

## Capability 定義を重複させない

Capability の定義源は

```text
CapabilityItemDefinition
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

✅ SnapshotReader 作成

✅ ProductDefinition 作成

✅ ProductProfile 作成

✅ ProductFactory 作成

✅ Validation Test 追加

✅ Functional Test Green
```

---

# 一言で表すと

```text
1. Domain を定義する

2. DataItem を定義する

3. Capability を定義する

4. Pipeline を定義する

5. ProductDefinition に集約する

6. ProductProfile から公開する

7. Factory で組み立てる

8. core は変更しない
```
