#include "DataAccessor.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"

namespace rim
{

bool
DataAccessor::TryGetData(
    const RIMId& id,
    RIMDataItem& item) const
{
    if (id.type !=
        RIMIdType::Data)
    {
        return false;
    }

    const DomainId domainId =
        context_.FindDomainId(
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
        id.id,
        item);
}

bool
DataAccessor::TryGetDomain(
    const RIMId& id,
    std::string_view& domain) const
{
    if (id.type !=
        RIMIdType::Data)
    {
        return false;
    }

    const auto* definition =
        context_.FindDataItem(
            id);

    if (definition == nullptr)
    {
        return false;
    }

    domain =
        definition->domain->name;

    return true;
}

bool
DataAccessor::GetBinaryHash(
    const RIMId& id,
    std::uint64_t& hash) const
{
    if (id.type !=
        RIMIdType::Data)
    {
        return false;
    }

    const DomainId domainId =
        context_.FindDomainId(
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
        id.id,
        hash);
}

bool
DataAccessor::GetBinaryInfo(
    const RIMId& id,
    BinaryInfo& info) const
{
    if (id.type !=
        RIMIdType::Data)
    {
        return false;
    }

    const DomainId domainId =
        context_.FindDomainId(
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

    return storage->GetBinaryInfo(
        id.id,
        info);
}

std::vector<std::string_view>
DataAccessor::GetDomains() const
{
    return context_.GetDomains();
}

}