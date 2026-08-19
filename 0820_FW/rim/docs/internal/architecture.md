# RIManager Architecture

ReactiveInfoManager (RIManager) は、製品固有定義と実行エンジンを明確に分離した Capability ベースの情報管理システムである。

RIManager は Device の生データを Canonical Data に正規化し、Canonical Data・Snapshot・Capability・Notification を提供する。

---

# Design Principles

RIManager は以下の責務分離を採用する。

```text
core
    = 実行エンジン (How)

products
    = 製品定義 (What)

products/common
    = 共通ルールライブラリ
```

設計上の基本方針は以下の通り。

```text
core
    は製品を知らない

products
    は製品仕様を宣言する

Runtime
    は RIMDataId を唯一の識別子として扱う

DataItemDefinition
    を Canonical Data Model とする

DataItemDefinition
    を唯一のデータ定義ソースとする

Notification
    は Data と Capability の両方を扱う
```

---

# Layer Structure

```mermaid
graph TD

    Product[Product Definition]

    Core[Core Engine]

    Client[Client/API]

    Product --> Core

    Core --> Client
```

---

# Repository Structure

```text
RIManager
├─ include
├─ core
│  ├─ adapter
│  ├─ capability
│  ├─ common
│  ├─ datastore
│  ├─ publisher
│  └─ storage
│
├─ products
│  ├─ common
│  ├─ printer_a
│  └─ skeleton
│
└─ test
```

---

# Responsibility Separation

## core

core は製品知識を持たない。

持たないべきもの:

```text
PrinterA
TemperatureSensorA
HumiditySensor
Door
Consumable
Job
Environment
PrintReady
```

責務:

```text
入力処理

データ保存

Snapshot生成

Capability生成

差分検出

通知
```

具体例:

```text
StoreDispatcher
ValueStore

AggregateRIMSnapshotReader
RIMSnapshot

CapabilityManager
MachineCapabilityStore

PublishManager
```

---

## products

products は製品仕様を保持する。

責務:

```text
どんなDataItemが存在するか

どんなDomainが存在するか

どんなCapabilityが存在するか

どんなPipelineが存在するか

どんな正規化が必要か
```

---

## products/common

製品横断で再利用可能な定義や処理を配置する。

例:

```cpp
IdentityNormalize()
```

将来候補:

```cpp
PercentNormalize()

CelsiusToKelvinNormalize()

KelvinToCelsiusNormalize()
```

---

# Product Boundary

core と products の境界は ProductProfile である。

```mermaid
graph LR

    Core --> ProductProfile

    ProductProfile --> ProductDefinition
```

```cpp
struct ProductProfile
{
    const ProductDefinition& definition;
};
```

core は ProductProfile 経由でのみ製品情報へアクセスする。

---

# Product Provider

製品構成の入口。

```mermaid
classDiagram

class IProductProvider {
    +GetProfile()
}

class PrinterAProvider
class SkeletonProvider

IProductProvider <|-- PrinterAProvider
IProductProvider <|-- SkeletonProvider
```

---

# ProductDefinition

製品仕様の中心となる宣言モデル。

```mermaid
graph TD

    ProductDefinition

    ProductDefinition --> Domains
    ProductDefinition --> DataItems
    ProductDefinition --> Capabilities
    ProductDefinition --> Pipelines
```

```cpp
struct ProductDefinition
{
    const DomainDefinition* domains;
    std::size_t domainCount;

    const DataItemDefinition* dataItems;
    std::size_t dataItemCount;

    const CapabilityItemDefinition* capabilities;
    std::size_t capabilityCount;

    const PipelineDefinition* pipelines;
    std::size_t pipelineCount;
};
```

ProductDefinition は

```text
製品に何が存在するか
```

のみを定義する。

---

# DataItemDefinition

RIManager における Canonical Data Model。

```cpp
struct DataItemDefinition
{
    RIMDataId id;

    std::string_view name;

    std::string_view domain;

    ValueType valueType;
};
```

