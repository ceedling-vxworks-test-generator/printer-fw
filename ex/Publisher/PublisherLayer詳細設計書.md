# Publisher Layer 詳細設計書（L4）

本書は `PublisherLayer仕様設計書.md` を親とし、各コンポーネントの内部データ構造・
インターフェース・排他制御・スレッドモデル・処理アルゴリズムを定義する。

---

## 1. 全体構成とスレッドモデル

```mermaid
flowchart TB
    Cap[Capability Layer]

    subgraph PublisherLayer
        PE[Publish Engine]
        SR[State Repository]
        SB[Subscription Broker]
    end

    Sub[外部購読者]

    Cap -->|notify(capability)| PE
    PE -->|対象解決| SB
    PE -->|配信的差分判定| SR
    PE -->|publish| SB
    SB -->|Push| Sub
    SB -->|最終配信値更新| SR
    Sub -.->|subscribe/unsubscribe| SB
```

### 1.1 スレッドモデル

| 実行主体 | 駆動 | 対象 |
|---------|------|------|
| Capability Layerスレッド | notify呼び出し | Publish Engineの受理（キュー投入まで） |
| Publish Engineタスク（Capabilityごと） | イベント/周期 | トリガ評価・差分判定・配信起動 |
| Subscription Brokerスレッド | subscribe/unsubscribe要求 | 購読者テーブル更新 |

- `notify` は受理して即戻る（Capability Layerを長時間ブロックしない）。
- 配信処理はPublish Engineの各Capabilityタスクで非同期に実行する。
- Rate Limitはタスク単位で適用する。

### 1.2 排他制御

| 資源 | ロック | 保持者 |
|------|-------|--------|
| 購読者テーブル（Subscriber×関心Cap） | Broker内部mutex（読み多め→RWロック可） | Subscription Broker |
| 配信記録（DeliveryRecord） | 購読者×Cap単位mutex | State Repository |
| 受理キュー | キュー内部mutex | Publish Engine |

---

## 2. データモデル

```cpp
using SubscriberId = /* 一意ID */;
enum class CapabilityKind { Error, Job, Env, Maint, Health, Safety, Consumable, Print };

struct Subscriber {
    SubscriberId id;
    // 配信先ハンドル（コールバック/キュー等・実装依存）
};

struct SubscriptionSet {
    FlagSet<CapabilityKind> interested;   // 関心Capability
    RateLimitPolicy         rate;         // 最小配信間隔等（任意）
};

struct DeliveryRecord {
    SubscriberId  subscriber;
    CapabilityKind kind;
    Value         lastDelivered;          // 最終配信値（配信的差分の基準）
    Timestamp     lastDeliveredAt;        // Rate Limit / Periodic判定に使用
};

enum class PublishTrigger { OnChange, Periodic, Threshold, Event, Initial };
```

---

## 3. Publish Engine（②配信管理）

### 3.1 責務
`notify(capability)` を受理し、Capabilityごとのタスクで配信要否を決定して配信を起動する。

### 3.2 内部構造
- Capabilityごとの受理キュー / タスク（タスク数はcfgから決定）。
- Rate Limitポリシ（Capability種別ごと）。

### 3.3 提供IF
```cpp
void notify(const Capability& capability);   // IPublisher実装
```

### 3.4 処理アルゴリズム（Capabilityタスク）
```text
1. 受理キューからCapabilityを取り出す
2. 変更ドメイン/優先度を確認する（Capability.publish優先度, 変更ドメイン）
3. Publishトリガーを決定する（§6）
4. Subscription Brokerへ対象購読者を要求（関心Cap一致）
5. 各対象購読者について State Repository で配信的差分を判定
6. Rate Limit を適用（lastDeliveredAt から最小間隔未満なら送出保留）
7. 配信対象を Subscription Broker.publish() へ渡す
```

- 優先度Critical/Highは他タスクに優先して処理する（優先度は CapabilityPriorityChecker 由来）。

---

## 4. State Repository（①最終配信値・配信的差分）

### 4.1 責務
購読者×Capability単位の最終配信値を保持し、配信的差分を判定する。

### 4.2 内部構造
```cpp
class StateRepository {
    // key = (SubscriberId, CapabilityKind)
    std::unordered_map<Key, DeliveryRecord> records_;
    std::mutex mutex_;   // 実装によりkeyシャーディング可
public:
    bool shouldDeliver(SubscriberId, const Capability&, PublishTrigger);
    void update(SubscriberId, const Capability&);   // 送出成功後に最終配信値更新
};
```

