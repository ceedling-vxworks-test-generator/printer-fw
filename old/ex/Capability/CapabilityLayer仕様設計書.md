# RIM_CapabilityLayer 基本設計書
## 1. はじめに
### 1.1 本書の目的
本設計は、RIM_CapabilityLayerの役割・責務・構成および設計原則を定義することを目的とする。
RIM_CapabilityLayerは、Domain DataStoreに保持された事実データを入力として受け取り、意味のある状態（Capability）を導出する層である。
本設計により、
- 状態の意味解釈を一箇所に集約
- 判定ロジックの分散防止
- UI・制御・外部連携に対する一貫した状態提供
を実現する。

### 1.2 対象範囲
本書で扱う範囲を明確化する。

### 1.3 用語定義
| 用語 | 説明 |
|--------|--------|
| DataEntryItem | RIM_DatastoreLayer内部で利用する標準データモデル |
| Data | Mediator外部から入力される情報。 |
| State | Dataを解釈した結果。Dataから再計算可能なものである。RIM_CapabilityLayerにて用いられる。 |
| DataId | データ種別識別子。RIM_DatastoreLayerにおいては保存先特定を主目的として利用する。 |
| Context | データに付与される補足情報 |
| Registry | システム内のデータを集約して管理するコンポーネント |
| RegistryDomain | Registry内の保存領域を示すドメイン。例えばRegistry内の温度湿度関係データのドメインといったように指定可能。 |
| RegistryDomainSet | RegistryDomainの複数指定版 |
| FaultInput | 異常系の入力。Error、Warningなど緊急性が高く、すぐにでも処理する必要がある入力。 |
| OperationReport | 動作報告系の入力。動作報告は順序を守る必要があるため、欠落や処理順序の入れ替えなどが起きてはいけない入力。 |
| CurrentValue | 最新値系の入力。ドライバから上がってくるセンサの値など最新値が欲しい入力。 |
| Snapshot | ある時点のRegistry状態を読み取り専用で表現したデータ |

## 2. RIM_CapabilityLayerの位置づけ
### 2.1 システム全体構成
RIM_AdapterLayer（正規化・変換）  
        ↓  
RIM_DatastoreLayer（事実保持）  
        ↓  
RIM_CapabilityLayer（意味・判断）  
        ↓  
Hint / Facade（解釈・制御補助）  
        ↓  
Publisher / 外部IF（公開）  

### 2.2 RIM_CapabilityLayerの責務
- Registry更新通知（onRegistryUpdated）の受理
- 必要範囲のSnapshot取得（MachineSnapshotReader経由）
- Snapshotから意味のある状態（Capability）を導出する
- 閾値跨ぎ（Threshold）の判定と派生イベントの発行
- 前回Capabilityとの意味的差分の判定
- Publish優先度の判定
- RIM_PublisherLayerへのCapability通知

### 2.3 RIM_CapabilityLayerの非責務
- Registryの直接参照・更新（RIM_DatastoreLayer）
- Snapshotの実コピー処理（MachineSnapshotReader）
- 状態の永続保持（RIM_DatastoreLayerが正本）
- 購読者への実配信・配信的差分の管理（RIM_PublisherLayer）
- 現在値の参照提供（Accessor Layer）

## 3. 入出力仕様
### 3.1 入力
RIM_CapabilityLayerへの主な入力はRIM_DatastoreLayerからのRegistry更新通知である。

入力内容：  
- 更新されたRegistry領域がどこかわかるもの

### 3.2 参照データ
RIM_CapabilityLayerは直接RIM_DatastoreLayerのRegistryを読まず、SnapshotReaderを介して参照する。

### 3.3 出力
RIM_CapabilityLayerはPublisherへ生成済みCapabilityを渡す。

出力内容：
生成されたCapability。


## 4. Capability生成方針
### 4.1 基本方針
RIM_CapabilityLayerは、RIM_DatastoreLayerから更新通知を受けたあと、必要なSnapshotを取得し、現在状態を生成する。
Registry更新通知  
    ↓  
