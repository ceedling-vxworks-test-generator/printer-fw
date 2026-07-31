#pragma once

#include "EnvironmentRule.hpp"
#include "ErrorRule.hpp"
#include "PrintReadyRule.hpp"
#include "ConsumableRule.hpp"
#include "JobRule.hpp"

#include "MachineCapabilityStore.hpp"

#include "RIMSnapshot.hpp"
#include "ErrorRepository.hpp"

namespace rim
{

class CapabilityManager
{
public:

    explicit CapabilityManager(
        MachineCapabilityStore& store,
        ErrorRepository& repository)
        : store_(store)
        , errorRepository_(repository)
    {
    }

    void Evaluate(
        const RIMSnapshot& snapshot);

private:

    ErrorRepository& errorRepository_;

    MachineCapabilityStore& store_;

    EnvironmentRule environmentRule_;

    ErrorRule errorRule_;

    PrintReadyRule printReadyRule_;

    ConsumableRule consumableRule_;

    JobRule jobRule_;
};

} // namespace rim