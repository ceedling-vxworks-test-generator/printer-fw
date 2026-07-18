#pragma once

#include "capability/ICapabilityRuleSet.hpp"

//
// TODO: この機種の判定 Rule を include する。
// #include "capability/EnvironmentRule.hpp"
// #include "capability/ErrorRule.hpp"
// #include "capability/PrintReadyRule.hpp"
//

namespace rim
{

//
// TODO: Skeleton を機種名(例 PrinterB)に置換。
// framework の CapabilityManager へ注入する Capability 判定集合。
// 機種ごとの閾値・判定順序はここで組み立てる。
//
class SkeletonCapabilityRuleSet final
    : public ICapabilityRuleSet
{
public:

    void Evaluate(
        const RIMSnapshot& snapshot,
        MachineCapabilityStore& store) const override
    {
        //
        // TODO: この機種の Rule を順に評価して store.Store(...) する。
        // 例:
        //   store.Store(environmentRule_.Evaluate(snapshot));
        //   store.Store(errorRule_.Evaluate(snapshot));
        //   store.Store(printReadyRule_.Evaluate(snapshot));
        //
        (void)snapshot;
        (void)store;
    }

private:

    // TODO: 機種別の Rule メンバを保持する。
    // EnvironmentRule environmentRule_;
    // ErrorRule       errorRule_;
    // PrintReadyRule  printReadyRule_;
};

} // namespace rim
