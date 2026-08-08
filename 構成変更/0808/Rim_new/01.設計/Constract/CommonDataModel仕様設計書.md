# 共通データモデル設計書

---

## 1. 概要

本設計書は、システム全体で利用する共通データモデルおよびレイヤ間契約を定義する。

対象:
- RIMDataId
- RIMDataDefinition
- ValueType
- RIMContext
- DataDomain

RIMAdapter Layer、RIMDataStore Layer、RIMCapability Layerは本設計書で定義する共通契約を利用する。

---

## 2. 背景

Data定義を各Layerに分散すると、Data追加時の変更箇所増加、型不一致、Domain不整合が発生する。
そのためDataに関する契約を集約する。

---

## 3. データモデル

### 3.1 RIMDataId

Data種別を識別する論理識別子。  
RIMDataIdはシステム全体で一意でなければならない。

例:
- Temperature
- Pressure
- CurrentJob
- CurrentError

---

### 3.2 DataDomain

RIMDataStore Layerにおけるデータ管理単位。

DataDomainはRIMDataStore Layer内部の保存先分類であると同時に、
RIMDataStore LayerとRIMCapability Layer間で共有される依存関係管理単位である。

DataDomainは以下の契約で利用される。

- RIMDataStore LayerからRIMCapability Layerへの更新通知
- RIMCapabilityRuleの依存ドメイン定義
- RIMSnapshot取得範囲指定

RIMCapability LayerはDataDomainを利用して必要なRIMSnapshot取得範囲を決定する。


現時点では1つのRIMDataIdは1つのDataDomainにのみ所属する。

複数DataDomainへの所属はサポートしない。


用途:
- 保存先判定
- 更新通知
- RIMSnapshot取得対象
- RIMCapabilityRule依存関係管理

例:
- EnvironmentDomain
- OperationDomain
- ErrorDomain
- DriveDomain

DataDomainはシステムの機能単位で定義する。

外付け機器、オプション機器および将来ユニットが追加された場合も、
機器種別ではなく提供する機能に応じたDataDomainへ格納する。

そのためDataDomainは機器構成とは独立した論理的な機能分類である。

---

### 3.3 ValueType

RIMDataIdに対応する具体型を表す。  
ValueTypeはシステムが扱うData値型を表す識別情報である。  
具体的な実装方法は各実装系に委ねる。  

例:
- double
- bool
- JobInfo
- ErrorInfo

許容型:
- スカラー型
- Enum型
- 配列型
- 構造体型

ストリーム型は対象外とする。

---

### 3.4 RIMContext

Data本体に付随する補助情報。

利用用途:
- RIMAdapter Layerでの正規化補助
- RIMDataStore Layerへの制御情報

制約:
- RIMContextは主データ保持用途に利用してはならない
- RIMCapability判定に必要な主要情報はValueに保持する
- RIMDataDefinitionで管理される情報をRIMContextへ重複保持してはならない。
  - 以下はRIMContextへ保持しない。
    - RIMDataId
    - DataDomain
    - ValueType


RIMContextはKey-Value形式で管理する。  
RIMContextKeyは必要に応じて追加可能とする。  
RIMContextValueは複数型を保持可能とする。  

例:
- Unit
- SensorType
- ScaleFactor
- Operation

---

### 3.5 RIMDataDefinition

RIMDataIdに関する共通定義。

RIMDataDefinitionはRIMDataIdに関する唯一の正本である。

RIMDataId追加時は必ずRIMDataDefinitionへ定義を追加する。

各LayerはRIMDataIdに関する固有情報を独自管理してはならない。


保持項目:
- RIMDataId
- ValueType
- DataDomain

```cpp
struct RIMDataDefinition
{
    RIMDataId id;
    ValueType valueType;
    DataDomain domain;
};
```

例:

```text
Temperature
 ├ ValueType      : double
 └ DataDomain : EnvironmentDomain
```

```text
CurrentError
 ├ ValueType      : ErrorInfo
 └ DataDomain : ErrorDomain
```

```text
CurrentJob
 ├ ValueType      : JobInfo
 └ DataDomain : OperationDomain
```

---

### 3.6 RIMDataDefinition利用規約

RIMDataDefinitionはシステム全体で利用するData定義の正本とする。

Dataに関する以下の情報はRIMDataDefinitionに集約する。

- RIMDataId
- ValueType
- DataDomain

Data追加時はRIMDataDefinitionを更新する。

各LayerはRIMDataDefinitionを参照して処理を行う。

RIMDataStore LayerやRIMCapability Layerは
独自のRIMDataId→DataDomain対応表を保持してはならない。

RIMDataIdに関する情報が必要な場合はRIMDataDefinitionを参照する。


RIMDataDefinitionに定義されていないRIMDataIdはシステムで利用してはならない。

---

#### 3.7 RIMDataItem