例:

```text
TemperatureSensorA
TemperatureSensorB
HumiditySensor

UpperDoorOpen
RightDoorOpen
LeftDoorOpen

JobActive
JobId

StapleLevel

ErrorList
```

---

# Data Model

DataItem はシステム内部で扱う正規化後データを定義する。

```text
DataItem
 ├ ID
 ├ Name
 ├ Domain
 └ ValueType
```

現在の設計では

```text
DataItemDefinition
```

が唯一のデータ定義ソースである。

---

# Runtime Identifier

Runtime では `RIMDataId` を利用する。

```mermaid
graph LR

    Adapter --> RIMDataId

    Storage --> RIMDataId

    Snapshot --> RIMDataId

    Publisher --> RIMDataId

    Subscriber --> RIMDataId
```

例:

```cpp
SetValue(
    RIMDataId id,
    Value value);

FindValue(
    RIMDataId id);
```

---

# Sensor Definition

Adapter 入力と Canonical Data の橋渡しを行う。

```mermaid
graph LR

    SensorId --> SensorDefinition

    SensorDefinition --> RIMDataId
```

責務:

```text
Physical Sensor
    ↓
Canonical DataItem
```

現在は製品側で SensorDefinition を管理する。

---

# Normalize Function

正規化処理は products 側に配置する。

```mermaid
graph LR

    RawValue --> NormalizeFunction

    NormalizeFunction --> CanonicalValue
```

例:

```cpp
IdentityNormalize()
```

将来的には:

```cpp
PercentNormalize()

KelvinToCelsiusNormalize()

CelsiusToKelvinNormalize()
```

---

# Adapter Responsibility

Adapter の責務。

```text
入力受信
↓
正規化
↓
Store投入
```

入力:

```text
Raw Device Data
```

出力:

```text
RIMDataId
Canonical Value
```

---

# Adapter Architecture

Rule レイヤは採用しない。

削除済み:

```text
TemperatureSensorARule
TemperatureSensorBRule
HumiditySensorRule
DoorSensorRule

ISensorRule
IdentityRule
```

目標構造:

```mermaid
graph TD

    Input --> Adapter

    Adapter --> NormalizeFunction

    NormalizeFunction --> Store
```

---

# Storage Responsibility

Storage は Canonical Data の保存と検証を担当する。

責務:

```text
Store

Lookup

Type Validation

Snapshot Generation

Snapshot Management
```

---

# Definition Lookup

Storage は ProductDefinition を参照して検索する。

```mermaid
graph LR

    RIMDataId --> Lookup

    Name --> Lookup

    Lookup --> DataItemDefinition
```

---

# Type Validation Policy

型生成責任と型検証責任を分離する。

```text
Adapter
 └ 正しい型を生成する責任

Storage
 └ 型を検証する責任
```

検証例:

```cpp
definition->valueType
```

と

```cpp
value.GetType()
```

を比較する。

Storage は最後の防壁として機能する。

---

# Runtime Flow

現在の実行パイプライン。

```text
Adapter
    ↓

StoreDispatcher

    ↓

ValueStore

    ↓

AggregateRIMSnapshotReader

    ↓

CapabilityInput
{
    snapshot,
    changedDataId
}

    ↓

CapabilityWorker

    ├ Data Notification
    └ Capability Evaluation

    ↓

PublisherWorker

    ↓

PublishManager
```

---

# Runtime Class Diagram

