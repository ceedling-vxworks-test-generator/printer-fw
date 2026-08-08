# Publisher Layer

## 1.Capability変更通知シーケンス

### シーケンス概要
Capability変更時に通知対象Subscriberを判定し、CapabilityUpdateを生成して配送するシーケンス。

```mermaid
sequenceDiagram
participant Capability as Capability Layer
participant PIQ as Publisher:PublisherInputQueue
participant Manager as Publisher:PublishManager
participant Latest as Publisher:LatestRIMCapabilityStore
participant Change as Publisher:ChangeNotifyManager
participant Registry as Publisher:SubscriptionRegistry
participant Builder as Publisher:RIMCapabilityUpdateBuilder
participant Mailbox as Publisher:SubscriberMailbox
participant Subscriber

Note over Capability,PIQ: Layer Boundary (Capability -> Publisher)
Capability->>PIQ: publish(RIMCapabilityPackage)
PIQ->>Manager: dequeue()

Manager->>Latest: updateRIMCapability(RIMCapability)
Manager->>Change: judgeNotification()
Manager->>Registry: findAll()
Registry-->>Manager: SubscriptionInfo

loop 各購読者
    Manager->>Change: judgeNotification()
    Change-->>Manager: NotifyTarget

    alt 通知対象の場合
        Manager->>Builder: buildRIMCapabilityUpdate()
        Builder->>Latest: getRIMCapability()
        Latest-->>Builder: RIMCapability
        Builder-->>Manager: RIMCapabilityUpdate
        Manager->>Mailbox: push(RIMCapabilityUpdate)
    end
end

Note over Mailbox,Subscriber: Layer Boundary (Publisher -> Subscriber)
Mailbox->>Subscriber: notify(RIMCapabilityUpdate)
```

## 2.Capability周期通知シーケンス

### シーケンス概要
登録された周期通知条件に基づいてCapabilityUpdateを生成し、Subscriberへ配送するシーケンス。

```mermaid
sequenceDiagram
participant Timer
participant Manager as Publisher:PublishManager
participant Periodic as Publisher:PeriodicNotifyManager
participant Latest as Publisher:LatestRIMCapabilityStore
participant Builder as Publisher:RIMCapabilityUpdateBuilder
participant Mailbox as Publisher:SubscriberMailbox

Timer->>Manager: periodicTick()
Manager->>Periodic: getDueConditions()
Periodic-->>Manager: DueConditions

loop 通知条件成立の場合
    Manager->>Latest: hasRIMCapability()

    alt Capabilityが利用可能の場合
        Manager->>Builder: buildRIMCapabilityUpdate()
        Builder->>Latest: getRIMCapability()
        Latest-->>Builder: RIMCapability
        Builder-->>Manager: RIMCapabilityUpdate
        Manager->>Mailbox: push(RIMCapabilityUpdate)
    end

    Manager->>Periodic: updateNextTime()
end
```

## 3.Data変更通知シーケンス

### シーケンス概要
DataStoreから通知されたDataDomain更新を契機に通知対象Subscriberを判定し、DataAccessorから取得した最新Dataを用いてDataUpdateを生成し配送するシーケンス。

```mermaid
sequenceDiagram
participant DataStore as DataStore Layer
participant DQ as Publisher:PublisherDataUpdateQueue
participant Manager as Publisher:PublishManager
participant Change as Publisher:ChangeNotifyManager
participant Registry as Publisher:SubscriptionRegistry
participant DUB as Publisher:DataUpdateBuilder
participant DA as DataAccessor Layer
participant Mailbox as Publisher:SubscriberMailbox
participant Subscriber

Note over DataStore,DQ: Layer Boundary (DataStore -> Publisher)
DataStore->>DQ: notifyUpdated(DataDomainSet)
DQ->>Manager: dequeue()

Manager->>Change: judgeNotification()
Manager->>Registry: findAll()
Registry-->>Manager: SubscriptionInfo

loop 各購読者
    Manager->>Change: judgeNotification()
    Change-->>Manager: NotifyTarget

    alt 通知対象の場合
        Manager->>DUB: buildDataUpdate()

        Note over DUB,DA: Layer Boundary (Publisher -> DataAccessor)
        DUB->>DA: getRegistry(DataDomain)

        Note over DA,DataStore: Layer Boundary (DataAccessor -> DataStore)
        DA->>DataStore: capture(RIMSnapshotRequest)
        DataStore-->>DA: RIMSnapshot

        DA-->>DUB: RIMSnapshot
        DUB-->>Manager: DataUpdate
        Manager->>Mailbox: push(DataUpdate)
    end
end

Note over Mailbox,Subscriber: Layer Boundary (Publisher -> Subscriber)
Mailbox->>Subscriber: notify(DataUpdate)
```

## 4.Data周期通知シーケンス

### シーケンス概要
周期通知条件に従ってDataAccessorから最新Dataを取得し、DataUpdateを生成してSubscriberへ通知するシーケンス。

```mermaid
sequenceDiagram
participant Timer
participant Manager as Publisher:PublishManager
participant Periodic as Publisher:PeriodicNotifyManager
participant DUB as Publisher:DataUpdateBuilder
participant DA as DataAccessor Layer
participant DataStore as DataStore Layer
participant Mailbox as Publisher:SubscriberMailbox

Timer->>Manager: periodicTick()
Manager->>Periodic: getDueConditions()
Periodic-->>Manager: DueConditions

loop 通知条件ごと

    Manager->>DUB: buildDataUpdate()

    Note over DUB,DA: Layer Boundary (Publisher -> DataAccessor)
    DUB->>DA: getRegistry(DataDomain)

    Note over DA,DataStore: Layer Boundary (DataAccessor -> DataStore)
    DA->>DataStore: capture(RIMSnapshotRequest)
    DataStore-->>DA: RIMSnapshot

    DA-->>DUB: RIMSnapshot
    DUB-->>Manager: DataUpdate

    Manager->>Mailbox: push(DataUpdate)
    Manager->>Periodic: updateNextTime()
end
```
---