### 4.3 shouldDeliver 判定
| トリガー | 判定 |
|---------|------|
| OnChange | lastDelivered と今回値が異なれば配信 |
| Threshold | Capability Layerが閾値跨ぎと判定済み → 配信（本層は再判定しない） |
| Periodic | 前回配信から周期経過で配信（値不変でも可） |
| Event | 無条件配信（明示イベント） |
| Initial | 記録が無い（初回）→ 配信 |

- **意味的差分（Capが変わったか）はCapability Layerの領域**。本層は購読者ごとの
  「前回配信値との差（配信的差分）」のみを見る。

### 4.4 update
配信成功後にのみ `lastDelivered` / `lastDeliveredAt` を更新する（送出失敗時は更新しない）。

---

## 5. Subscription Broker（③購読管理・配信）

### 5.1 責務
購読者の登録/解除、関心Cap一致による対象解決、実配信（Push）。

### 5.2 内部構造
```cpp
class SubscriptionBroker {
    // 登録順を保持（配信順の安定化）
    std::vector<Subscriber> order_;
    std::unordered_map<SubscriberId, SubscriptionSet> subs_;
    std::shared_mutex mutex_;
public:
    void subscribe(const Subscriber&, const SubscriptionSet&);
    void unsubscribe(SubscriberId);
    std::vector<SubscriberId> resolve(CapabilityKind);   // 関心Cap一致（登録順）
    void publish(SubscriberId, const Capability&);       // Push送出
};
```

### 5.3 配信順序
`order_` により購読者登録順で配信する（決定的順序）。

### 5.4 publish
購読先ハンドルへ非ブロッキングに送出する。送出結果をPublish Engineへ返し、
成功時のみ State Repository.update を行う。

---

## 6. Publishトリガー決定

```mermaid
flowchart TD
    A[notify受理] --> B{明示イベント?}
    B -- Yes --> E[Event配信]
    B -- No --> C{閾値跨ぎ通知?}
    C -- Yes --> T[Threshold配信]
    C -- No --> D{周期到来?}
    D -- Yes --> P[Periodic配信]
    D -- No --> O{配信的差分あり?}
    O -- Yes --> OC[OnChange配信]
    O -- No --> N[配信なし]
```

- Initialは subscribe 時に別途、現在Capを初回配信する経路で扱う。
- Heartbeatは本層に存在しない（外部タスクの責務）。

---

## 7. シーケンス（notify→配信）

```mermaid
sequenceDiagram
    participant Cap as Capability
    participant PE as Publish Engine
    participant SB as Subscription Broker
    participant SR as State Repository
    participant Sub as 購読者

    Cap->>PE: notify(capability)
    Note over PE: Capabilityタスクで非同期処理
    PE->>SB: resolve(kind)
    SB-->>PE: 対象購読者[登録順]
    loop 各購読者
        PE->>SR: shouldDeliver(sub, cap, trigger)
        SR-->>PE: 配信要否
        alt 配信要 かつ Rate Limit OK
            PE->>SB: publish(sub, capability)
            SB->>Sub: Push
            PE->>SR: update(sub, capability)
        end
    end
```

---

## 8. エラー処理

| 事象 | 挙動 |
|------|------|
| 購読者送出失敗 | 当該購読者のみスキップ、State Repository更新せず、次周期で再送機会 |
| 受理キュー溢れ | 優先度低のCapabilityを間引く/最新優先（cfg方針）。ログ出力 |
| 未登録購読者への配信 | resolve結果に含めない（発生しない前提） |

---

## 9. 非機能・設計原則の対応

| 要件 | 担保 |
|------|------|
| notifyでCapabilityを長時間ブロックしない | 受理即戻り + 非同期タスク |
| 高頻度notifyの抑制 | Rate Limit（購読者×Cap単位） |
| 配信順序の安定 | Subscription Brokerの登録順配信 |
| 差分の責務分離 | 配信的差分=本層 / 意味的差分=Capability |
| テスト容易性 | 3コンポーネント独立、購読先はモック可 |

---

## 10. 次段（未確定）

- RateLimitPolicyの具体仕様（最小間隔・バースト許容）
- 受理キュー溢れ時の間引き方針の確定
- Initial配信（subscribe時初回）の現在Cap取得経路（Accessor併用可否）
- 購読先ハンドルの抽象（コールバック / メッセージキュー）の確定
