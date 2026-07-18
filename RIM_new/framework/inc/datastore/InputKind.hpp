#pragma once

//
// InputKind - 入力性質(GAP-1)。Adapterが判別しpost選択に用いる。rim_input_kind_t 相当。
//   kFault           : 異常系。喪失不可・高優先
//   kOperationReport : 動作報告系。順序保証・喪失不可
//   kCurrentValue    : 最新値系。上書き可
//

namespace rim
{

enum class InputKind
{
    kFault = 0,
    kOperationReport,
    kCurrentValue
};

} // namespace rim
