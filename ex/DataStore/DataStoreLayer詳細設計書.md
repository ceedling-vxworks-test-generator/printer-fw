# DataStore Layer 詳細設計書（L2）

本書は `DataStoreLayer仕様設計書.md`（③基本設計・2026-07-07）を親とし、
各コンポーネントの内部データ構造・インターフェース・排他制御・スレッドモデル・
処理アルゴリズムを定義する。用語・構成は基本設計に準拠する。

> 旧②世代（単一Queue / 7 Store / SnapshotProvider・SnapshotTrigger）は廃止済み
> （`ex/_deprecated_旧世代/`）。本書は3レーン + MachineRegistry + Pull型Snapshot構成を詳細化する。

---

## 1. 全体構成とスレッドモデル

### 1.1 コンポーネント全体

```mermaid
flowchart TB
    Adapter[Adapter Layer]

    subgraph DataStoreLayer
        CIP[CentralInputPort]

        FQ[FaultInputQueue]
        OQ[OperationReportQueue]
        CB[CurrentValueBuffer]

        FD[FaultDispatcher]
        OD[OperationDispatcher]
        CD[CurrentValueDispatcher]

        FR[FaultRegistry]
        OR[OperationRegistry]
        CR[CurrentValueRegistry]

        MR[MachineRegistry]
        MSR[MachineSnapshotReader]
    end

    Cap[Capability Layer]

    Adapter -->|postFaultInput| CIP
    Adapter -->|postOperationReport| CIP
    Adapter -->|postCurrentValueInput| CIP

    CIP --> FQ
    CIP --> OQ
    CIP --> CB

    FQ -->|notify| FD
    OQ -->|notify| OD
    CB -->|周期pull| CD

    FD --> FR
    OD --> OR
    CD --> CR

    FR --> MR
    OR --> MR
    CR --> MR

    FD -. notifyUpdated .-> Cap
    OD -. notifyUpdated .-> Cap
    CD -. notifyUpdated .-> Cap

    Cap -->|capture| MSR
    MSR --> MR
```

### 1.2 スレッドモデル

| 実行主体 | 駆動 | 対象 |
|---------|------|------|
| Adapter呼び出しスレッド | 同期 | CentralInputPort.post*（Queue/Bufferへの投入まで） |
| FaultDispatcherスレッド | イベント駆動（Queue通知でwake） | FaultInputQueue → FaultRegistry |
| OperationDispatcherスレッド | イベント駆動（Queue通知でwake） | OperationReportQueue → OperationRegistry |
| CurrentValueDispatcherスレッド | 周期駆動（タイマ） | CurrentValueBuffer → CurrentValueRegistry |
| Capability Layerスレッド | notifyUpdated契機 | MachineSnapshotReader.capture |

**設計原則**：投入（post）は呼び出し側スレッドで即時完了し、Registry反映は各Dispatcher
スレッドへ非同期に移す。これによりAdapter/入力元はDataStore内部の反映時間に依存しない。

### 1.3 排他制御の配置

| 資源 | ロック | 保持者 |
|------|-------|--------|
| FaultInputQueue / OperationReportQueue | Queue内部mutex | Queue |
| CurrentValueBuffer（slot群 + dirty） | Buffer内部mutex | CurrentValueBuffer |
| 各Registry | ドメイン単位mutex | 各Registry |

- Dispatcherは自前のロックを持たず、Queue/Buffer/Registryのロックに委ねる。
- MachineSnapshotReaderは各Registryの `makeSnapshot()` を通してのみロックに触れる（短時間コピー）。
- 複数Registryにまたがる同時ロックは取得しない（デッドロック回避／Store間は弱整合）。

---

## 2. 共通データモデル

### 2.1 DataEntryItem

```cpp
struct DataEntryItem {
    DataId       id;       // データ種別識別子（保存先特定に使用）
    DataValue    value;    // 正規化済み値（Adapterが変換済み）
    DataContext  context;  // 補足情報（key-valueの集合）
};
```

- `DataValue` は種別ごとに定めた型のバリアント（例：温度=Celsius型）。
- `DataContext` は `key -> 値` の集合。利用可能keyは種別ごとに定義する。

### 2.2 DataContext の代表key

| key | 対象 | 値 |
|-----|------|----|
| FaultState | 異常系 | Raised / Cleared / AllCleared / UpdatedHeal / UpdatedActive |
| （拡張） | — | 種別ごとに追加定義 |

