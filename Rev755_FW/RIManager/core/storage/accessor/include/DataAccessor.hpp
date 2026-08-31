#pragma once

#include <string_view>
#include <vector>

#include "RIMId.hpp"
#include "RIMDataItem.hpp"
#include "PartitionStorageRegistry.hpp"
#include "BinaryInfo.hpp"
#include "ProductContext.hpp"

namespace rim
{

class DataAccessor
{
public:
    DataAccessor(
        const PartitionStorageRegistry& store,
        const ProductContext& context)
        :
    store_(store),
    context_(context)
    {
    }

    bool
    TryGetData(
        const RIMId& id,
        RIMDataItem& item) const;

    bool
    TryGetDomain(
        const RIMId& id,
        std::string_view& domain) const;

    bool
    GetBinaryHash(
        const RIMId& id,
        std::uint64_t& hash) const;

    bool
    GetBinaryInfo(
        const RIMId& id,
        BinaryInfo& info) const;

    std::vector<std::string_view>
    GetDomains() const;

private:

    const PartitionStorageRegistry&
        store_;

    const ProductContext&
        context_;

};

}