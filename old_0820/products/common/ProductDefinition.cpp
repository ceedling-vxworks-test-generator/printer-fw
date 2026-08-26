#include "ProductDefinition.hpp"

#include "DomainDefinition.hpp"
#include "RouteDefinition.hpp"
#include "DataItemDefinition.hpp"
#include "CapabilityItemDefinition.hpp"

namespace
{

template<typename T, typename Getter>
bool HasDuplicate(
    const T* items,
    std::size_t count,
    Getter getter)
{
    if (items == nullptr)
    {
        return false;
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        for (std::size_t j = i + 1;
             j < count;
             ++j)
        {
            if (getter(items[i]) ==
                getter(items[j]))
            {
                return true;
            }
        }
    }

    return false;
}

} // namespace

namespace rim
{

const DomainDefinition*
FindDomain(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.domainCount; ++i)
    {
        const auto& domain = product.domains[i];

        if (domain.name == name)
        {
            return &domain;
        }
    }

    return nullptr;
}

const RouteDefinition*
FindRoute(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.routeCount; ++i)
    {
        const auto& route = product.routes[i];

        if (route.name == name)
        {
            return &route;
        }
    }

    return nullptr;
}

const DataItemDefinition*
FindDataItem(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.dataItemCount; ++i)
    {
        const auto& item = product.dataItems[i];

        if (item.name == name)
        {
            return &item;
        }
    }

    return nullptr;
}

const DataItemDefinition*
FindDataItem(
    const ProductDefinition& product,
    RIDataId id)
{
    for (std::size_t i{}; i < product.dataItemCount; ++i)
    {
        if (product.dataItems[i].id == id)
        {
            return &product.dataItems[i];
        }
    }

    return nullptr;
}

const CapabilityItemDefinition*
FindCapability(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.capabilityCount; ++i)
    {
        const auto& capability = product.capabilities[i];

        if (capability.name == name)
        {
            return &capability;
        }
    }

    return nullptr;
}

const CapabilityItemDefinition*
FindCapability(
    const ProductDefinition& product,
    RICapabilityId id)
{
    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        if (product.capabilities[i].id == id)
        {
            return &product.capabilities[i];
        }
    }

    return nullptr;
}

bool ValidateProductDefinition(
    const ProductDefinition& product)
{
    //
    // Domain uniqueness
    //

    if (HasDuplicate(
            product.domains,
            product.domainCount,
            [&](const DomainDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }
    for (std::size_t i = 0;
        i < product.dataItemCount;
        ++i)
    {
        const auto* domain =
            product.dataItems[i].domain;

        if (domain == nullptr)
        {
            return false;
        }

        if (!FindDomain(
                product,
                domain->name))
        {
            return false;
        }
    }
    
    //
    // DataItem name uniqueness
    //

    if (HasDuplicate(
            product.dataItems,
            product.dataItemCount,
            [&](const DataItemDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // DataItem id uniqueness
    //

    if (HasDuplicate(
            product.dataItems,
            product.dataItemCount,
            [&](const DataItemDefinition& item)
            {
                return item.id;
            }))
    {
        return false;
    }

    //
    // Route uniqueness
    //

    if (HasDuplicate(
            product.routes,
            product.routeCount,
            [&](const RouteDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // DataItem -> Route
    //

    for (std::size_t i = 0;
        i < product.dataItemCount;
        ++i)
    {
        if (!FindRoute(
                product,
                product.dataItems[i].route))
        {
            return false;
        }
    }

    //
    // Capability uniqueness
    //

    if (HasDuplicate(
            product.capabilities,
            product.capabilityCount,
            [&](const CapabilityItemDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // Capability id uniqueness
    //

    if (HasDuplicate(
            product.capabilities,
            product.capabilityCount,
            [&](const CapabilityItemDefinition& item)
            {
                return item.id;
            }))
    {
        return false;
    }

    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        const auto& capability =
            product.capabilities[i];

        for (std::size_t j = 0;
             j < capability.requiredDataCount;
             ++j)
        {
            const auto dataId =
                capability.requiredDataIds[j];

            if (!FindDataItem(
                    product,
                    dataId))
            {
                return false;
            }
        }
    }

    return true;
}

const RouteDefinition*
GetRoutes(
    const ProductDefinition& product)
{
    return product.routes;
}

std::size_t
GetRouteCount(
    const ProductDefinition& product)
{
    return product.routeCount;
}

const CapabilityItemDefinition*
GetCapabilities(
    const ProductDefinition& product)
{
    return product.capabilities;
}

std::size_t
GetCapabilityCount(
    const ProductDefinition& product)
{
    return product.capabilityCount;
}

const DataItemDefinition*
GetDataItems(
    const ProductDefinition& product)
{
    return product.dataItems;
}

std::size_t
GetDataItemCount(
    const ProductDefinition& product)
{
    return product.dataItemCount;
}

const DomainDefinition*
GetDomains(
    const ProductDefinition& product)
{
    return product.domains;
}

std::size_t
GetDomainCount(
    const ProductDefinition& product)
{
    return product.domainCount;
}


} // namespace rim