### 2.3 RegistryDomain / RegistryDomainSet

```cpp
enum class RegistryDomain { /* Fault系, Operation系, CurrentValue系のドメイン */ };
using RegistryDomainSet = FlagSet<RegistryDomain>;   // 複数ドメインの集合
```

- 更新通知（notifyUpdated）とSnapshot要求（SnapshotRequest）の両方で使用する。

### 2.4 Snapshotデータ型

```cpp
struct FaultSnapshot     { /* 発生中Faultの読取専用ビュー */ };
struct OperationSnapshot { /* 現在の動作報告の読取専用ビュー */ };
struct CurrentValueSnapshot {
    std::optional<EnvironmentSnapshot> environment;
    // 他CurrentValue系ドメインを追加
};

struct MachineSnapshot {
    std::optional<FaultSnapshot>        fault;
    std::optional<OperationSnapshot>    operation;
    std::optional<CurrentValueSnapshot> currentValue;
};
```

**Snapshotの保証**：読み取り専用・生成後不変・単一時点整合（同一Registry内）。

---

## 3. CentralInputPort

### 3.1 責務
Adapterからの3系統の投入を受け付け、post種別とIdの分類の一致を検証し、
対応するQueue/Bufferへ振り分ける。

### 3.2 内部データ
- 3つの投入先への参照（FaultInputQueue / OperationReportQueue / CurrentValueBuffer）
- Id→分類の対応表（IdClassifier）。検証に使用。

### 3.3 提供IF

```cpp
bool postFaultInput(const DataEntryItem& item);
bool postOperationReport(const DataEntryItem& item);
bool postCurrentValueInput(const DataEntryItem& item);
```

### 3.4 処理（各post共通）
1. `item.id` の分類を IdClassifier で取得する。
2. 呼び出されたpost種別と分類が一致するか検証する。
   - 不一致（例：postFaultInputに動作報告Idが来た）→ `false` を返し投入しない（設計不備）。
3. `item.value` の型が `item.id` の定義型と一致するか検証する。
4. 問題なければ対応するQueue/Bufferへ投入する。
   - Fault/Operation：`enqueue(item)`（Queueが受理時にDispatcherへ通知）
   - CurrentValue：`buffer.post(item)`（通知は行わない）
5. 投入成否を `bool` で返す。

### 3.5 非責務
- 値の意味解釈・状態判定は行わない（分類/型の整合検証のみ）。

---

## 4. 入力保持コンポーネント

### 4.1 FaultInputQueue / OperationReportQueue

```cpp
using FaultInputQueue     = IQueue<DataEntryItem>;
using OperationReportQueue = IQueue<DataEntryItem>;
```

| 項目 | FaultInputQueue | OperationReportQueue |
|------|-----------------|----------------------|
| 順序 | FIFO | FIFO（順序保証・欠落不可） |
| 優先 | 高優先で処理 | 通常 |
| 受理時通知 | FaultDispatcherへnotify | OperationDispatcherへnotify |
| 排他 | 内部mutex | 内部mutex |

- 提供IF：`enqueue(item)` / `dequeue()`（一般的なthread-safe FIFO）。
- `enqueue` 成功時に対応Dispatcherの待機を解除（condition variable等）。

### 4.2 CurrentValueBuffer

最新値系を **Idごとに上書き保持** するバッファ。周期取り出し方式。

```cpp
struct Slot {
    DataEntryItem data;
    bool          dirty = false;
};

class CurrentValueBuffer {
    std::unordered_map<DataId, Slot> slots_;   // ID分のslot
    std::mutex mutex_;
public:
    bool post(const DataEntryItem& item);                  // 上書き + dirty=true
    std::vector<DataEntryItem> takeUpdatedValues();        // dirtyのみ取り出し + clear
};
```

- `post`：`slots_[id].data = item; slots_[id].dirty = true;`（mutex内）。
  同一Idが周期内に複数回来ても最新1件のみ保持。
- `takeUpdatedValues`：dirtyなslotのdataを収集し、**同一ロック内でdirtyをクリア**して返す。
  周期内に同一Idが複数更新されても取り出しは最新1件。
- 受理（post）時に **Dispatcherへ通知しない**（周期pullで吸い上げる）。

---

## 5. Dispatcher群

### 5.1 共通事項
- 更新を該当Registryへ `apply()` で反映する。
- 反映により状態変化が生じたドメインを収集し、`IRegistryUpdateNotifier.notifyUpdated(domains)`
  でCapability Layerへ通知する（**変更契機のみ通知。データは渡さない**）。
