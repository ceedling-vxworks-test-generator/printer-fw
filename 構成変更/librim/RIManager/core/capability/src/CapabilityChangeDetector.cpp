#include "CapabilityChangeDetector.hpp"

namespace rim
{

CapabilityChangeSet
CapabilityChangeDetector::Detect(
    const MachineCapabilityStore& store)
{
    CapabilityChangeSet changes;

    const auto& environment =
        store.GetEnvironment();

    const auto& error =
        store.GetError();

    const auto& printReady =
        store.GetPrintReady();

    const auto& consumable =
        store.GetConsumable();

    const auto& job =
        store.GetJob();

    if (CapabilityDiffChecker::HasChanged(
            previousEnvironment_,
            environment))
    {
        changes.changedCapabilities.push_back(
            "Environment");
    }

    if (CapabilityDiffChecker::HasChanged(
            previousError_,
            error))
    {
        changes.changedCapabilities.push_back(
            "Error");
    }

    if (CapabilityDiffChecker::HasChanged(
            previousPrintReady_,
            printReady))
    {
        changes.changedCapabilities.push_back(
            "PrintReady");
    }

    if (CapabilityDiffChecker::HasChanged(
            previousConsumable_,
            consumable))
    {
        changes.changedCapabilities.push_back(
            "Consumable");
    }

    if (CapabilityDiffChecker::HasChanged(
            previousJob_,
            job))
    {
        changes.changedCapabilities.push_back(
            "Job");
    }

    previousEnvironment_ = environment;
    previousError_ = error;
    previousPrintReady_ = printReady;
    previousConsumable_ = consumable;
    previousJob_ = job;

    return changes;
}

} // namespace rim