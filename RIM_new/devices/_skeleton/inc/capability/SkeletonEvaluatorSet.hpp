#pragma once

//
// SkeletonEvaluatorSet - 新機種の Capability 判定テンプレート。ICapabilityEvaluatorSet 実装。
// TODO: Skeleton を機種名へ置換し、Snapshot から CapabilitySet を組み立てる。
//

#include "capability/ICapabilityEvaluatorSet.hpp"

namespace rim
{

class SkeletonEvaluatorSet final
    : public ICapabilityEvaluatorSet
{
public:

    void Evaluate(
        const MachineSnapshot& snapshot,
        CapabilitySet& out) const override
    {
        // TODO: この機種の判定式を実装する。
        //   例: out.printable = !(snapshot.fault && snapshot.fault->activeCount > 0);
        (void)snapshot;
        (void)out;
    }
};

} // namespace rim
