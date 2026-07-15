# RIM (ReactiveInfoManager) 実装スケルトン

`ex/` の設計（旧称FSI）を **RIM＝ReactiveInfoManager** として実装するための骨組み。
設計の全体像・命名・方針は [`../ex/共通/ReactiveInfoManager概要.md`](../ex/共通/ReactiveInfoManager概要.md)、
言語選定の根拠は [`../ex/共通/実装言語比較_C_vs_Cpp.md`](../ex/共通/実装言語比較_C_vs_Cpp.md) §0 を参照。

## 実装方針（C-firstハイブリッド）

| 項目 | 方針 |
|------|------|
| **公開IF** | すべて `extern "C"`（C ABI）。C言語の解釈でそのまま使用・実装できる |
| **内部実装** | 基本C（タグ付きunion / `bool`+out引数 / `has_xxx`フラグ / 関数ポインタテーブル＋ctx / 連番enum＋固定長配列 / 短時間mutex） |
| **C++を使う箇所（限定）** | **(a) 可変長Listの仕組み** = `rim/src/rim_list.cpp` の `rim::FixedVector<T,N>`（固定容量・静的確保） |
| | **(b) RIM_AdapterLayer 受理点の型の自由さ** = `rim/src/rim_adapter.cpp` の `intake<T>()` テンプレート |
| 使わない | 例外・RTTI・STLコンテナ・動的確保・atomic shared_ptr・テンプレートメタプロ |

## レイヤと公開ヘッダ

| レイヤ | ヘッダ | 実装 |
|--------|--------|------|
| L1 RIM_AdapterLayer | `include/rim/rim_adapter.h` | `src/rim_adapter.cpp`（型自由な受理点＝C++テンプレート） |
| L2 RIM_DatastoreLayer | `include/rim/rim_datastore.h` | `src/rim_datastore.cpp`（Registry＝固定長配列・3種post検証） |
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
4. 隣接 Accessor Layer（Pull）で現在Capability/Snapshotを都度取得。

## スケルトンの範囲（未実装・簡略箇所）

- 排他制御（port mutex）・Dispatcherの周期スレッド・Queueの本格実装は簡略化。
  スケルトンは `rim_datastore_dispatch()` の明示呼び出しで駆動する。
- OperationReport の順序保証・喪失検知（フル再同期）は未実装（設計 §10.1 準拠は今後）。
- Evaluator は代表2種（printable / temp_alert）のみ。
