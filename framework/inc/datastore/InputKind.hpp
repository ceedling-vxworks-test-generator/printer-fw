#pragma once

//
// InputKind - 入力性質(Fault / OperationReport / CurrentValue)。
// Adapter が判別し CentralInputPort の3種post選択に用いる。
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
