#pragma once

#include <string_view>
#include <vector>

#include "rim_data_id.h"

#include "ProductDefinition.hpp"
#include "RIMDataItem.hpp"
#include "ValueStore.hpp"

namespace rim
{

class DataAccessor
{
public:

    DataAccessor(
        const ValueStore& store,
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

    const ValueStore& store_;

    const ProductDefinition& product_;
};

} // namespace rim
