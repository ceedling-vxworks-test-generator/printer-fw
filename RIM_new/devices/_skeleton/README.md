# devices/_skeleton — 新機種テンプレート

新しい機種(例: `printer_b`)を追加するときの雛形。ビルド対象外。

## 追加手順

1. コピーして機種名にリネーム: `cp -r devices/_skeleton devices/printer_b`
2. `Skeleton` を機種名(例 `PrinterB`)へ一括置換し、TODO を実装する。
   - `adapter/SkeletonDataProfile` … `IDataProfile` 実装。この機種の
     **Classify**(id→性質) と **Convert**(生値→正規化 RIMValue) を定義。
   - `capability/SkeletonEvaluatorSet` … `ICapabilityEvaluatorSet` 実装。
     この機種の Capability 判定(閾値)を定義。
3. `CMakeLists.txt` の `RIM_INCLUDE_DIRS` に `devices/printer_b/inc`、
   `RIM_SOURCES` の glob に `devices/printer_b/src/*.cpp` を追加。
4. 結線は `RIMSystem`(framework) に profile と evaluatorSet を注入するだけ:
   ```cpp
   PrinterBDataProfile   profile;
   PrinterBEvaluatorSet  evaluators;
   RIMSystem system{profile, evaluators};
   system.Init();
   ```

## 設計の約束

- **framework(機種共通)には触れない**。機種差はすべて devices 側で吸収する。
- Capability の**構造体型**(CapabilitySet)は framework(公開契約)、
  **判定ロジック**(Evaluator)は devices。
- 抽象IF(`IDataProfile` / `ICapabilityEvaluatorSet`)を実装して機種を差し込む。
