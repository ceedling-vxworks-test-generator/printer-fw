# printer-fw — ReactiveInfoManager (C++ OO / 旧ex仕様③準拠)

`old/ex/` の各レイヤ仕様③(2026-07-07 正)を **C++ OO ＋ framework/devices 構造**で実装したもの。
本ディレクトリ構成が printer-fw の正式なコード。旧`pf_*`フレームワーク一式は `old/` を参照。
機器のセンサ値を受理・正規化し、性質別にステージングして Snapshot を作り、8種Capabilityを
生成して購読者へ配信する。

## レイヤ構成(仕様③)

```
RIM_AdapterLayer(L1) → RIM_DatastoreLayer(L2) → RIM_CapabilityLayer(L3) → RIM_PublisherLayer(L4)
                              │
                              └→ Accessor Layer(隣接・Pull参照)
```

### L1 RIM_AdapterLayer
`RawDataInput`(型自由 PushI32/PushF/PushU32) → `IRuleResolver.SelectRule` → `IRule.Convert`(正規化)
→ `IInputClassifier.Classify`(性質判別) → CentralInputPort の3種post。

### L2 RIM_DatastoreLayer
`CentralInputPort`(3種post・検証) → 3レーン(`FaultInputQueue`/`OperationReportQueue`/`CurrentValueBuffer`)
→ 3Dispatcher(`FaultDispatcher`/`OperationDispatcher`/`CurrentValueDispatcher`)
→ 3Registry(`FaultRegistry`/`OperationRegistry`/`CurrentValueRegistry`) ← `MachineRegistry`(集約)
→ `MachineSnapshotReader.Capture`。`Registry.Apply`→変更ドメイン、`notifyUpdated(RegistryDomainSet)`。
ドメイン単位 mutex。RegistryDomain は Fault/Operation/Environment/Consumable/Safety/Maintenance/Health。

### L3 RIM_CapabilityLayer
`CapabilityManager`(IRegistryUpdateNotifier実装) が更新通知→Snapshot取得→`ICapabilityBuilder.Build`
→`CapabilityDiffChecker`(意味的差分)→`CapabilityPriorityChecker`(優先度)→`IPublisher.Notify`。
8種Capability(Error/Job/Env/Maint/Health/Safety/Consumable/Print)。ErrorCap/JobCap変化は Event 通知。

### L4 RIM_PublisherLayer
`PublishEngine`(IPublisher実装) が トリガ評価・Rate Limit → `SubscriptionBroker`(関心Cap一致)
→ `StateRepository`(購読者ごと配信的差分) → Push配信。5トリガ(OnChange/Periodic/Threshold/Event/Initial)。
Event / 優先度Critical・High は Rate Limit 除外。

### Accessor Layer
`PrinterStatusReader.GetPrinterStatus` → `PrinterStatus{data, capability}`。参照専用・Pull・状態非保持。

## framework / devices

- **framework/**(機種共通): 全レイヤ機構・共通型・抽象IF(IRule/IRuleResolver/IInputClassifier/
  IRegistryUpdateNotifier/IMachineSnapshotReader/ICapabilityBuilder/IPublisher/ISubscriber)・
  固定容量コンテナ・`RIMSystem`(結線)。
- **devices/printer_a/**(機種可変): `RIMDataId`(id一覧)、`PrinterADataProfile`(SelectRule/Classify)、
  Rule群、`PrinterACapabilityBuilder`(8種判定)。`RIMDataId` は devices 側の定義で、framework は
  ヘッダ名にのみ依存する(機種展開してもframework無改修。詳細は`docs/adapter/02_詳細設計.md`§1.1)。
- **devices/_skeleton/**: 新機種テンプレ(ビルド対象外・README参照)。

## 設計判断(仕様TBDの確定)
- 更新通知後の取得範囲: PrintCap 等の横断依存のため全ドメインを capture。
- 優先度ポリシー: Error/非Safety=Critical、Maint/消耗品少=High、Job/Env変化=Normal、他=Low。
- 閾値: 温度>60℃、湿度20-80%、消耗品<10%、メンテカウンタ>10000。
- 喪失検知: Queue満杯を kErrPost で検知。フル再同期プロトコルは今後(hook)。
- Rate Limit: 最小間隔既定0(無効)。トレーリングエッジ配信は周期タスク導入時に実装(hook)。
- C API(extern"C")は持たない(旧`old/rim/` の役割)。

## ビルド・テスト
```sh
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
```
テスト: Registry(Fault/Operation/CurrentValue) / Capability(Builder/Diff/Priority) / 全系フロー(SystemFlow)。
