#pragma once

#include "MachineCapabilityStore.hpp"
#include "ICapabilityRuleSet.hpp"

#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class CapabilityManager
{
public:

    CapabilityManager(
        MachineCapabilityStore& store,
        const ICapabilityRuleSet& rules)
        : store_(store)
        , rules_(rules)
    {
    }

    void Evaluate(
        const RIMSnapshot& snapshot);

private:

    MachineCapabilityStore& store_;

    const ICapabilityRuleSet& rules_;
};

} // namespace rim
