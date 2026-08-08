## End-to-End シーケンス図

## 概要
RIManager全体の代表的な処理フローを示す。

本シーケンスはレイヤ間連携により、センサー値などのData情報を処理し、Capability(State)へ変換した状態通知、またはData情報そのものを利用したData通知としてSubscriberへ通知する一連の処理を表現する。

## レイヤー構成

```text
Adapter
  : RawDataをRIMDataItemへ変換

DataStore
  : Dataの保存・更新通知・Snapshot提供

Capability
  : DataからCapability(State)を生成

Publisher
  : CapabilityUpdateおよびDataUpdateを生成し通知

DataAccessor
  : CapabilityおよびDataを参照提供
    （Data通知生成時に利用）

```


## シーケンス
### シーケンス概要
Driverから入力されたDataがDataStoreへ保存され、
Capability通知またはData通知として
Subscriberへ通知されるシーケンス。


```mermaid
sequenceDiagram

participant Driver
participant Adapter as Adapter Layer
participant DataStore as DataStore Layer
participant Capability as Capability Layer
participant Publisher as Publisher Layer
participant DataAccessor as DataAccessor Layer
participant Subscriber

Driver->>Adapter: postCurrentValueInput(RawData)

Note over Adapter,DataStore: Layer Boundary (Adapter -> DataStore)
Adapter->>DataStore: postCurrentValueInput(RIMDataItem)

alt Capability通知

    Note over DataStore,Capability: Layer Boundary (DataStore -> Capability)
    DataStore->>Capability: notifyUpdated(DataDomainSet)

    Note over Capability,DataStore: Layer Boundary (Capability -> DataStore)
    Capability->>DataStore: capture(RIMSnapshotRequest)
    DataStore-->>Capability: RIMSnapshot

    Capability->>Capability: build(RIMSnapshot)
    Capability->>Capability: hasDifference(previous,current)
    Capability->>Capability: checkPriority(Capability)

    Note over Capability,Publisher: Layer Boundary (Capability -> Publisher)
    Capability->>Publisher: publish(RIMCapabilityPackage)

    Publisher->>Publisher: judgeNotification()
    Publisher->>Publisher: buildRIMCapabilityUpdate()

    Note over Publisher,Subscriber: Layer Boundary (Publisher -> Subscriber)
    Publisher->>Subscriber: notify(RIMCapabilityUpdate)

else Data通知

    Note over DataStore,Publisher: Layer Boundary (DataStore -> Publisher)
    DataStore->>Publisher: notifyUpdated(DataDomainSet)

    Publisher->>Publisher: judgeNotification()

    Note over Publisher,DataAccessor: Layer Boundary (Publisher -> DataAccessor)
    Publisher->>DataAccessor: getRegistry(DataDomain)

    Note over DataAccessor,DataStore: Layer Boundary (DataAccessor -> DataStore)
    DataAccessor->>DataStore: capture(RIMSnapshotRequest)
    DataStore-->>DataAccessor: RIMSnapshot

    DataAccessor-->>Publisher: RIMSnapshot

    Publisher->>Publisher: buildDataUpdate()

    Note over Publisher,Subscriber: Layer Boundary (Publisher -> Subscriber)
    Publisher->>Subscriber: notify(DataUpdate)

end
```