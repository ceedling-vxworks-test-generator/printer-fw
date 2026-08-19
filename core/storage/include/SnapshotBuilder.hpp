#pragma once

#include <vector>

#include "ProductDefinition.hpp"
#include "RIMSnapshot.hpp"
#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"

namespace rim
{

class SnapshotBuilder
{
public:

    SnapshotBuilder(
        const DomainStorageRegistry& store,
        const ProductDefinition& product);

    RIMSnapshot
    Build(
        const std::vector<DomainId>& domains) const;

private:

    const DomainStorageRegistry&
        store_;

    const ProductDefinition&
        product_;

    DataDomainMap
        domainMap_;
};

} // namespace rim