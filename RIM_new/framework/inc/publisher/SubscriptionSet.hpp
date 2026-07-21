#pragma once

//
// SubscriptionSet - 購読者の関心Capability(仕様 §9)。
// interested==0 は「全Capに関心」を意味する。
//

#include "capability/CapabilityKind.hpp"

namespace rim
{

struct SubscriptionSet
{
    CapabilityKindSet interested{0};  // 0 = 全関心

    bool IsInterested(CapabilityKindSet changedKinds) const
    {
        if (interested == 0) return true;
        return (interested & changedKinds) != 0;
    }
};

} // namespace rim
