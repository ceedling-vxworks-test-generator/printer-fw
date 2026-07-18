#pragma once

//
// CollectionOp - 管理配列(キー付きコレクション)への操作動詞。rim_collection_op_t 相当。
// 「配列に1件 追加/削除/既存更新/全消去」を汎用化。FaultRegistry が最初の実体。
//

namespace rim
{

enum class CollectionOp
{
    kNone = 0,
    kAdd,
    kRemove,
    kUpdate,
    kClearAll
};

} // namespace rim