- 自前ロックは持たない。

### 5.2 FaultDispatcher / OperationDispatcher（イベント駆動）

```cpp
void dispatch() {   // Dispatcher専用スレッドのループ
    for (;;) {
        DataEntryItem item = queue.dequeue();   // 空ならブロック
        RegistryDomain d = registry.apply(item);
        if (changed(d)) notifier.notifyUpdated({ d });
    }
}
```

- FaultDispatcherは高優先で処理する。
- 状態変化がなければ通知しない（`alt 状態変化あり`）。

### 5.3 CurrentValueDispatcher（周期駆動）

```cpp
void dispatch() {   // タイマ周期で起動
    auto updated = buffer.takeUpdatedValues();
    RegistryDomainSet changed;
    for (auto& item : updated) {
        RegistryDomain d = registry.apply(item);
        if (isChanged(d)) changed.add(d);
    }
    if (!changed.empty()) notifier.notifyUpdated(changed);
}
```

- 高頻度なCurrentValue更新を周期でまとめて反映し、Snapshot生成頻度を抑制する。

---

## 6. Registry群

### 6.1 共通事項
- ドメイン単位mutexで排他する。
- `apply()` で反映、`makeSnapshot()` で読取専用スナップショットを返す。
- State（意味）は保持しない。観測事実のみ。

### 6.2 FaultRegistry

現在発生中の異常系を保持する。`DataContext.FaultState` に応じ挙動を分岐する。

```cpp
class FaultRegistry {
    mutable std::mutex mutex_;
    /* 発生中Faultの集合（Id -> 状態） */
public:
    RegistryDomain apply(const DataEntryItem& item);
    FaultSnapshot  makeSnapshot() const;
};
```

`apply` の FaultState 別挙動：

| FaultState | 挙動 |
|-----------|------|
| Raised | 新規エラーを登録する |
| Cleared | 該当エラーをクリアする |
| AllCleared | 全エラーをクリアする |
| UpdatedHeal | 登録済みエラーの状態を「回復」に更新する |
| UpdatedActive | 登録済みエラーの状態を「発生」に更新する |

- 変化があった場合、対応する `RegistryDomain` を返す（Dispatcherが通知判定に使用）。

### 6.3 OperationRegistry

現在の動作報告を保持する。

```cpp
class OperationRegistry {
    mutable std::mutex mutex_;
public:
    RegistryDomain    apply(const DataEntryItem& item);   // Idに応じ動作報告を反映
    OperationSnapshot makeSnapshot() const;
};
```

- 順序保証された入力（OperationReportQueueがFIFO）を前提に反映する。

### 6.4 CurrentValueRegistry

最新値系の現在値を保持する。

```cpp
class CurrentValueRegistry {
    mutable std::mutex mutex_;
public:
    RegistryDomain      apply(const DataEntryItem& item); // Idに応じ最新値を反映
    CurrentValueSnapshot makeSnapshot() const;
};
```

### 6.5 MachineRegistry（ファサード）

FaultRegistry / OperationRegistry / CurrentValueRegistry を集約する。

```cpp
class MachineRegistry {
    FaultRegistry&        fault_;
    OperationRegistry&    operation_;
    CurrentValueRegistry& currentValue_;
public:
    // domain指定に応じ各RegistryのmakeSnapshotを呼び出し集約
    MachineSnapshot makeSnapshot(const RegistryDomainSet& domains) const;
};
```

- 外部（MachineSnapshotReader）はMachineRegistryを介してのみ各Registryへアクセスする。
- Registry全体の生参照は提供しない。

---

## 7. MachineSnapshotReader

Capability Layerに対し `IMachineSnapshotReader` を提供する。

```cpp
struct SnapshotRequest { RegistryDomainSet domains; };

class MachineSnapshotReader /* : IMachineSnapshotReader */ {
    const MachineRegistry& registry_;
public:
    MachineSnapshot capture(const SnapshotRequest& request) const;
};
```

### 7.1 capture 処理
1. `request.domains` に含まれるドメインについてのみ、対応Registryの `makeSnapshot()` を呼ぶ。
2. 各 `makeSnapshot()` はドメイン単位mutexを**短時間**保持してコピーを作る。
3. 取得できたものを `MachineSnapshot`（optional）に詰めて返す。

### 7.2 整合性
- 各Registry内：強整合（単一時点コピー）。
- Registry間：弱整合（同時刻性は保証しない）。Capability評価は各ドメインの整合スナップショットで行う。

