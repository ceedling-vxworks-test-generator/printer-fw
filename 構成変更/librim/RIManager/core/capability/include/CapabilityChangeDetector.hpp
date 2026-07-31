#pragma once

#include "MachineCapabilityStore.hpp"
#include "CapabilityDiffChecker.hpp"
#include "CapabilityChangeSet.hpp"

namespace rim
{

class CapabilityChangeDetector
{
public:

CapabilityChangeSet Detect(
    const MachineCapabilityStore& store);

private:

    EnvironmentCapability previousEnvironment_{};

    ErrorCapability previousError_{};

    PrintReadyCapability previousPrintReady_{};

    ConsumableCapability previousConsumable_{};

    JobCapability previousJob_{};
};

} // namespace rim