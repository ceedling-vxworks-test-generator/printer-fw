#pragma once

//
// CapabilityDiffChecker - 前回Capabilityと今回Capabilityの意味的差分を判定(仕様 §8.6)。
// 変更有無と変更Capability集合(CapabilityKindSet)を返す。
//

#include "capability/CapabilitySet.hpp"
#include "capability/CapabilityKind.hpp"

namespace rim
{

struct DiffResult
{
    bool              changed{false};
    CapabilityKindSet changedKinds{0};
};

class CapabilityDiffChecker
{
public:

    DiffResult HasDifference(const CapabilitySet& prev, const CapabilitySet& curr) const
    {
        DiffResult r;
        Check(prev.error,      curr.error,      CapabilityKind::kError,      r);
        Check(prev.job,        curr.job,        CapabilityKind::kJob,        r);
        Check(prev.env,        curr.env,        CapabilityKind::kEnv,        r);
        Check(prev.maint,      curr.maint,      CapabilityKind::kMaint,      r);
        Check(prev.health,     curr.health,     CapabilityKind::kHealth,     r);
        Check(prev.safety,     curr.safety,     CapabilityKind::kSafety,     r);
        Check(prev.consumable, curr.consumable, CapabilityKind::kConsumable, r);
        Check(prev.print,      curr.print,      CapabilityKind::kPrint,      r);
        return r;
    }

private:

    template <typename Opt>
    static void Check(const Opt& prev, const Opt& curr, CapabilityKind kind, DiffResult& r)
    {
        if (!(prev == curr)) {
            r.changed = true;
            r.changedKinds |= KindBit(kind);
        }
    }
};

} // namespace rim
