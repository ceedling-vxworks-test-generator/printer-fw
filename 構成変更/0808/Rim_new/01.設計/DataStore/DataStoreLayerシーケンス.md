# DataStore Layer

## 1.Fault / Report / Parameter シーケンス

### シーケンス概要
Adapterから受け取ったDataをQueue経由でStoreへ反映し、更新DomainをCapabilityへ通知するシーケンス。

```mermaid
sequenceDiagram
participant Adapter as Adapter Layer
participant Input as DataStore:RIMInputGateway
participant Queue as DataStore:Queue
participant Dispatcher as DataStore:Dispatcher
participant Store as DataStore:Store
participant Capability as Capability:RIMCapabilityManager

Adapter->>Input: postXxxInput(RIMDataItem)
Input->>Queue: enqueue()
Queue->>Dispatcher: notify()
Dispatcher->>Queue: dequeue()
Queue-->>Dispatcher: RIMDataItem
Dispatcher->>Store: apply(item)

Note over Store,Capability: Layer Boundary (DataStore -> Capability)

alt ドメインが更新された場合
    Store-->>Capability: notifyUpdated(DataDomainSet)
end
```

## 2.Value シーケンス

### シーケンス概要
最新ValueをBufferで管理し、Storeへ反映後にCapabilityへ更新通知するシーケンス。

```mermaid
sequenceDiagram
participant Adapter as Adapter Layer
participant Input as DataStore:RIMInputGateway
participant Buffer as DataStore:ValueBuffer
participant Dispatcher as DataStore:ValueDispatcher
participant Store as DataStore:ValueStore
participant Capability as Capability:RIMCapabilityManager
participant Publisher as Publisher:PublishManager

Note over Adapter,Input: Layer Boundary (Adapter -> DataStore)
Adapter->>Input: postCurrentValueInput(RIMDataItem)
Input->>Buffer: post(RIMDataItem)

loop 周期タスク
    Dispatcher->>Buffer: takeUpdatedValues()
    Buffer-->>Dispatcher: UpdatedValues
    Dispatcher->>Store: apply(RIMDataItem)
end

Note over Store,Capability: Layer Boundary (DataStore -> Capability)
Store-->>Capability: DataDomainSet

Note over Store,Publisher: Layer Boundary (DataStore -> Publisher)
Store-->>Publisher: DataDomainSet
```

## 3.Snapshot取得シーケンス

### シーケンス概要
CapabilityまたはDataAccessorからの要求に応じて、Store群から整合性のあるRIMSnapshotを生成して返却するシーケンス。

### 概要
CapabilityやDataAccessorからの要求に応じて、Store群の状態を整合性のあるRIMSnapshotとして提供するシーケンス。

```mermaid
sequenceDiagram
participant Capability as Capability Layer
participant Reader as DataStore:AggregateSnapshotReader
participant ErrorStore as DataStore:ErrorStore
participant ReportStore as DataStore:ReportStore
participant ValueStore as DataStore:ValueStore
participant ParameterStore as DataStore:ParameterStore

Note over Capability,Reader: Layer Boundary (Capability -> DataStore)

Capability->>Reader: capture(domains)
Reader->>ErrorStore: makeSnapshot()
Reader->>ReportStore: makeSnapshot()
Reader->>ValueStore: makeSnapshot()
Reader->>ParameterStore: makeSnapshot()
ErrorStore-->>Reader: ErrorSnapshot
ReportStore-->>Reader: ReportSnapshot
ValueStore-->>Reader: ValueSnapshot
ParameterStore-->>Reader: ParameterSnapshot
Reader-->>Capability: RIMSnapshot
```
---