```mermaid
classDiagram

class PrinterAdapter
class AdapterDispatcher

class StoreInputQueue
class DataStoreWorker
class ValueStore

class AggregateRIMSnapshotReader
class RIMSnapshot

class CapabilityInputQueue
class CapabilityInput

class CapabilityWorker
class CapabilityManager

class MachineCapabilityStore

class CapabilityDiffChecker
class CapabilityChangeDetector

class PublisherInputQueue
class PublisherWorker

class PublishManager
class ChangeNotifyManager

class SubscriptionStore

class SubscriberMailbox
class NotificationMailboxReader

class CallbackSubscriptionRegistry

class CapabilityAccessor

PrinterAdapter --> AdapterDispatcher

AdapterDispatcher --> StoreInputQueue

StoreInputQueue --> DataStoreWorker

DataStoreWorker --> ValueStore

DataStoreWorker --> AggregateRIMSnapshotReader

DataStoreWorker --> CapabilityInputQueue

CapabilityInputQueue --> CapabilityWorker

CapabilityWorker --> CapabilityManager

CapabilityManager --> MachineCapabilityStore

CapabilityWorker --> CapabilityChangeDetector

CapabilityChangeDetector --> CapabilityDiffChecker

CapabilityWorker --> PublisherInputQueue

PublisherInputQueue --> PublisherWorker

PublisherWorker --> PublishManager

PublishManager --> ChangeNotifyManager

ChangeNotifyManager --> SubscriptionStore

ChangeNotifyManager --> SubscriberMailbox

ChangeNotifyManager --> CallbackSubscriptionRegistry

SubscriberMailbox --> NotificationMailboxReader

CapabilityAccessor --> MachineCapabilityStore
```

---

# CapabilityInput

Capability 再評価要求。

```cpp
struct CapabilityInput
{
    RIMSnapshot snapshot;

    RIMDataId changedDataId;
};
```

現在:

```text
snapshot
    → Capability再評価

changedDataId
    → Data Notification生成
```

将来:

```text
changedDataId
    ↓
Affected Capability Lookup
    ↓
Partial Capability Evaluation
```

---

# Data Flow

```mermaid
graph TD

    Input["Device / API Input"]

    Adapter["Adapter Engine"]

    Normalize["Normalize Function"]

    Store["ValueStore"]

    Snapshot["RIMSnapshot"]

    Capability["Capability Engine"]

    DataNotify["Data Notification"]

    CapabilityNotify["Capability Notification"]

    Publisher["Publisher"]

    Client["Client"]

    Input --> Adapter

    Adapter --> Normalize

    Normalize --> Store

    Store --> Snapshot

    Snapshot --> Capability

    Store --> DataNotify

    Capability --> CapabilityNotify

    DataNotify --> Publisher

    CapabilityNotify --> Publisher

    Publisher --> Client
```

---

# Snapshot Flow

```mermaid
sequenceDiagram

    participant Storage

    participant SnapshotReader

    participant Snapshot

    SnapshotReader->>Storage: Read()

    Storage-->>SnapshotReader: Current Values

    SnapshotReader-->>Snapshot: Build Snapshot
```

---

# Capability Flow

現在は Snapshot 全体を再評価する。

```mermaid
sequenceDiagram

    participant DataStoreWorker

    participant CapabilityWorker

    participant CapabilityManager

    participant CapabilityStore

    participant ChangeDetector

    DataStoreWorker->>CapabilityWorker: CapabilityInput

    CapabilityWorker->>CapabilityManager: Evaluate(snapshot)

    CapabilityManager->>CapabilityStore: Update()

    CapabilityWorker->>ChangeDetector: Detect()

    ChangeDetector-->>CapabilityWorker: Changes
```

---

# Notification

RIManager は Data Notification と Capability Notification を提供する。

---

## Data Notification

Canonical Data の変更通知。

```cpp
NotificationTargetType::Data
```

例:

```text
TemperatureSensorA

HumiditySensor

UpperDoorOpen

ErrorList
```

---

## Capability Notification

Capability 状態変更通知。

```cpp
NotificationTargetType::Capability
```

例:

```text
Environment

PrintReady

Consumable

Job
```

---

# Capability

Capability は製品機能を定義する。

```mermaid
graph LR

    Capability --> RequiredDataItems

    RequiredDataItems --> DataItem
```

例:

```cpp
requiredDataItems =
{
    "UpperDoorOpen",
    "StapleLevel"
};
```

---

# Pipeline

