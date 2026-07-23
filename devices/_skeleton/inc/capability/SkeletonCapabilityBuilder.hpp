#pragma once

//
// SkeletonCapabilityBuilder - 新機種テンプレ。ICapabilityBuilder 実装。
// TODO: Snapshot から8種Capabilityを生成する判定を実装する。
//

#include "capability/ICapabilityBuilder.hpp"

namespace rim
{

class SkeletonCapabilityBuilder final
    : public ICapabilityBuilder
{
public:

    CapabilitySet Build(const MachineSnapshot& snapshot) const override
    {
        // TODO: この機種の判定式で CapabilitySet を組み立てる。
        (void)snapshot;
        return CapabilitySet{};
    }
};

} // namespace rim
