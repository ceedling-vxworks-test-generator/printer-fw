# シーケンス図（Adapter → Callback）

V2で実装済みの E2E フローです。

```mermaid
sequenceDiagram

participant Adapter as PrinterAdapter
participant Dispatcher as AdapterDispatcher

participant StoreQ as StoreInputQueue
participant DSWorker as DataStoreWorker

participant CapQ as CapabilityInputQueue
participant CapWorker as CapabilityWorker

participant PubQ as PublisherInputQueue
participant PubWorker as PublisherWorker

participant PubManager as PublishManager
participant Notify as ChangeNotifyManager

participant Callback as CallbackSubscriptionRegistry

Adapter->>Dispatcher: DeviceEvent

Dispatcher->>StoreQ: Push(RIMDataItem)

DSWorker->>StoreQ: TryPop()

DSWorker->>DSWorker: ValueStore更新

DSWorker->>DSWorker: Snapshot生成

DSWorker->>CapQ: Push(RIMSnapshot)

CapWorker->>CapQ: TryPop()

CapWorker->>CapWorker: Capability生成

CapWorker->>PubQ: Push(PublisherInput)

PubWorker->>PubQ: TryPop()

PubWorker->>PubManager: HasEnvironmentChanged()

PubManager->>Notify: Notify(EnvironmentCapability)

Notify->>Callback: NotifyEnvironment()

Callback-->>Callback: callback()
```

---

# シーケンス図（Mailbox通知）

```mermaid
sequenceDiagram

participant PublishManager
participant ChangeNotifyManager
participant SubscriberMailbox
participant NotificationReceiver
participant RIManager

PublishManager->>ChangeNotifyManager: Notify()

ChangeNotifyManager->>SubscriberMailbox: Push()

RIManager->>NotificationReceiver: TryGetEnvironment()

NotificationReceiver->>SubscriberMailbox: Pop()

SubscriberMailbox-->>NotificationReceiver: EnvironmentCapability

NotificationReceiver-->>RIManager: Capability
```

---

# シーケンス図（Callback通知）

```mermaid
sequenceDiagram

participant PublishManager
participant ChangeNotifyManager
participant CallbackRegistry
participant Subscriber

PublishManager->>ChangeNotifyManager: Notify()

ChangeNotifyManager->>CallbackRegistry: NotifyEnvironment()

CallbackRegistry->>Subscriber: callback(EnvironmentCapability)
```

---

# 将来のスレッド化後の想定図

```mermaid
flowchart LR

    Adapter

    SQ["StoreInputQueue"]
    DT["DataStoreThread"]

    CQ["CapabilityInputQueue"]
    CT["CapabilityThread"]

    PQ["PublisherInputQueue"]
    PT["PublisherThread"]

    Callback

    Adapter --> SQ

    SQ --> DT

    DT --> CQ

    CQ --> CT

    CT --> PQ

    PQ --> PT

    PT --> Callback
```

現在は各 Thread の代わりに

```cpp
ExecuteOnce()
```

を順番に呼び出しており、

```cpp
dataStoreWorker.ExecuteOnce();

capabilityWorker.ExecuteOnce();

publisherWorker.ExecuteOnce();
```

が将来の

```cpp
DataStoreThread
CapabilityThread
PublisherThread
```

へ対応する構造になっています。