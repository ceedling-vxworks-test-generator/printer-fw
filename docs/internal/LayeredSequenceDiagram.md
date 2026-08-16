# レイヤ構成付き シーケンス図

本ドキュメントは、`products/common/rim_api.cpp` を起点とする実行時フローを、
実際のフォルダ構成(= レイヤ)ごとに色分けした Mermaid シーケンス図としてまとめたものです。

ソースコード (refactor/0808-migration ブランチ時点) を実際に追跡して作成しています。
`docs/internal/architecture.md` / `sequence.md` / `RIManager_SequenceDiagram.md` は
設計方針ドキュメントであり、クラス名が現状の実装と一部食い違っています。
本ドキュメントは実装追従を優先し、両者を突き合わせる際の橋渡しとして使ってください。

---

## レイヤとフォルダの対応

| レイヤ | フォルダ | 主なコンポーネント |
| --- | --- | --- |
| API Layer | `products/common/rim_api.cpp`, `include/rim_api.h` | `RIM_SetXXX` / `RIM_GetXXX` / `RIM_Subscribe` / `RIM_GetNotification` |
| Adapter Layer | `core/adapter` | `AdapterDispatcher` |
| Store Layer | `core/store` | `StoreInputQueue`, `DataStoreWorker`, `RouteProvider`, `DataStoreQueue`, `DataStoreDispatcher` |
| Storage Layer | `core/storage` | `ValueStore`, `RIMSnapshotManager`(`IRIMSnapshotReader`) |
| Capability Layer | `core/capability` | `CapabilityInputQueue`, `CapabilityWorker`, `CapabilityManager`, `CapabilityStore` |
| Publisher Layer | `core/publisher` | `PublisherInputQueue`, `PublisherWorker`, `PublishManager`, `ChangeNotifyManager`, `SubscriptionStore`, `SubscriberMailboxManager`, `CallbackSubscriptionRegistry`, `NotificationReceiver` |
| Accessor Layer | `core/storage/accessor` | `CapabilityAccessor` |

レイヤ間の依存方向は次の通りです(`RoutePipeline` が Store〜Capability を束ねる構成):

```mermaid
flowchart LR
    API[API Layer]
    Adapter[Adapter Layer]
    Store[Store Layer]
    Storage[Storage Layer]
    Capability[Capability Layer]
    Publisher[Publisher Layer]
    Accessor[Accessor Layer]

    API --> Adapter
    Adapter --> Store
    Store --> Storage
    Store --> Capability
    Storage --> Capability
    Capability --> Publisher
    Publisher --> API
    Storage --> Accessor
    Capability --> Accessor
    Accessor --> API
```

---

## 1. Set 系 API 〜 通知配送(Push / Callback)までの全体シーケンス

`RIM_SetDouble()` などの Set 系 API が呼ばれてから、
Callback 購読者に通知が届くまでの一連の流れです。

```mermaid
sequenceDiagram
    autonumber

    box API Layer
    participant Client
    participant RimApi as rim_api.cpp
    end

    box Adapter Layer
    participant Adapter as AdapterDispatcher
    end

    box Store Layer
    participant StoreQ as StoreInputQueue
    participant StoreWorker as DataStoreWorker
    participant RouteQ as DataStoreQueue
    participant StoreDispatcher as DataStoreDispatcher
    end

    box Storage Layer
    participant ValueStore
    participant SnapshotReader as RIMSnapshotManager
    end

    box Capability Layer
    participant CapQ as CapabilityInputQueue
    participant CapWorker as CapabilityWorker
    participant CapManager as CapabilityManager
    participant CapStore as CapabilityStore
    end

    box Publisher Layer
    participant PubQ as PublisherInputQueue
    participant PubWorker as PublisherWorker
    participant PubManager as PublishManager
    participant NotifyManager as ChangeNotifyManager
    participant CallbackRegistry as CallbackSubscriptionRegistry
    end

    Client->>RimApi: RIM_SetDouble(dataId, value, ctx)
    RimApi->>Adapter: Dispatch(DeviceEvent)

    Adapter->>Adapter: FindDataItem() / normalize()
    Adapter->>StoreQ: Push(RIMDataItem)

    StoreQ->>StoreWorker: WaitAndPop()
    StoreWorker->>RouteQ: RouteProvider.find(id) 経由で push(item)

    RouteQ->>StoreDispatcher: Pop(item)
    StoreDispatcher->>ValueStore: Find(oldItem)
    StoreDispatcher->>StoreDispatcher: IChangeChecker.isChanged()

    alt 値が変化した場合
        StoreDispatcher->>ValueStore: ProcessItem() → Store(item)
        StoreDispatcher->>SnapshotReader: Read()
        SnapshotReader-->>StoreDispatcher: RIMSnapshot
        StoreDispatcher->>CapQ: Push(CapabilityInput{snapshot, changedDataId})

        CapQ->>CapWorker: WaitAndPop()
        CapWorker->>PubQ: Push(Data Notification)
        CapWorker->>CapManager: Evaluate(snapshot, changedDataId)
        CapManager->>CapStore: 変化した Capability を更新
        CapManager-->>CapWorker: CapabilityChangeSet
        CapWorker->>PubQ: Push(Capability Notification) ※変化分のみ

        PubQ->>PubWorker: WaitAndPopFor(100ms)
        PubWorker->>PubManager: Publish(target, OnChange)
        PubManager->>NotifyManager: Notify(target, trigger)
        NotifyManager->>CallbackRegistry: 該当購読へ通知
        CallbackRegistry-->>Client: callback(subscriptionId, message)
    else 値が変化しなかった場合
        StoreDispatcher-->>StoreDispatcher: 何もせず次の item を待つ
    end
```

