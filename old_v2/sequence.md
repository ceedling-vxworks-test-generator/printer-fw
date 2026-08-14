# RIManager Main Sequence

```mermaid
sequenceDiagram

    participant Adapter
    participant StoreDispatcher
    participant ValueStore

    participant SnapshotReader as AggregateRIMSnapshotReader
    participant Snapshot as RIMSnapshot

    participant CapabilityManager

    participant EnvironmentRule
    participant ErrorRule
    participant JobRule
    participant ConsumableRule
    participant PrintReadyRule

    participant CapabilityStore as MachineCapabilityStore

    participant PublishManager

    Adapter->>StoreDispatcher: DeviceEvent

    StoreDispatcher->>ValueStore: Store(RIMDataId, Value)

    Note over ValueStore: 最新値を保持

    SnapshotReader->>ValueStore: Read Values

    ValueStore-->>SnapshotReader: RIMValue Collection

    SnapshotReader-->>Snapshot: Build Snapshot

    Snapshot->>CapabilityManager: Generate()

    CapabilityManager->>EnvironmentRule: Evaluate()
    EnvironmentRule-->>CapabilityManager: EnvironmentCapability

    CapabilityManager->>ErrorRule: Evaluate()
    ErrorRule-->>CapabilityManager: ErrorCapability

    CapabilityManager->>JobRule: Evaluate()
    JobRule-->>CapabilityManager: JobCapability

    CapabilityManager->>ConsumableRule: Evaluate()
    ConsumableRule-->>CapabilityManager: ConsumableCapability

    CapabilityManager->>PrintReadyRule: Evaluate()
    PrintReadyRule-->>CapabilityManager: PrintReadyCapability

    CapabilityManager->>CapabilityStore: Store Capabilities

    CapabilityStore->>PublishManager: Capability Updated

    PublishManager-->>PublishManager: Diff Check

    PublishManager-->>PublishManager: Notify Subscribers
```

---

# Product起動時の構成シーケンス

```mermaid
sequenceDiagram

    participant Application

    participant ProductProvider as PrinterAProvider

    participant ErrorRegistry as PrinterAErrorRegistry
    participant DefinitionProvider as PrinterADataDefinitionProvider

    Application->>ProductProvider: Create

    ProductProvider->>ErrorRegistry: Own

    ProductProvider->>DefinitionProvider: Own

    Application->>ProductProvider: GetErrorRegistry()

    ProductProvider-->>Application: ErrorRegistry

    Application->>ProductProvider: GetDataDefinitionProvider()

    ProductProvider-->>Application: DataDefinitionProvider
```

---

# Definitionアクセスシーケンス
```mermaid
sequenceDiagram

    participant Client

    participant ProductProvider
    participant DefinitionProvider
    participant Registry as ProfileAwareDataDefinitionRegistry

    Client->>ProductProvider: GetDataDefinitionProvider()

    ProductProvider-->>Client: DefinitionProvider

    Client->>Registry: Create(provider)

    Client->>Registry: Find(id)

    Registry->>DefinitionProvider: GetDefinitions()

    DefinitionProvider-->>Registry: Definitions

    Registry-->>Client: RIMDataDefinition
```

---

# RIManager Module

# RIManager モジュール構成図（現状）

## アーキテクチャ概要

```mermaid
flowchart LR

    Adapter

    StoreInputQueue

    DataStoreWorker

    ValueStore

    SnapshotReader

    CapabilityInputQueue

    CapabilityWorker

    CapabilityManager

    CapabilityChangeDetector

    MachineCapabilityStore

    PublisherInputQueue

    PublisherWorker

    PublishManager

    ChangeNotifyManager

    SubscriberMailbox

    NotificationReceiver

    CallbackSubscriptionRegistry

    Adapter
        --> StoreInputQueue

    StoreInputQueue
        --> DataStoreWorker

    DataStoreWorker
        --> ValueStore

    DataStoreWorker
        --> SnapshotReader

    DataStoreWorker
        --> CapabilityInputQueue

    CapabilityInputQueue
        --> CapabilityWorker

    CapabilityWorker
        --> CapabilityManager

    CapabilityManager
        --> MachineCapabilityStore

    CapabilityWorker
        --> CapabilityChangeDetector

    CapabilityChangeDetector
        --> PublisherInputQueue

    PublisherInputQueue
        --> PublisherWorker

    PublisherWorker
        --> PublishManager

    PublishManager
        --> ChangeNotifyManager

    ChangeNotifyManager
        --> SubscriberMailbox

    ChangeNotifyManager
        --> CallbackSubscriptionRegistry

    SubscriberMailbox
        --> NotificationReceiver
```

---

# レイヤ構成

```mermaid
flowchart TD

    AdapterLayer

    DatastoreLayer

    CapabilityLayer

    PublisherLayer

    AccessorLayer

    AdapterLayer
        --> DatastoreLayer

    DatastoreLayer
        --> CapabilityLayer

    CapabilityLayer
        --> PublisherLayer

    PublisherLayer
        --> AccessorLayer
```

