#pragma once

#include <vector>

#include "DomainId.hpp"
#include "ProductDefinition.hpp"
#include "RIMSnapshot.hpp"
#include "SnapshotBuilder.hpp"
#include "DomainStorageRegistry.hpp"

namespace rim
{

class SnapshotAccessor
{
public:

    SnapshotAccessor(
        const DomainStorageRegistry& store,
        const ProductDefinition& product)
        :
        store_(store),
        snapshotBuilder_(
            store,
            product)
    {
    }

    RIMSnapshot
    CreateSnapshot(
        const std::vector<DomainId>& domains) const;

private:

    const DomainStorageRegistry&
        store_;

    SnapshotBuilder
        snapshotBuilder_;
};

} // namespace rim