Pipeline は Capability の集合である。

```mermaid
graph LR

    Pipeline --> Capability
```

例:

```text
OperationalPipeline
 ├ PrintReady
 ├ Consumable
 └ Job
```

---

# Current Capability Evaluation Strategy

現在:

```text
Data Changed
    ↓
Snapshot Build
    ↓
Evaluate All Capabilities
    ↓
Detect Changes
    ↓
Notify
```

利点:

```text
単純

正しさを保証しやすい

依存関係管理不要
```

欠点:

```text
影響しないCapabilityも再評価

Capability数増加で効率低下

Dependency情報を活用していない
```

---

# Future Architecture

将来的には ProductDefinition に依存関係を定義し、

```text
ChangedDataId
    ↓
AffectedCapabilities
    ↓
Evaluate Only Affected Capabilities
    ↓
Notify
```

へ移行する。

例:

```text
UpperDoorOpen
    ↓
Environment

PrintReady
```

---

# Adapter Design Guideline

## Bad

```cpp
TemperatureSensorAAdapter

HumiditySensorAdapter

DoorSensorAdapter
```

理由:

```text
Product知識を持っている
```

---

## Good

```cpp
IAdapter

AdapterDispatcher

AdapterEngine
```

理由:

```text
Productを知らない
```

---

# Placement Rules

## Rule 1

```text
PrinterA を知っているか？
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

## Rule 2

```text
DataItem を定義しているか？
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

## Rule 3

```text
これは定義か？

これは実行か？
```

定義

```text
products
```

実行

```text
core
```

---

# Target Architecture

```mermaid
graph TB

    subgraph Products
        Profile[ProductProfile]
        ProductDef[ProductDefinition]
        DataItem[DataItemDefinition]
        Domain[DomainDefinition]
        Capability[CapabilityDefinition]
        Pipeline[PipelineDefinition]
        Normalize[Normalize Functions]
        SensorDef[SensorDefinition]
    end

    subgraph Core
        Adapter[Adapter Engine]
        Store[ValueStore]
        Snapshot[RIMSnapshot]
        CapabilityEngine[Capability Engine]
        Publisher[Publisher]
    end

    ProductDef --> Adapter

    DataItem --> Adapter

    Normalize --> Adapter

    SensorDef --> Adapter

    Adapter --> Store

    Store --> Snapshot

    Snapshot --> CapabilityEngine

    CapabilityEngine --> Publisher
```

---

# Current Status

```text
✅ Core / Products 分離
✅ ProductProfile 導入
✅ ProductProvider 導入
✅ ProductDefinition 導入
✅ DomainDefinition 導入
✅ DataItemDefinition 導入
✅ CapabilityDefinition 導入
✅ PipelineDefinition 導入
✅ Snapshot中心設計
✅ Runtime Identifier 統一 (RIMDataId)
✅ Adapter 正規化設計
✅ Storage 型検証設計
✅ ProductDefinition Lookup API
✅ Rule レイヤ削除
✅ ProfileAwareDataDefinitionRegistry 削除
✅ IDataDefinitionProvider 削除
✅ RIMDataDefinition 削除
✅ DataItemDefinition を唯一のデータ定義へ統一
✅ NotificationMailboxReader 導入
✅ SubscriptionStore 導入
✅ Data Notification
✅ Capability Notification
✅ ChangedDataId伝搬
✅ CapabilityInput 導入
✅ Functional Test Green
✅ End-to-End Test Green
✅ 177 Tests Passed

⬜ Capability Dependency Metadata
⬜ ChangedDataId → Capability Reverse Lookup
⬜ Partial Capability Evaluation
⬜ Dependency Driven Capability Generation
```

## Summary

```text
core
    = How

products
    = What

products/common
    = Shared Rules

DataItemDefinition
    = Canonical Data Model

ProductDefinition
    = Product Specification

CapabilityInput
    = Snapshot + ChangedDataId

Notification
    = Data + Capability
```