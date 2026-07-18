# devices/_skeleton — 新機種テンプレート

新しい機種(例: `printer_b`)を追加するときの雛形です。

## 追加手順

1. このフォルダをコピーして機種名にリネームする。

   ```
   cp -r devices/_skeleton devices/printer_b
   ```

2. `Skeleton` を機種名(例 `PrinterB`)へ一括置換し、TODO を実装する。
   - `datastore/SkeletonProvider` … その機種の RIMDataDefinition / SensorDefinition を定義
   - `adapter/SkeletonSensorAdapter`・`adapter/rule/SkeletonSensorRule` … センサ変換の具象
   - `capability/SkeletonCapabilityRuleSet` … その機種の Capability 判定(閾値)

3. `devices/registry/` の配線を更新する。
   - `datastore/MachineProfile.hpp` … enum に機種を追加(既存)
   - `datastore/MachineProfileFactory.cpp` … profile → Provider の分岐を追加
   - `adapter/AdapterFactory.cpp` … SensorId → 具象 Adapter の分岐を追加

4. `CMakeLists.txt` の `RIM_INCLUDE_DIRS` と `RIM_SOURCES` に
   `devices/printer_b/inc` と `devices/printer_b/src/*.cpp` を追加する。

## 設計の約束

- **framework(機種共通)には触れない**。機種差はすべて devices 側で吸収する。
- Capability の**構造体型**は framework(公開契約)、**判定ロジック(Rule)**は devices。
- 抽象IF(`IDataDefinitionProvider` / `ISensorAdapter` / `ISensorRule` /
  `ICapabilityRuleSet`)を実装する形で機種を差し込む。
