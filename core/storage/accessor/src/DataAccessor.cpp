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
    const DomainId domainId =
        context_
            .FindDataDomainId(
                id);

    if (domainId ==
        kInvalidDomainId)
    {
        return false;
    }

    const auto* storage =
        store_.Find(
            domainId);

    if (storage == nullptr)
    {
        return false;
    }

    return storage->Find(
        id,
        item);
}

bool
DataAccessor::TryGetDomain(
    RIDataId id,
    std::string_view& domain) const
{
    const auto* definition =
        context_
        .FindDataItem(
            id);

    if (definition == nullptr)
    {
        return false;
    }

    domain =
        definition->domain->name;

    return true;
}

std::vector<std::string_view>
DataAccessor::GetDomains() const
{
    return context_.GetDomains();
}

bool
DataAccessor::GetBinaryHash(
    RIDataId id,
    std::uint64_t& hash) const
{
    const DomainId domainId =
        context_
            .FindDataDomainId(
                id);

    if (domainId ==
        kInvalidDomainId)
    {
        return false;
    }

    const auto* storage =
        store_.Find(
            domainId);

    if (storage == nullptr)
    {
        return false;
    }

    return storage->GetBinaryHash(
        id,
        hash);
}

bool
DataAccessor::GetBinaryInfo(
    RIDataId id,
    BinaryInfo& info) const
{
    const DomainId domainId =
        context_
            .FindDataDomainId(
                id);

    if (domainId ==
        kInvalidDomainId)
    {
        return false;
    }

    const auto* storage =
        store_.Find(domainId);

    if (storage == nullptr)
    {
        return false;
    }

    return storage->GetBinaryInfo(
        id,
        info);
}

}