必要Snapshot決定  
    ↓
Snapshot取得
    ↓
Capability生成
    ↓
前回Capabilityと比較
    ↓
変化ありならPublisherへ通知（ErrorCapおよびJobCapに変化があったならばそれに加えて明示イベントとしても通知する）

### 4.2 Capability生成単位
Capabilityは意味のある製品状態の単位で生成する。本システムでは以下の8種を対象とする。

| Capability | 意味 | 主な依存Registry |
|-----------|------|-----------------|
| ErrorCap | エラー集約 | FaultRegistry |
| JobCap | ジョブ進捗 | OperationRegistry |
| EnvCap | 温湿度範囲 | CurrentValueRegistry |
| MaintCap | メンテ要否 | OperationRegistry / CurrentValueRegistry |
| HealthCap | ユニット死活/劣化 | FaultRegistry / OperationRegistry |
| SafetyCap | カバー/E-Stop | FaultRegistry / CurrentValueRegistry |
| ConsumableCap | インク/ワイパー残量 | CurrentValueRegistry |
| PrintCap | 印字可能性 | （下記参照） |

### 4.3 Capability構成
各Capabilityは、Snapshot（観測事実）から導出される再計算可能な状態である。

- Capabilityは状態（名詞）であり、判定式の適用結果として得られる
- CapabilityはDataStoreの値のみに依存し、他Capabilityには依存しない

> **PrintCap（印字可能性）の設計原則**：PrintCapは一見他Cap（Error/Safety/Consumable等）を
> 参照するように見えるが、**実際にはDataStore（Registry/Snapshot）の値にのみ依存する**。
> 複数Capを合成した「印字可能か」の総合判断が必要な場合、その合成はFacade側で行い、
> RIM_CapabilityLayer内でCap同士を相互参照させない。

### 4.4 Capability生成ルール
- 各Capabilityの種類ごとに判定クラス（判定式）を持つ（CapabilityBuilder配下）
- 判定式はSnapshotを入力とし、対応するCapabilityを出力する
- **閾値跨ぎ（Threshold）の判定は本層で行う**。連続値が閾値を跨いだ場合、
  該当Capabilityの変化として扱い、派生イベントを発行する
  （閾値の「跨ぎ判定」は判断であり本層の責務。配信タイミング制御はRIM_PublisherLayer）
- ErrorCap / JobCap に変化があった場合は、通常の変更通知に加えて明示イベントとしても通知する

## 5. Snapshot取得設計
RIM_CapabilityLayerは毎回すべてのRegistryを参照せず、更新内容に応じて必要なSnapshotのみ取得する。  
Snapshot取得対象は、RegistrtyDomainで指定する。(RIM_DatastoreLayer参照)  
Snapshot取得時の排他はSnapshotReader側で行う。
ステータス管理モジュールはRegistryを直接ロックしない。

## 6. Capability生成設計
RIM_CapabilityLayerは取得したSnapshotを元にCapabilityを生成する。  
Capabilityの種類ごとに判定クラスが存在しており、それを用いてSnapshotからCapabilityの生成を行う。  
各判定クラスについては現在TBD

## 7. 差分検知設計
RIM_CapabilityLayerは生成したCapabilityと前回生成済のCapabilityを比較する。
差分がある場合、Capability変更検知通知をRIM_PublisherLayerに通知する。
差分のある箇所がErrorCapおよびJobCapならば、それに加えて明示イベントとしても通知する。


## 8. クラス構成  
RIM_CapabilityLayerを構成するクラス、およびRIM_CapabilityLayerが利用する外部IFについて定義する。

RIM_CapabilityLayer内のクラスは、以下を対象とする。
- CapabilityManager
- CapabilityBuilder
- CapabilityDiffChecker
- CapabilityPriorityChecker

また、RIM_CapabilityLayerは以下の外部IFを利用する。
- IMachineSnapshotReader
- IPublisher

### 8.1 クラス構成概要
RIM_CapabilityLayerのクラス構成図を以下に示す。
```mermaid
TBD
```

