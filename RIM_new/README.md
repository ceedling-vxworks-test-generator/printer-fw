# RIM_new — ReactiveInfoManager (C++ OO 実装)

`rim/`(C-firstハイブリッド)の各レイヤ処理を **C++ OO ＋ framework/devices 構造**へ全面移植した実装。
機器のセンサ値を収集・正規化し、性質別にステージングして Snapshot を作り、Capability
(意味づけ)を生成して購読者へ配信する。

## パイプライン

```
Adapter(L1) → DataStore(L2) → CapabilityManager(L3) → Publisher(L4)
                   │
                   └→ Accessor(隣接・Pull参照)
```

1. **Adapter(L1)**: 型自由な受理点 `PushI32/PushF/PushU32`(内部で double へ集約)。
   `IDataProfile::Convert` で id 別に正規化、`Classify` で性質判別し、DataStore の3種postへ。
   `Submit` はコレクション操作(add/remove/update/clear_all)。
2. **DataStore(L2)**: 性質別ステージング。
   - **FAULT**: `RingBuffer` FIFO ＋ `FixedMap` レジストリ(喪失不可・順序保証)
   - **OPERATION**: `RingBuffer` FIFO ＋ 最新JobProgress
   - **CURRENT**: slot＋dirty の latest-wins
   `Dispatch()` で取り込み→レジストリ反映→変化ドメインを `IUpdateNotifier` で通知。
   `Capture()` で MachineSnapshot を生成。排他は `std::mutex`(通知発火の前に解放)。
3. **CapabilityManager(L3)**: 更新通知を受け Snapshot を取得 → `ICapabilityEvaluatorSet`
   (機種の判定式)で `CapabilitySet` を生成 → Publisher へ配信。
4. **Publisher(L4)**: 購読者(`ISubscriber`)へ Push 配信。
5. **Accessor**: 現在の Snapshot / Capability を Pull で都度取得。

## framework / devices

- **framework/**(機種共通): パイプライン機構・共通型(RIMDataId/RIMValue/DataContext/Snapshot…)・
  固定容量コンテナ(RingBuffer/FixedMap/FixedVector)・抽象IF・`RIMSystem`(結線)。
- **devices/printer_a/**(機種可変): `PrinterADataProfile`(Classify/Convert)、
  `PrinterAEvaluatorSet`(printable / temp_alert)。
- **devices/_skeleton/**: 新機種テンプレ(ビルド対象外・README参照)。

## rim/ からの設計置換

| rim/(C-first) | RIM_new(C++ OO) |
|---------------|-----------------|
| 関数ポインタテーブル(notifier/subscriber/evaluator) | 抽象IF(`IUpdateNotifier`/`ISubscriber`/`ICapabilityEvaluatorSet`) |
| `rim_port_lock/unlock`(weak seam) | `std::mutex` |
| has_xxx フラグ | `std::optional` |
| classify()/convert() が各所に散在 | `IDataProfile`(devices)へ集約 |
| グローバル状態＋extern "C" | `RIMSystem` が各レイヤ実体を所有・注入結線 |

> 公開 C ABI(extern "C")は `rim/` の役割。RIM_new はフルC++実装のため C API は持たない。

## ドメイン(スケルトン範囲)

- データ: Temperature / Humidity / Pressure / JobProgress / FaultCode
- Capability: printable(activeなfault無し) / temp_alert(温度>60℃)

## ビルド・テスト

要 C++17 ツールチェーン。

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

テストは `rim/examples/rim_demo.c` のシナリオを移植:
型自由 push の正規化 / FAULTキューの溢れ・順序・更新 / latest-wins /
Evaluator(printable・temp_alert) / Accessor Pull。
