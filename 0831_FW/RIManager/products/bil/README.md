# BIL Product

`製品要求定義/ProductRequirement_BIL追記.xlsx`(Domain/Data/Capabilityシート)を入力として、
`製品要求定義/製品要求定義書仕様.md` の対応表に従い作成した製品実装。

ベースは `products/test_product`(現時点の最新実装パターン)。`products/skeleton` は
古い構成のため使用していない。

---

## 要求定義書との対応

| 要求定義書 | 実装ファイル |
| --- | --- |
| Domainシート | `Domain/Domains.hpp` |
| Dataシート | `data_id.h`, `types.h`, `DataItem/DataItems.hpp`, `DataItem/NormalizationFunctions.*` |
| Capabilityシート | `capability_id.h`, `types.h`, `CapabilityItem/Capabilities.hpp`, `CapabilityItem/CapabilityBuilders.*` |

Facadeシートは要求定義書に存在しないため、本製品にFacadeは無い
(`ProductDefinition::facades/facadeCount` は `nullptr`/`0`)。

---

## 型の扱いについて

要求定義書のValueType列はBool/Int32/Double/Binaryのみを想定しているが、実際のData記載内容
(`layer_init_state`配列、`print_job_info_t`構造体等)はいずれも固定長配列/構造体であり、
単純なスカラ値ではない。RIManagerのValueTypeには構造体/配列を直接表現する型が無いため、
既存の`ErrorList`(`RI_FAULT_INFO_LIST`)と同じ方式で「`ValueType::kBinary` + 製品側で
定義したC構造体(`types.h`)」として扱った。

Capability(`product_state`/`supply_status`/`print_job_status`)はいずれも列挙型の状態を
返すため `ValueType::kInt32` とした。

---

## 要確認事項(実装時の解釈・仮定)

1. **配列サイズのプレースホルダ**: `PRODUCT_LAYER_NUM` / `MAX_PRINT_JOB_NUM` /
   `UNIT_ID_NUM` / `USE_CASE_EXECUTION_NUM` / `PRINT_COLOR_NUM` /
   フォールト・機能制限の最大件数は、要求定義書に具体的な数値が記載されていないため
   `types.h` 冒頭で暫定値を置いている。実機仕様確定後に更新すること。

2. **`product_state` の判定優先順位**: GenerationRule「初期化が終わっているか？
   実行中のJobはあるか？エラーor機能制限があるか？」を
   `UNINIT → ERROR(フォールト有) → RECOVERY(機能制限のみ) → BUSY → IDLE` の順で
   判定するよう実装した(`CapabilityItem/CapabilityBuilders.cpp`)。この優先順位は
   要求定義書に明記が無いため、要件定義者への確認を推奨する。

3. **`print_job_status` の判定**: GenerationRule「印字ジョブがあるか？」を
   字句通りに「ジョブが1件でもあればABNORMAL」と実装したが、ジョブの存在自体を
   異常とみなす意図で合っているか要確認(エラー中のジョブのみ異常とする、等の
   可能性もある)。

4. **ルーティング/圧縮ポリシー**: 要求定義書はルーティング方針を規定していないため、
   全DataItemを単一ルート(`ROUTE_DATA`)にまとめ、更新頻度の記載(60回/秒等)を
   参考にCompressionPolicyを仮設定した。実運用の要件に応じて見直すこと。

5. **Adapter実装は未接続のスタブ**: `Adapter/Adapter.cpp` は実ハードウェア入力が
   無いため、起動時に`layer_init_state`を1回流すだけの最小限のスタブ。
   実装時のポインタ寿命に関する注意点はコード内コメントを参照。