CapabilityManagerは、DataStor Layerからの更新通知を契機として、Snapshot取得、Capability生成、差分判定、Publisher通知を制御する。

CapabilityBuilderは、SnapshotからCapabilityを生成する。

CapabilityDiffCheckerは、前回のCapabilityと今回生成したCapabilityを比較し、Publisherへ通知すべき差分があるかを判定する。

CapabilityPriorityCheckerは、今回生成した、Capabilityの内容をもとに、Publish優先度を判定する。

### 8.2 RIM_CapabilityLayerクラス一覧
| クラス | 概要 |
|--------|--------|
| CapabilityManager | RIM_CapabilityLayer処理の中心クラス |
| CapabilityBuilder | SnapshotからCapabilityを生成するクラス |
| CapabilityDiffChecker | 前回Capabilityと今回Capabilityの差分を判定するクラス |
| CapabilityPriorityChecker | 今回生成したCapabilityの内容をもとに、Publish優先度を判定するクラス。 |

### 8.3 外部依存IF一覧
| IF | 所属Layer | 概要 |
|--------|--------|--------|
| IMachineSnapshotReader | RIM_DatastoreLayer | RegistryからSnapshotを取得するためのIF |
| IPublisher | RIM_PublisherLayer | 生成したCapabilityをPublisher層へ渡すためのIF |

上記クラスは、RIM_CapabilityLayer内のクラスではない。
ただしRIM_CapabilityLayerが処理を行うために利用する外部依存IFとして、本章に記載する。

### 8.4 CapabilityManager
#### 8.4.1 責務
CapabilityManagerは、RIM_CapabilityLayer処理全体を制御する中心クラスである。

主な責務を以下に示す。
- Registry更新通知を受ける。
- 更新されたRegistryDomainをもとにSnapshot取得範囲を決定する。
- IMachineSnapshotReaderを介してSnapshotを取得する。
- CapabilityBuilderにCapability生成を依頼する。
- CapabilityDiffCheckerに差分判定を依頼する。
- CapabilityPriorityCheckerにPublish優先度判定を依頼する。
- Publishが必要な場合、IPublisherへKachineStatusを渡す
- 前回Publish済のCapabilityを保持する。

### 8.4.2 非責務
CapabilityManagerは、以下を行わない。
- Registryへの入力反映
- Registryの直接参照
- Registryの直接六
- Snapshotの実コピー処理
- Capabilityの詳細な生成ロジック
- Subscriberへの実配信処理

### 8.4.3 提供IF
CapabilityManagerの提供IFを以下に示す。

| IF | 内容 |
|--------|--------|
| onRegistryUpdated(RegistryDomainSet& domains) | RIM_DatastoreLayerからのRegistry更新通知を受け、ステータス生成処理を開始する。 |

※IFは疑似コードであり、特定の言語の実装に準拠しない。

### 8.4.4 内部処理
Registry更新通知  
    ↓  
Snapshot取得範囲決定  
    ↓  
Snapshot取得要求  
    ↓  
Capability生成  
    ↓  
差分判定  
    ↓  
重要度判定  
    ↓
RIM_PublisherLayerへ通知

### 8.4.5 データ型
```
sturct Capability {
    Capability全体,
    publish優先度,
    変更ドメイン,
}
```

## 8.5 CapabilityBuilder
### 8.5.1 責務
CapabilityBuilderは、SnapshotからCapabilityを生成するクラスである。

主な責務を以下に示す。
- SnapshotからCapabilityを生成する。

### 8.5.2 非責務
CapabilityBuilderは、いかを行わない。
- Registry更新通知の受信
- Snapshot取得
- 前回Capabilityとの差分判定
- RIM_PublisherLayerへの通知
- Registryの直接参照

### 8.5.3 提供IF
CapabilityBuilderの提供IFを以下に示す。

| IF | 内容 |
|--------|--------|
| build(Snapshot) | Snapshotを元にCapabilityを生成する。 |

