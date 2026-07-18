#pragma once

#include "capability/MachineCapabilityStore.hpp"

#include "datastore/RIMSnapshot.hpp"

namespace rim
{

//
// 機種可変の Capability 判定ルール集合の抽象。
// framework(共通)側は本インターフェース越しにのみ Rule を呼び、
// 具象 Rule(閾値・判定ロジック)は devices 側が実装する。
//
class ICapabilityRuleSet
{
public:

    virtual ~ICapabilityRuleSet() = default;

    virtual void Evaluate(
        const RIMSnapshot& snapshot,
        MachineCapabilityStore& store) const = 0;
};

} // namespace rim
