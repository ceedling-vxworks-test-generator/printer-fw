# SystemSnapshot / SnapshotProvider 詳細設計書（L2）

---

## 1. 目的

本設計は、DataStore Layerにおける状態を一貫した形で外部へ提供するための
データ構造（SystemSnapshot）および生成機構（SnapshotProvider）を定義する。

SystemSnapshotは、各Domain Storeの状態を統合した時点スナップショットとして、
Capability Layerへの入力データとして使用される。

---

## 2. 構成概要

本設計は以下の2要素で構成される：

- SystemSnapshot：状態データ構造
- SnapshotProvider：スナップショット生成機構

---

## 3. SystemSnapshot

---

### 3.1 役割

- 各Domain Storeの状態を統合したデータ表現
- ある時点における「システムの状態」を完全に表現する

---

### 3.2 特性

- Full Snapshot（全Storeを含む）
- Immutable（読み取り専用）
- 一貫状態（整合性を持つ）

---

### 3.3 構成

SystemSnapshotは以下の要素で構成される：

- EnvStoreData
- SafetyStoreData
- ConsumableStoreData
- ErrorStoreData
- JobStoreData
- MaintenanceStoreData
- TimeStoreData

---

### 3.4 データ形式

- 各Storeの内部データを格納する専用構造体を使用する
- Storeクラス自身は含めない

---

### 3.5 使用制約

- Capability LayerはSystemSnapshotのみを参照する
- SystemSnapshotは生成後に変更されない
- DataStoreへの直接アクセスは行わない

---

## 4. SnapshotProvider

---

### 4.1 役割

- 各Domain Storeから状態を収集し、SystemSnapshotを生成する
- SnapshotTriggerからの要求に応じて動作する

---

### 4.2 責務

---

#### 4.2.1 スナップショット生成

- 全Storeの状態を取得し、SystemSnapshotを構築する
- 各Storeからデータをコピーする

---

#### 4.2.2 一貫性確保

- スナップショット生成時に整合性を維持する
- 各Store単位で排他制御されたコピーを行う

---

---

## 5. 非責務

---

### SystemSnapshot

以下の責務を持たない：

- データの更新
- 計算処理
- 状態判定
- ビジネスロジック

---

### SnapshotProvider

以下の責務を持たない：

- トリガ制御
- データ解釈
- Store更新
- Capability制御

---

---

## 6. インターフェース定義

---

### 6.1 SnapshotProvider

提供機能：

- createSnapshot

機能仕様：

- 現在の全Store状態を取得する
- SystemSnapshotを生成する
- 呼び出し元に返却する

---

---

## 7. 処理仕様

---

### 7.1 スナップショット生成

createSnapshotは以下の処理を行う：

- 各Domain Storeから状態データを取得する
- 取得したデータをSystemSnapshotとして統合する
- 完成したスナップショットを返却する

---

---

### 7.2 Storeからのデータ取得

各Storeからのデータ取得は以下のルールに従う：

- Storeが提供するコピー機能を使用する
- コピー処理はStore内部で排他制御される

---

---

### 7.3 整合性

スナップショットは以下の整合性を満たす：

- 各Store内のデータは一貫した状態で取得される
- Store間の厳密な同時性は保証しない（弱整合）

---

---

## 8. 同期制御方針

---

### 8.1 基本方針

- 各Store単位で排他制御を行う
- SnapshotProviderは直接ロック制御を行わない

---

### 8.2 整合性レベル

- 単一Store内：強い整合性
- Store間：弱い整合性

---

---

### 8.3 拡張性

将来的に以下の方式へ変更可能とする：

- 全体ロック方式
- ダブルバッファ方式
- バージョン同期方式

---

---

## 9. エラー処理

---

### 9.1 Store取得失敗

- 原則として発生しない前提とする
- 異常時は呼び出し元へ通知する

---

### 9.2 スナップショット生成失敗

- SnapshotProviderは復旧処理を持たない
- 呼び出し元（Triggerまたは上位）が処理する

---

---

## 10. 非機能要件

---

### 10.1 一貫性

- 各Store内の整合性を保証すること

---

### 10.2 性能

- スナップショット生成は短時間で完了すること
- 余計なコピー処理を行わないこと

---

### 10.3 安定性

- 高頻度呼び出しに耐えられること

---

---

## 11. 設計原則

---

### 原則1

Snapshotは「状態」を表現する

---

### 原則2

Snapshotは変更不可能とする

---

### 原則3

生成責務は一箇所に集約する

---

### 原則4

Storeの内部構造を外部に漏らさない

---

### 原則5

意味や判定は一切持たない

---

---

# 最終定義

---

SystemSnapshotは、

DataStore内の各Store状態を統合した
一貫した時点状態のデータ表現である。

---

SnapshotProviderは、

各Storeから状態を収集し、
SystemSnapshotを生成する唯一のコンポーネントである。

---

# 最重要原則

---

「Snapshotは“状態のコピー”であり、“計算結果”ではない」