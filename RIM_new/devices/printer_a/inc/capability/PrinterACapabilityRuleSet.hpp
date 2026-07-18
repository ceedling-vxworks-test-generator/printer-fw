#pragma once

#include "capability/ICapabilityRuleSet.hpp"

#include "capability/EnvironmentRule.hpp"
#include "capability/ErrorRule.hpp"
#include "capability/PrintReadyRule.hpp"

namespace rim
{

//
// PrinterA 用の Capability 判定ルール集合。
// framework の CapabilityManager へ注入して使う。
// 判定順序(Environment→Error→PrintReady)は従来仕様を温存。
//
class PrinterACapabilityRuleSet final
    : public ICapabilityRuleSet
{
public:

    void Evaluate(
        const RIMSnapshot& snapshot,
        MachineCapabilityStore& store) const override
    {
        store.Store(
            environmentRule_.Evaluate(
                snapshot));

        store.Store(
            errorRule_.Evaluate(
                snapshot));

        store.Store(
            printReadyRule_.Evaluate(
                snapshot));
    }

private:

    EnvironmentRule environmentRule_;

    ErrorRule errorRule_;

    PrintReadyRule printReadyRule_;
};

} // namespace rim