※IFは疑似コードであり、特定の言語の実装に準拠しない。

## 8.6 CapabilityDiffChecker
### 8.6.1 責務
CapabilityDiffCheckerは、前回生成済のCapabilityと今回生成したCapabilityを比較し、RIM_PublisherLayerへ通知すべき差分があるかを判定するクラスである。

主な責務を以下に示す。
- 前回Capabilityと今回Capabilityを比較する。
- Capabilityに差分があるか判定する。

### 8.6.2 非責務
CapabilityDiffCheckerは以下を行わない。
- Capabilityの生成
- Snapshotの取得
- RIM_PublisherLayerへの通知
- 前回Capabilityの更新管理

### 8.6.3 提供IF
CapabilityDiffCheckerの提供IFを以下に示す。
| IF | 内容 |
|--------|--------|
| hasDifference(previousCapability, currentCapability) | 前回Capabilityと今回Capabilityを比較し、差分がある場合に差分有無と変更Capabilityを返す。 |

### 8.6.4 データ型
```
struct StatusDiffResult{
    変更有無,
    変更Capabilityがどれか
}

```
## 8.7 CapabilityPriorityChecker（TBD）
TBD
CapabilityPriorityCheckerは今回生成したCapabilityの内容をもとに、Publish優先度を判定するクラスである。
Publisher層でPublishの優先度を制御するために用いられる。

### 8.7.1 責務
主な責務を以下に示す。
- Capabilityの内容から優先度を判定する

### 8.7.2 非責務
CapabilityPriorityCheckerは以下を行わない。
- Capabilityの生成
- Snapshotの取得
- RIM_PublisherLayerへの通知
- 前回Capabilityの更新管理

### 8.7.3 関連データ型
```
CapabilityPriority
```
Low、Normal、High、Criticalのいずれかの値をとるenum。

### 8.7.4 提供IF
CapabilityPriorityCheckerの提供IFを以下に示す。
| IF | 内容 |
|--------|--------|
| check(capability) | Capabilityの内容を元にPublish優先度を返す。 |



## 8.7 IMachineSnapshotReader（外部依存IF）
本クラスはRIM_DatastoreLayerが提供するSnapshot取得IFである。
CapabilityaManagerがRegistry情報を参照するために利用する。
| IF | 内容 |
|--------|--------|
| capture(SnapshotRequest request) | 指定されたRegistryDomainSetに対応するSnapshotを取得する。 |

詳細については、RIM_DatastoreLayerの設計書を参照すること。

## 8.8 IPublisher
本クラスはRIM_PublisherLayerが提供するSnapshot取得IFである。
CapabilityManagerがRIM_PublisherLayerにPublish Triggerを通知するために用いる。
| IF | 内容 |
|--------|--------|
| notify(capability) | capabilityを渡す。 |


## 8.9 クラス間の呼び出し関係
TBD

## 8.10 クラス構成上の方針
本構成では以下の方針とする。
- CapabilityManagerに処理を集中させすぎない。
- RIM_CapabilityLayerはRegistryを直接参照しない
- RIM_CapabilityLayerはSubscriberに直接Publishしない。

## 9. 処理シーケンス
TBD

## 10. 異常処理設計
### 10.1 Snapshot取得失敗
Snapshot取得失敗時は、Capability生成を中止し、ログを出力する。

### 10.2 不整合Snapshot
Snapshot内のデータに不整合がある場合、軽微であれば可能な範囲でCapabilityを生成する。  
重大な不整合の場合はCapability生成を中止し、ログを出力する。  

## 11. 排他・リアルタイム性設計
RIM_CapabilityLayerはRegistyを直接ロックしない。  
MachineSnacpshotReaderが必要なRegistry領域のみ短時間ロックし、Snapshotをコピーする。  
参照渡しではなくコピーであるのは書き込みと読み込みが競合してしまうことを防ぐためである。　

Snapshotタイミング以外でRegistryロックを行わない。

## 12. テスト方針
TBD

## 13. 未決事項
