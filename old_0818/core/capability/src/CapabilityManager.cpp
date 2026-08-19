#include "CapabilityManager.hpp"

#include <any>
#include <new>

#include "CapabilityItemDefinition.hpp"

namespace rim
{

CapabilityChangeSet
CapabilityManager::Evaluate(
    const RIMSnapshot& snapshot,
    RIDataId changedDataId)
{
    CapabilityChangeSet changes;

    const auto& capabilities =
        dependencyMap_.Find(
            changedDataId);

    for (const auto* capability
            : capabilities)
    {
        std::any newValue =
            capability->build(
                snapshot);

        bool changed = true;

        const auto* oldValue =
            store_.Find(
                capability->id);

        if (oldValue != nullptr)
        {
            changed =
                capability->compare(
                    *oldValue,
                    newValue);
        }

        if (!changed)
        {
            continue;
        }

        store_.Store(
            capability->id,
            std::move(
                newValue));

        changes.changedCapabilities
            .push_back(
                capability->id);
    }

    return changes;
}

} // namespace rim