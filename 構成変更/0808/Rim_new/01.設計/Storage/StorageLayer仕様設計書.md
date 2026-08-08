# StorageLayer 仕様設計書

## 1. 概要

### 1.1 目的

Storage Layerは、システム内で利用される最新Dataおよび最新RIMCapabilityを保持し、利用者へ一貫した情報取得機能を提供するコンポーネントである。

StorageLayerは最新Data、最新RIMCapability、最新Facadeおよび
RIMSnapshotを保持し、利用者へ一貫した情報取得機能を提供する。

StorageLayerは情報保持、RIMSnapshotの生成・保存、
およびStorageLayerAPIによる情報提供を担当する。

### 1.2 対象範囲

- ErrorStorage保持
- GeneralStorage保持
- ValueStorage保持
- CapabilityStorage保持
- FacadeStorage保持
- RIMSnapshot生成
- RIMSnapshot保持
- StorageLayerAPI提供
- StorageLayerの構成および責務定義

## 2. システム構成上の位置づけ

### 2.1 システム全体構成

```text
Adapter
    ↓
DataStore Layer
    ↓
Capability Layer
    ↓
Publisher Layer

            +-------------------+
            |        StorageLayer        |
            |-------------------|
            | StorageLayerAPI            |
            | ErrorStorage        |
            | GeneralStorage      |
            | ValueStorage        |
            | CapabilityStorage   |
            | SnapshotGenerator |
            +-------------------+
```

### 2.2 StorageLayerの責務

- ErrorStorage保持
- GeneralStorage保持
- ValueStorage保持
- CapabilityStorage保持
- FacadeStorage保持
- RIMSnapshot生成
- RIMSnapshot保持
- 利用者向け情報提供

### 2.3 StorageLayerの非責務

- Data受理
- Queue管理
- Dispatcher制御
- 更新通知
- RIMCapability生成
- RIMCapability差分判定
- Publisher通知
- Subscription管理

## 3. 保持情報設計

### 3.1 ErrorStorage

#### 責務

現在発生中の異常情報を保持する。

#### 保持方針

- 現在発生中の異常のみ保持する
- 異常履歴は保持しない
- ErrorStateに応じて状態を更新する
- 状態変化を検知する

#### 提供IF

```cpp
DataDomainSet apply(const RIMDataItem& item);
ErrorRIMSnapshot makeSnapshot(DataDomain domain);
```

#### 動作

- Raised : 新規異常登録
- Cleared : 異常解除
- AllCleared : 全異常解除
- UpdatedHeal : 回復状態へ更新
- UpdatedActive : 発生状態へ更新

#### 排他

ドメイン単位で排他制御を行う。

### 3.2 GeneralStorage

#### 責務

General系情報を保持する。

#### 保持方針

- 現在状態のみ保持する
- 状態変化を検知する
- RIMDataIdに応じて格納先を決定する

#### 提供IF

```cpp
DataDomainSet apply(const RIMDataItem& item);
OperationRIMSnapshot makeSnapshot(DataDomain domain);
```

#### 動作

RIMDataIdに応じて適切な保存先へGeneral情報を反映する。

#### 排他

ドメイン単位で排他制御を行う。

### 3.3 ValueStorage

#### 責務

最新値系データの現在値を保持する。

#### 保持方針

- 最新値のみ保持する
- 中間更新履歴は保持しない
- RIMDataIdに応じて格納先を決定する
- 状態変化を検知する

ValueStorageは保存責務のみを持ち、呼び出し元コンポーネントへ依存しない。

#### 提供IF

```cpp
DataDomainSet apply(const RIMDataItem& item);
ValueRIMSnapshot makeSnapshot(DataDomain domain);
```

#### 動作

RIMDataIdに応じて保存先へ最新値を反映する。

#### 排他

ドメイン単位で排他制御を行う。

### 3.4 CapabilityStorage

#### 責務