---

## 2. 定期通知(Periodic)シーケンス

`PublisherWorker` はキューを 100ms タイムアウトで待ちつつ、
毎ループ `PublishManager.ProcessPeriodicNotifications()` を呼び出します。

```mermaid
sequenceDiagram
    autonumber

    box Publisher Layer
    participant PubWorker as PublisherWorker
    participant PubManager as PublishManager
    participant PeriodicManager as PeriodicNotifyManager
    participant SubStore as SubscriptionStore
    participant NotifyManager as ChangeNotifyManager
    end

    box API Layer
    participant Client
    end

    loop 100ms ごと
        PubWorker->>PubManager: ProcessPeriodicNotifications()
        PubManager->>PeriodicManager: GetDueConditions()
        PeriodicManager-->>PubManager: due conditions
        loop 期限到来した購読ごと
            PubManager->>SubStore: Find(subscriptionId)
            SubStore-->>PubManager: SubscriptionInfo
            PubManager->>NotifyManager: Notify(target, Periodic)
            NotifyManager-->>Client: Callback / Mailbox 配送
            PubManager->>PeriodicManager: UpdateNextTime(subscriptionId)
        end
    end
```

---

## 3. 状態取得(Pull / Current State)シーケンス

Mailbox による Pull 型通知取得、Capability の現在値取得、Value の直接取得の 3 経路です。

```mermaid
sequenceDiagram
    autonumber

    box API Layer
    participant Client
    participant RimApi as rim_api.cpp
    end

    box Publisher Layer
    participant Receiver as NotificationReceiver
    participant MailboxMgr as SubscriberMailboxManager
    end

    box Accessor Layer
    participant CapAccessor as CapabilityAccessor
    end

    box Capability Layer
    participant CapStore as CapabilityStore
    end

    box Storage Layer
    participant ValueStore
    end

    Note over Client,MailboxMgr: Pull型通知の取得
    Client->>RimApi: RIM_GetNotification(subscriptionId)
    RimApi->>Receiver: TryGetNotification(subscriptionId)
    Receiver->>MailboxMgr: Mailbox から Pop
    MailboxMgr-->>Receiver: NotificationMessage
    Receiver-->>RimApi: message
    RimApi-->>Client: notificationMessage

    Note over Client,CapStore: Capability の現在値取得
    Client->>RimApi: RIM_GetCapability(capabilityId)
    RimApi->>CapAccessor: Get<T>(capabilityId)
    CapAccessor->>CapStore: 現在値を参照
    CapStore-->>CapAccessor: Capability
    CapAccessor-->>RimApi: Capability
    RimApi-->>Client: capability

    Note over Client,ValueStore: DataItem の現在値取得
    Client->>RimApi: RIM_GetDouble/GetBool/GetInt32(dataId)
    RimApi->>ValueStore: Find(dataId)
    ValueStore-->>RimApi: RIMDataItem
    RimApi-->>Client: value
```

---

## 参照した実装ファイル

```text
products/common/rim_api.cpp
core/adapter/src/AdapterDispatcher.cpp
core/store/src/DataStoreWorker.cpp
core/store/src/DataStoreDispatcher.cpp
core/store/src/RouteProvider.cpp
core/common/src/RoutePipeline.cpp (products/common/RoutePipeline.cpp)
core/capability/src/CapabilityWorker.cpp
core/publisher/src/PublisherWorker.cpp
core/publisher/src/PublishManager.cpp
```
