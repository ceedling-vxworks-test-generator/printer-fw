#pragma once

#include "ProductDefinition.hpp"

#include "CapabilityStore.hpp"
#include "CapabilityDependencyMap.hpp"
#include "CapabilityChangeSet.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class CapabilityManager
{
public:

    explicit CapabilityManager(
        CapabilityStore& store,
        const ProductDefinition& product)
        : store_(store)
        , dependencyMap_(product)
        , product_(product)
    {
    }

    CapabilityChangeSet Evaluate(
        const RIMSnapshot& snapshot,
        RIDataId changedDataId);

private:

    CapabilityStore& store_;

    CapabilityDependencyMap dependencyMap_;

    const ProductDefinition& product_;
};

} // namespace rim
