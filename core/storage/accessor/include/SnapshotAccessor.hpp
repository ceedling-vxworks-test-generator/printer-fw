#pragma once

#include <vector>

#include "DomainId.hpp"
#include "RIMSnapshot.hpp"
#include "SnapshotBuilder.hpp"
#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"

namespace rim
{

class SnapshotAccessor
{
public:

    SnapshotAccessor(
        const PartitionStorageRegistry& store,
        const ProductContext& context)
        :
        store_(store),
        snapshotBuilder_(
            store)
    {
    }

    RIMSnapshot
    CreateSnapshot(
        const std::vector<DomainId>& domains) const;

private:

    const PartitionStorageRegistry&
        store_;

    SnapshotBuilder
        snapshotBuilder_;
};

} // namespace rim
