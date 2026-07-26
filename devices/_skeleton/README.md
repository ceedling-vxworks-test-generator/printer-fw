# devices/_skeleton — 新機種テンプレート

新機種(例 `printer_b`)追加の雛形。ビルド対象外。

## 追加手順

1. `cp -r devices/_skeleton devices/printer_b`
2. `Skeleton` を機種名へ一括置換し TODO を実装。
   - `datastore/RIMDataId.hpp` … この機種のデータ種別一覧(id)を定義する。`kCount` は末尾の番兵。
     framework はこのヘッダの中身を知らず、型名(`RIMDataId`/`ToIndex`/`kRIMDataIdCount`)だけに
     依存するため、機種ごとに差し替えても framework は無改修のまま。
   - `adapter/SkeletonDataProfile` … `IRuleResolver`(SelectRule) ＋ `IInputClassifier`(Classify) を実装。
     id→(Rule, 性質) のテーブルで定義する(`PrinterADataProfile.cpp` を参照。Rule実体は状態を
     持たないため、id横断で共有する単一インスタンスとして持つ)。
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
- `RIMDataId`(id一覧)は devices 側が定義する。framework は型名にのみ依存し、
  id の中身(何種類あるか・何を意味するか)を一切知らない。
- Capability の**構造体型**(CapabilitySet)は framework(公開契約)、**判定**(Builder)は devices。