RIMDataItemはRIMAdapter LayerとDataStore Layer間、
およびRIMDataStore Layer内部で利用する標準データモデルである。

保持項目:

- RIMDataId
- Value
- RIMContext

```cpp
struct RIMDataItem
{
    RIMDataId id;
    Value value;
    RIMContext context;
};
``` 
RIMDataItemはシステム内におけるData契約の正本とする。　　
RIMAdapter LayerおよびRIMDataStore LayerはRIMDataItemを利用してデータを受け渡す。　　
RIMCapability LayerはRIMDataItemを利用せず、RIMSnapshotを利用する。

---

#### 3.8 RIMRIMSnapshotRequest

RIMSnapshotRequestはRIMSnapshot取得要求を表す。

保持項目:

- DataDomainSet

```cpp
struct RIMRIMSnapshotRequest
{
    DataDomainSet domains;
};
```

RIMCapability LayerはRIMRIMSnapshotRequestを利用して
取得対象となるDataDomainを指定する。

---

#### 3.9 RIMSnapshot

RIMSnapshotはある時点のRegistry状態を
読み取り専用で表現したデータである。

RIMSnapshotは以下を保証する。

- 読み取り専用
- 生成後不変
- 単一時点整合性

RIMSnapshotの具体的な構造は
システムごとのRegistry構成に依存する。

RIMCapability LayerはRIMSnapshotのみを利用して
RIMCapabilityを生成する。

---

## 4. 利用方針

### Adapter Layer

RIMAdapter LayerはRIMDataDefinitionを参照して
変換対象となるData定義を取得する。

Ruleは外部入力をRIMDataDefinitionで定義された
ValueTypeへ正規化する。

RIMAdapter LayerはRIMDataItemを生成し、
RIMDataStore Layerへ受け渡す。

---

### DataStore Layer

RIMDataStore LayerはRIMDataDefinitionのDataDomainを参照して保存先を決定する。

Registry実装はRIMDataDefinitionを正本として利用する。

RIMDataStore Layer内に独自のRIMDataId→DataDomain変換表を持たない。

RIMDataId追加時の修正は原則としてRIMDataDefinitionへ集約する。

RIMDataStore LayerはRIMDataItemを受理する。

RIMDataStore LayerはRIMDataItemに含まれるRIMDataIdから対応するRIMDataDefinitionを参照し、DataDomainを決定する。

---

### RIMCapability Layer
RIMCapability LayerはDataDomainを利用してRIMSnapshot取得範囲を決定する。

RIMCapability LayerはRIMRIMSnapshotRequestを利用して必要なRIMSnapshot取得を要求する。

RIMCapability LayerはRIMSnapshotを利用してRIMCapabilityを生成する。

---

## 5. 拡張方針

---

### Data追加

新DataはRIMDataDefinition追加によって拡張する。

RIMDataDefinition追加によりData種別をシステムへ登録できる。

ただし、当該Dataを利用するAdapter Rule、
Registry処理、RIMCapabilityRule等が必要となる場合は、
それぞれのLayerで追加実装を行う。


新たなData追加時は既存RIMDataItem構造を変更しないことを原則とする。

Dataの拡張はRIMDataDefinition追加によって行う。

---

### DataDomain追加

必要に応じてDataDomainを追加可能とする。

---

### ValueType追加

新たなValueTypeを追加可能とする。

---

## 6. 設計方針

### 6.1 Single Source of Truth

RIMDataIdに関する定義の正本はRIMDataDefinitionとする。

---

### 6.2 変更影響局所化

Data追加時の変更をRIMDataDefinitionへ集約する。

---

### 6.3 Layer共通契約

下記をLayer間共通契約として扱う。
- RIMDataDefinition
- RIMDataItem
- RIMRIMSnapshotRequest
- RIMSnapshot
- RIMContext
- DataDomain

---

### 6.4 RIMDataDefinition Centralization

RIMDataIdに関する知識はRIMDataDefinitionへ集約する。

各LayerはRIMDataDefinitionを参照することで
RIMDataId追加時の変更影響を最小化する。

新Data追加時はRIMDataDefinitionの更新を起点とする。

---

### 6.5 データ受け渡し方針

システム全体として不要なデータコピーを避けるため、
読み取り専用データは参照による受け渡しを基本とする。

ただし、ライフタイム管理、
スレッド間データ受け渡し、
整合性保証が必要な場合は独立したインスタンスを保持する。

適用方針は以下とする。

- 読み取り専用入力は参照渡しを推奨する。
- 不変オブジェクトの参照は共有可能とする。
- Queueへ格納するデータは独立したインスタンスとして保持する。
- RIMSnapshotは読み取り専用かつ生成後不変の独立オブジェクトとして保持する。
- Layer間で参照寿命が保証できないデータはコピーして受け渡す。

各Layerは本方針に従い、
性能とデータ整合性のバランスを考慮して実装する。

