# DataStoreDispatcher 詳細設計書（L2）

---

## 1. 目的

本コンポーネントは、DataStoreQueueから取得したデータを
DataIdに基づいて適切なDomain Storeへ反映し、
DataStore Layer内におけるデータ更新の中核処理を担うことを目的とする。

---

## 2. 責務

---

### 2.1 データ取得

- DataStoreQueueからデータを取得する
- データはFIFO順序で処理する

---

### 2.2 Store解決

- 受信したDataIdに基づき更新対象のStoreを決定する
- Store解決はマッピング規則に従う

---

### 2.3 データ更新

- 解決されたStoreに対してデータ更新を行う
- 更新はDataId単位で実行する

---

### 2.4 更新通知

- データ更新後、SnapshotTriggerへ通知する
- 通知は更新事実のみを伝える

---

---

## 3. 非責務

---

本コンポーネントは以下の責務を持たない：

- データ内容の解釈（値の意味）
- 閾値判定
- 状態判定
- Snapshot生成
- トリガ条件判定
- エラーの意味分類

---

👉 本コンポーネントは「更新の伝達」に専念する

---

## 4. データモデル

---

### 4.1 入力データ

DataStoreQueueから取得したデータは以下の情報を持つ：

- DataId
- Value（正規化済み）

---

### 4.2 Store分類

DataIdは以下いずれかのStoreに対応する：

- EnvStore
- SafetyStore
- ConsumableStore
- ErrorStore
- JobStore
- MaintenanceStore
- TimeStore

---

---

## 5. インターフェース定義

---

### 5.1 提供機能

- run：データ処理ループを実行する

---

### 5.2 依存コンポーネント

- DataStoreQueue（入力）
- Domain Store群（更新対象）
- SnapshotTrigger（通知先）
- StoreResolver（DataIdからStoreを解決）

---

---

## 6. 処理仕様

---

### 6.1 メイン処理（run）

runは以下の処理を繰り返し実行する：

- DataStoreQueueからデータを取得する
- DataIdに基づき対象Storeを解決する
- 対象Storeに対してデータ更新を行う
- SnapshotTriggerに対して「更新があった」ことを通知する

---

### 6.2 Store解決

Store解決は以下のルールに従う：

- DataIdごとに対応するStoreが定義されている
- 未定義のDataIdの場合は処理を行わない

---

---

### 6.3 更新処理

更新処理は以下のルールに従う：

- 解決したStoreのupdate機能を呼び出す
- Store内部でDataIdごとの値を更新する

---

---

### 6.4 通知処理

通知処理は以下を行う：

- 更新されたDataIdをSnapshotTriggerに通知する
- 通知は事実のみ（更新があった）を伝える

---

---

## 7. エラー処理

---

### 7.1 未定義DataId

- 対応するStoreが存在しない場合、更新を行わない
- 必要に応じてログ出力する

---

### 7.2 Store更新失敗

- Store側で処理される
- Dispatcherはリカバリを行わない

---

---

## 8. 非機能要件

---

### 8.1 順序保証

- Queueから取得した順序で処理されること

---

### 8.2 スループット

- 高頻度入力に対して遅延なく処理できること

---

### 8.3 スレッド安全性

- 単一実行コンテキストで動作することを前提とする
- Store側で排他制御を行う

---

---

## 9. 設計原則

---

### 原則1

Dispatcherは「判断しない」

---

### 原則2

DataIdに基づいてのみ処理を行う

---

### 原則3

データの意味に関与しない

---

### 原則4

処理の流れを単純に保つ

---

### 原則5

他コンポーネントの責務を侵害しない

---

---

# 最終定義

---

DataStoreDispatcherは、

入力データを適切なStoreへ振り分け、
更新処理を実行し、その事実をTriggerへ通知する
データ更新中核コンポーネントである。

---

# 最重要原則

---

「Dispatcherは流すだけであり、判断しない」