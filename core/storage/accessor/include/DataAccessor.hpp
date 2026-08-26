#pragma once

#include <string_view>
#include <vector>

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
        RIDataId id,
        RIMDataItem& item) const;

    bool
    TryGetDomain(
        RIDataId id,
        std::string_view& domain) const;

    std::vector<std::string_view>
    GetDomains() const;

    bool GetBinaryHash(
        RIDataId id,
        std::uint64_t& hash) const;

    bool GetBinaryInfo(
        RIDataId id,
        BinaryInfo& info) const;

private:

    const PartitionStorageRegistry&
        store_;

    const ProductContext&
        context_;

};

}