#pragma once

#include <vector>

#include "RIMSnapshot.hpp"
#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "ProductContext.hpp"

namespace rim
{

class SnapshotBuilder
{
public:

    SnapshotBuilder(
        const PartitionStorageRegistry& store);

    RIMSnapshot
    Build(
        const std::vector<DomainId>& domains) const;

private:

    const PartitionStorageRegistry&
        store_;
};

} // namespace rim