# devices/_skeleton — 新機種テンプレート

新機種(例 `printer_b`)追加の雛形。ビルド対象外。

## 追加手順

1. `cp -r devices/_skeleton devices/printer_b`
2. `Skeleton` を機種名へ一括置換し TODO を実装。
   - `adapter/SkeletonDataProfile` … `IRuleResolver`(SelectRule) ＋ `IInputClassifier`(Classify) を実装。
     Rule実体を所有し、id→Rule と id→性質(InputKind) を定義。
   - `adapter/rule/*` … この機種の Rule(生値→正規化 RIMValue)。
   - `capability/SkeletonCapabilityBuilder` … `ICapabilityBuilder` を実装し、
     Snapshot から8種Capabilityを生成。
3. `CMakeLists.txt` の `RIM_INCLUDE_DIRS` に `devices/printer_b/inc`、
   `RIM_SOURCES` に `devices/printer_b/src/*.cpp` を追加。
4. 結線は `RIMSystem`(framework) に注入するだけ:
   ```cpp
   PrinterBDataProfile      profile;      // classifier 兼 resolver
   PrinterBCapabilityBuilder builder;
   RIMSystem system{profile, profile, builder};
   system.Init();
   ```

## 約束
- framework(機種共通)には触れない。機種差は devices 側で吸収する。
- Capability の**構造体型**(CapabilitySet)は framework(公開契約)、**判定**(Builder)は devices。
