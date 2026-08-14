#include "DataAccessor.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"

namespace rim
{

bool
DataAccessor::TryGetData(
    RIDataId id,
    RIMDataItem& item) const
{
    return store_.Find(
        id,
        item);
}

bool
DataAccessor::TryGetDomain(
    RIDataId id,
    std::string_view& domain) const
{
    const auto* definition =
        rim::FindDataItem(
            product_,
            id);

    if (definition == nullptr)
    {
        return false;
    }

    domain =
        definition->domain;

    return true;
}

std::vector<std::string_view>
DataAccessor::GetDomains() const
{
    std::vector<std::string_view>
        result;

    const auto* domains =
        rim::GetDomains(
            product_);

    const auto count =
        rim::GetDomainCount(
            product_);

    result.reserve(
        count);

    for (std::size_t i = 0;
         i < count;
         ++i)
    {
        result.push_back(
            domains[i].name);
    }

    return result;
}

bool
DataAccessor::TryGetDomain(
    std::string_view dataItemName,
    std::string_view& domain) const
{
    const auto* definition =
        rim::FindDataItem(
            product_,
            dataItemName);

    if (definition == nullptr)
    {
        return false;
    }

    domain =
        definition->domain;

    return true;
}

} // namespace rim