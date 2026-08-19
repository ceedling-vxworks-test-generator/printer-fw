#pragma once

#include <string_view>
#include <vector>

#include "ProductDefinition.hpp"
#include "RIMDataItem.hpp"
#include "DomainStorageRegistry.hpp"

namespace rim
{

class DataAccessor
{
public:

    DataAccessor(
        const DomainStorageRegistry& store,
        const ProductDefinition& product)
        :
        store_(store),
        product_(product)
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

    bool
    TryGetDomain(
        std::string_view dataItemName,
        std::string_view& domain) const;

    std::vector<std::string_view>
    GetDomains() const;

    const ProductDefinition&
    GetProductDefinition() const
    {
        return product_;
    }

private:

    const DomainStorageRegistry&
        store_;

    const ProductDefinition&
        product_;

};

}