最新RIMCapabilityを保持する。

#### 保持方針

- RIMCapabilityCategory単位で保持する
- 最新RIMCapabilityを保持する
- RIMCapabilityの正本として振る舞う

#### 提供IF

```cpp
updateCapability(category, capability);
getCapability(category);
```

#### 排他

Capabilityの更新および参照の整合性を保証する。

Capabilityの正本はCapabilityStorageとする。


### 3.5 FacadeStorage
#### 責務

最新Facadeを保持する。

#### 保持方針

- Facade単位で保持する
- 最新Facadeを保持する
- Facadeの正本として振る舞う

#### 提供IF

updateFacade(...)
getFacade(...)

#### 排他

Facadeの更新および参照の整合性を保証する。

## 4. Snapshot生成設計

### 4.1 Snapshot生成方針

StorageLayerはStorageに保持されているDataを利用してRIMSnapshotを生成する。

### 4.2 Snapshot整合性保証

StorageLayerは要求されたDataDomain集合について単一時点整合性を保証する。

### 4.3 Snapshot保持方針

StorageLayerはRIMSnapshotの生成および保持を担当する。

生成されたRIMSnapshotはStorageLayer内で保存される。
利用者はStorageLayerAPIを介してRIMSnapshotを取得する。

## 5. StorageLayerAPI設計

### 5.1 StorageLayerAPIの責務

- Capability取得
- Facade取得
- Data取得
- Snapshot取得

### 5.2 Capability取得

入力：RIMCapabilityCategory

出力：RIMCapability

### 5.3 Facade取得
入力：FacadeId
出力：Facade

StorageLayerAPIはFacadeStorageに保持されている
Facadeを返却する。

### 5.4 Data取得

入力：DataDomain

出力：Data

Dataとは指定されたDataDomainに対応する現在保持中のデータを表す。

Dataの具体的なデータ構造はDataDomainごとに定義される。

StorageLayerAPIはStorageを直接公開せず、保持中のDataのみを返却する。

### 5.5 Snapshot取得

入力：DataDomainSet
出力：RIMSnapshot

提供IF：
```
RIMSnapshot capture(
     const RIMSnapshotRequest& request);
```
SnapshotGeneratorは指定された複数DataDomainを集約し、
単一時点整合性を持つRIMSnapshotを生成する。

### 5.6 Storage更新IF

```text
apply(item)
    ↓
変更DataDomain返却
```

Storageは変更DataDomainを返却する。

更新通知はDataStorage Layerの責務とする。

### 5.7 Capability Layer連携IF

CapabilityStorageを更新可能なコンポーネントはCapability Layerのみとする。

## 6. コンポーネント構成

```text
Storage Layer
├─ StorageLayerAPI
├─ ErrorStorage
├─ GeneralStorage
├─ ValueStorage
├─ CapabilityStorage
└─ SnapshotGenerator
```

## 7. 差分検知および通知設計

### 7.1 基本方針

差分検知はStorageの責務とする。

更新通知はDataStorage Layerの責務とする。

### 7.2 Store責務

- Data保持
- Data更新
- 差分検知
- 変更DataDomain判定

Storageは変更が発生したDataDomainを返却する。

Storageは更新通知を行わない。

## 8. 排他・整合性設計

各Storageは自身の排他を管理する。

## 9. 設計方針

### Single Source of Truth

最新Dataの正本はStorage Layerのみが管理する。

最新Capabilityの正本はStorage Layerのみが管理する。

最新Facadeの正本はStorage Layerのみが管理する。

## 10. 制約事項

- StorageLayerはCapability生成を行わない
- StorageLayerはPublisher通知を行わない
- 情報取得はStorageLayerAPI経由で行う

## 11. 非機能要件

- 情報整合性を保証できること
- Snapshot単一時点整合性を保証できること
- 並列アクセスに対して安全であること
- コンポーネント単位でテスト可能であること
