#pragma once

//
// ICapabilityEvaluatorSet - 機種可変の Capability 判定式(Evaluator)集合の抽象IF。
// rim の関数ポインタ配列 g_evaluators[] を devices 実装の抽象へ置換。
// CapabilityManager が Snapshot を渡して呼び、CapabilitySet を組み立てる。
//

#include "capability/CapabilitySet.hpp"

#include "datastore/Snapshot.hpp"

namespace rim
{

class ICapabilityEvaluatorSet
{
public:

    virtual ~ICapabilityEvaluatorSet() = default;

    virtual void Evaluate(
        const MachineSnapshot& snapshot,
        CapabilitySet& out) const = 0;
};

} // namespace rim
