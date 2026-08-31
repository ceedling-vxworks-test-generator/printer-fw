#pragma once

#include "PartitionStorageRegistry.hpp"
#include "CapabilityDependencyMap.hpp"
#include "CapabilityChangeSet.hpp"
#include "RIMSnapshot.hpp"
#include "ProductContext.hpp"

namespace rim
{

class CapabilityManager
{
public:

    CapabilityManager(
        PartitionStorageRegistry& store,
        const ProductContext& context)
        : store_(store)
        , context_(context)
        , dependencyMap_(
            context.AffectedCapabilities())
    {
    }

    bool Evaluate(
        const RIMSnapshot& snapshot,
        const CapabilityItemDefinition* capability);

private:

    PartitionStorageRegistry& store_;

    const CapabilityDependencyMap&
        dependencyMap_;

    const ProductContext&
        context_;

};

} // namespace rim