---

# Datastore 層

## 責務

```text
Data保持

Snapshot生成

Capabilityトリガ生成
```

## 構成

```mermaid
flowchart LR

    StoreInputQueue

    DataStoreWorker

    ValueStore

    AggregateRIMSnapshotReader

    CapabilityInputQueue

    StoreInputQueue
        --> DataStoreWorker

    DataStoreWorker
        --> ValueStore

    DataStoreWorker
        --> AggregateRIMSnapshotReader

    DataStoreWorker
        --> CapabilityInputQueue
```

---

# Capability 層

## 責務

```text
Snapshot評価

Capability生成

差分検出
```

## 構成

```mermaid
flowchart LR

    CapabilityInputQueue

    CapabilityWorker

    CapabilityManager

    EnvironmentRule

    ErrorRule

    PrintReadyRule

    ConsumableRule

    JobRule

    MachineCapabilityStore

    CapabilityChangeDetector

    PublisherInputQueue

    CapabilityInputQueue
        --> CapabilityWorker

    CapabilityWorker
        --> CapabilityManager

    CapabilityManager
        --> EnvironmentRule

    CapabilityManager
        --> ErrorRule

    CapabilityManager
        --> PrintReadyRule

    CapabilityManager
        --> ConsumableRule

    CapabilityManager
        --> JobRule

    CapabilityManager
        --> MachineCapabilityStore

    CapabilityWorker
        --> CapabilityChangeDetector

    CapabilityChangeDetector
        --> PublisherInputQueue
```

---

# Publisher 層

## 責務

```text
通知のみ

差分判定は行わない
```

## 構成

```mermaid
flowchart LR

    PublisherInputQueue

    PublisherWorker

    PublishManager

    ChangeNotifyManager

    SubscriberMailbox

    CallbackSubscriptionRegistry

    PublisherInputQueue
        --> PublisherWorker

    PublisherWorker
        --> PublishManager

    PublishManager
        --> ChangeNotifyManager

    ChangeNotifyManager
        --> SubscriberMailbox

    ChangeNotifyManager
        --> CallbackSubscriptionRegistry
```

---

# Accessor 層

## 責務

```text
利用者向け取得API

通知受信API
```

## 構成

```mermaid
flowchart LR

    NotificationReceiver

    RIManager

    CapabilityAccessor

    SubscriberMailbox
        --> NotificationReceiver

    NotificationReceiver
        --> RIManager

    CapabilityAccessor
        --> MachineCapabilityStore
```

---

# 差分判定フロー

```mermaid
sequenceDiagram

    participant Snapshot
    participant CapabilityManager
    participant CapabilityStore
    participant ChangeDetector
    participant PublisherQueue

    Snapshot->>CapabilityManager: Evaluate()

    CapabilityManager->>CapabilityStore: Store()

    CapabilityStore->>ChangeDetector: Current Capability

    ChangeDetector->>ChangeDetector: DiffCheck

    ChangeDetector->>PublisherQueue: PublisherInput
```

---

# 通知フロー

```mermaid
sequenceDiagram

    participant PublisherQueue

    participant PublisherWorker

    participant PublishManager

    participant NotifyManager

    participant Mailbox

    participant Callback

    PublisherQueue->>PublisherWorker: PublisherInput

    PublisherWorker->>PublishManager: PublishXxx()

    PublishManager->>NotifyManager: Notify()

    NotifyManager->>Mailbox: Push()

    NotifyManager->>Callback: Invoke()
```

---

# 各モジュール責務一覧

## ValueStore

```text
DataId → Value 保持
```

---

## AggregateRIMSnapshotReader

```text
ValueStore → Snapshot生成
```

---

## CapabilityManager

```text
Snapshot → Capability生成
```

---

## MachineCapabilityStore

```text
Capability保持
```

---

## CapabilityDiffChecker

```text
Capability比較ロジック
```

---

## CapabilityChangeDetector

```text
前回Capability保持

差分検出

PublisherInput生成
```

---

## PublisherWorker

```text
PublisherInput解釈
```

---

## PublishManager

```text
通知実行

Diff判定は持たない
```

---

## ChangeNotifyManager

```text
Mailbox通知

Callback通知
```

---

## SubscriberMailbox

```text
Pull型通知キュー
```

---

## CallbackSubscriptionRegistry

```text
Push型通知購読管理
```

---

# 現在の責務分離

```text
Datastore
    = Data保持

CapabilityManager
    = Capability生成

CapabilityChangeDetector
    = 差分検出

PublisherWorker
    = Publish指示解釈

PublishManager
    = 通知実行

ChangeNotifyManager
    = 配信制御
```

今回のリファクタ後は、

```text
CapabilityManager
    = 生成のみ

PublishManager
    = 通知のみ

CapabilityChangeDetector
    = 差分判定のみ
```