### 7.3 制約
- Capability LayerはDataStore処理スレッドから独立して実行される前提。
- captureは書き込みと競合しても、makeSnapshotのコピー中のみ短時間ロックするため入力を長時間阻害しない。

---

## 8. 更新通知 IRegistryUpdateNotifier

```cpp
struct IRegistryUpdateNotifier {
    virtual void notifyUpdated(const RegistryDomainSet& domains) = 0;
};
```

- **Capability Layerが実装**し、DataStore Layer（各Dispatcher）が呼び出す。
- 通知は「どのドメインが変化したか」の契機のみ。実データはcaptureで取りに来る（Pull型）。
- notifyUpdatedは軽量・非ブロッキングであること（Dispatcherを長時間止めない）。

---

## 9. シーケンス

### 9.1 FaultInput（イベント駆動）

```mermaid
sequenceDiagram
    participant Adapter
    participant Port as CentralInputPort
    participant Q as FaultInputQueue
    participant D as FaultDispatcher
    participant R as FaultRegistry
    participant Cap as Capability

    Adapter->>Port: postFaultInput(item)
    Port->>Port: 分類/型 検証
    Port->>Q: enqueue(item)
    Q-->>D: notify(wake)
    D->>Q: dequeue()
    D->>R: apply(item)  %% FaultStateで分岐
    alt 状態変化あり
        D-->>Cap: notifyUpdated(domains)
    end
```

### 9.2 CurrentValue（周期駆動）

```mermaid
sequenceDiagram
    participant Adapter
    participant Port as CentralInputPort
    participant B as CurrentValueBuffer
    participant D as CurrentValueDispatcher
    participant R as CurrentValueRegistry
    participant Cap as Capability

    Adapter->>Port: postCurrentValueInput(item)
    Port->>B: post(item)  %% 上書き + dirty
    loop 周期
        D->>B: takeUpdatedValues()
        B-->>D: updated[]（dirtyのみ・最新1件）
        D->>R: apply(item) x N
    end
    alt 状態変化あり
        D-->>Cap: notifyUpdated(changed)
    end
```

### 9.3 Snapshot取得（Pull）

```mermaid
sequenceDiagram
    participant Cap as Capability
    participant Reader as MachineSnapshotReader
    participant MR as MachineRegistry
    participant Reg as 各Registry

    Cap->>Reader: capture(request)
    Reader->>MR: makeSnapshot(domains)
    loop 要求ドメイン
        MR->>Reg: makeSnapshot()  %% 短時間ロック・コピー
        Reg-->>MR: XxxSnapshot
    end
    MR-->>Reader: MachineSnapshot
    Reader-->>Cap: MachineSnapshot
```

---

## 10. エラー処理

| 事象 | 発生元 | 挙動 |
|------|--------|------|
| 分類/型不一致 | CentralInputPort | `false` 返却・投入せず・ログ（設計不備） |
| Queue容量超過 | Queue | `enqueue` 失敗を返す・データ破棄しないのは呼び出し側責務 |
| 未定義Id | Registry.apply | 反映せず・ログ |
| makeSnapshot中の異常 | Registry/Reader | 呼び出し元へ通知・Readerはリカバリしない |

- Dispatcherは反映失敗時にリカバリを行わず、影響をDataStore Layer内に限定する。

---

## 11. 非機能・設計原則の対応

| 要件（基本設計 §11） | 本詳細での担保 |
|---------------------|---------------|
| 単一時点整合性 | Registry単位のmakeSnapshotコピー |
| CurrentValue高頻度に対するSnapshot抑制 | Buffer上書き + Dispatcher周期反映 |
| Capability処理時間に非依存 | post即時完了 + 非同期Dispatcher + Pull型capture |
| 差分監視 | apply戻り値のドメイン + notifyUpdated |
| 責務分離 | 入力=Port/Queue、反映=Dispatcher/Registry、参照=Reader |
| テスト容易性 | 各コンポーネントを独立IFで分離、Notifierはモック可能 |

---

## 12. 次段（未確定・L2で詰める項目）

- `DataValue` / 各ドメインの具体スキーマ（Env等）の型定義
- RegistryDomainの正式な列挙とId→Domain対応表
- Queue容量・CurrentValueDispatcher周期・Fault優先度の具体値（cfg化）
- notifyUpdatedの実装形態（直接呼び出し / イベントキュー）の確定
