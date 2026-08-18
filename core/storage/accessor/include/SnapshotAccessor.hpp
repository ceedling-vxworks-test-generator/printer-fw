#pragma once

#include <string_view>
#include <vector>

#include "AccessId.hpp"
#include "RIMSnapshotManager.hpp"
#include "SnapshotStorage.hpp"
#include "ProductDefinition.hpp"

#include "CapabilityDependencyMap.hpp"
#include "CapabilityDomainResolver.hpp"
#include "SnapshotBuilder.hpp"
#include "DomainStorageRegistry.hpp"

namespace rim
{

struct SnapshotResult
{
    AccessId accessId{};

    RIMSnapshot snapshot{};
};

class SnapshotAccessor
{
public:

    SnapshotAccessor(
        const DomainStorageRegistry& store,
        SnapshotStorage& storage,
        const ProductDefinition& product)
        :
        store_(store),
        storage_(storage),
        product_(product),
        dependencyMap_(product),
        domainResolver_(product),
        snapshotBuilder_(
            store,
            product)
    {
    }

    SnapshotResult
    CreateSnapshot(
        RIDataId changedDataId);

    SnapshotResult
    CreateSnapshot(
        const std::vector<RIDataId>& changedDataIds);

    bool
    TryGetSnapshot(
        AccessId id,
        RIMSnapshot& snapshot) const;

private:

    SnapshotStorage&
        storage_;

    const ProductDefinition&
        product_;

    const DomainStorageRegistry&
        store_;

    CapabilityDependencyMap
        dependencyMap_;

    CapabilityDomainResolver
        domainResolver_;

    SnapshotBuilder
        snapshotBuilder_;
};

} // namespace rim