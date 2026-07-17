# RIM (ReactiveInfoManager) 実装スケルトン

`ex/` の設計（旧称FSI）を **RIM＝ReactiveInfoManager** として実装するための骨組み。
設計の全体像・命名・方針は [`../ex/共通/ReactiveInfoManager概要.md`](../ex/共通/ReactiveInfoManager概要.md)、
言語選定の根拠は [`../ex/共通/実装言語比較_C_vs_Cpp.md`](../ex/共通/実装言語比較_C_vs_Cpp.md) §0 を参照。

## 実装方針（C-firstハイブリッド）

| 項目 | 方針 |
|------|------|
| **公開IF** | すべて `extern "C"`（C ABI）。C言語の解釈でそのまま使用・実装できる |
| **内部実装** | 基本C（タグ付きunion / `bool`+out引数 / `has_xxx`フラグ / 関数ポインタテーブル＋ctx / 連番enum＋固定長配列 / 短時間mutex） |
| **C++を使う箇所（限定）** | **(a) 可変長Listの仕組み** = `include/rim/rim_fixed_vector.h` の `rim::FixedVector<T,N>`（固定容量・静的確保） |
| | **(b) RIM_AdapterLayer 受理点の型の自由さ** = `rim/src/rim_adapter.cpp` の `intake<T>()` テンプレート |
| | **(c) 性質別キュー** = `include/rim/rim_ring_buffer.h` の `rim::RingBuffer<T,N>`（固定容量FIFO） |
| | **(d) 管理配列（コレクション）** = `include/rim/rim_fixed_map.h` の `rim::FixedMap<K,V,N>`（キー付き・add/remove/update） |
| 使わない | 例外・RTTI・STLコンテナ・動的確保・atomic shared_ptr・テンプレートメタプロ |

> **1クラス1ファイル**: C++テンプレートの実体クラスは各1ヘッダに独立
> （`rim_fixed_vector.h` / `rim_ring_buffer.h` / `rim_fixed_map.h`）。いずれも
> `#ifndef __cplusplus / #error` ガード付きで、公開C ABIには一切漏れない。

## レイヤと公開ヘッダ

| レイヤ | ヘッダ | 実装 |
|--------|--------|------|
| L1 RIM_AdapterLayer | `include/rim/rim_adapter.h` | `src/rim_adapter.cpp`（型自由な受理点＝C++テンプレート） |
| L2 RIM_DatastoreLayer | `include/rim/rim_datastore.h` | `src/rim_datastore.cpp`（性質別: FAULT/OP=FIFOキュー, CURRENT=latest-wins。Fault Registry＝FixedMap） |
| 排他ポート（H-7） | `include/rim/rim_port.h` | `src/rim_port.cpp`（`rim_port_lock/unlock`。既定no-op weak・実機で差替） |
| L3 RIM_CapabilityLayer | `include/rim/rim_capability.h` | `src/rim_capability.cpp`（Evaluator＝関数ポインタ配列） |
| L4 RIM_PublisherLayer | `include/rim/rim_publisher.h` | `src/rim_publisher.cpp`（購読者Push配信） |
| 可変長List | `include/rim/rim_list.h` | `src/rim_list.cpp`（**C++ FixedVector** をC ABIで公開） |
| 共通型 | `include/rim/rim_types.h` | — |
| アンブレラ＋Accessor（隣接/Pull） | `include/rim/rim.h` | `src/rim_core.cpp` |

## ビルド・実行

**要 C++17 ツールチェーン**（内部実装がC++のため）。デモ `examples/rim_demo.c` は
**C言語**で書かれており、C ABI越しにRIMを使う（C開発者の利用形態の実証）。
Cデモのリンクには C++実体を含むため `-lstdc++` が必要。

```sh
# g++ 環境（Linux 等）
cd rim
make run          # librim.a をビルドし、Cデモ(rim_demo.c)をリンク・実行

# CMake
cmake -S . -B build && cmake --build build && ./build/rim_demo
```

> 注意: 本スケルトンを生成したWindows環境にはC++ツールチェーンが無く、
> ビルド実機検証は未実施（コード整合はレビューで確認）。g++ / clang++ / MSVC(C++)
> のいずれかがある環境でビルドすること。

## デモが示すこと

1. **要件3**: `rim_demo.c`（純C）から `extern "C"` IFを呼び、C++の存在を意識せず利用。
2. **要件4(b)**: `rim_adapter_push_f/_i32/_u32` に異なるnative型を渡せる（受理点の型の自由さ）。
3. **要件4(a)**: `rim_dei_list_*`（C++ FixedVector）を可変長ListとしてCから操作。
4. **コレクション操作＋性質別キュー**: `rim_adapter_submit(id, op, key, value, ctx)` で
   管理配列（Fault Registry）へ add/remove/update。以下を実証する:
   - **溢れ**: dispatch前に `RIM_FAULT_QUEUE_DEPTH+1` 件ADD → 超過分は `RIM_ERR_POST`（無音上書きしない）。
   - **順序**: `ADD A → ADD B → REMOVE A` を投入 → 反映後は B のみ（FIFO・喪失なし）。
   - **更新**: 既存キーの UPDATE 成功／不在キー操作の扱い。
5. 隣接 Accessor Layer（Pull）で現在Capability/Snapshotを都度取得。

## 性質別ステージング（設計意図の実装）

| 性質 (kind) | ステージ機構 | Registry | 理由 |
|------|------|------|------|
| FAULT | `RingBuffer<FaultEvent, N>` FIFO | `FixedMap<code, FaultEntry>` | 喪失不可・順序保証。add/remove/update の順序が結果に影響 |
| OPERATION_REPORT | `RingBuffer<OpEvent, N>` FIFO | 最新JobProgress | 喪失不可・順序保証 |
| CURRENT_VALUE | slot＋dirty（latest-wins） | 固定長配列 | 最新値優先。coalesceは機能（キュー化は逆効果） |

**排他 (H-7)**: `post_* / dispatch / capture` を `rim_port_lock/unlock` で直列化。
生産者はタスクのみ想定＝単一mutexで十分。`dispatch` は通知発火の**前に**ロック解放する
（Capabilityの通知先が `capture` を再入呼びするため）。

## スケルトンの範囲（未実装・簡略箇所）

- **排他ポート `rim_port.cpp` の既定はno-op**。実機（例: VxWorks `semMCreate/semTake/semGive`）で
  weakシンボルを上書きすること。Dispatcherの周期スレッド駆動は未実装
  （スケルトンは `rim_datastore_dispatch()` の明示呼び出しで駆動）。
- OperationReport の喪失検知（フル再同期）は未実装（設計 §10.1 準拠は今後）。順序/無損失は本FIFO化で担保。
- Evaluator は代表2種（printable / temp_alert）のみ。
