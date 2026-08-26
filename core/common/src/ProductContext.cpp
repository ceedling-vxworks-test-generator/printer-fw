#include "ProductContext.hpp"

#include "FacadeItemDefinition.hpp"
#include "CapabilityItemDefinition.hpp"
#include "DataItemDefinition.hpp"

namespace rim
{

// | メンバ                           | 種別        | 入力(Key)        | 出力(Value)                  | 利用関数                                |
// | -------------------------------- | ----------- | ---------------- | ---------------------------- | ------------------------------------- |
// | `dataItemsByIdMap_`              | Lookup      | `RIDataId`       | `DataItemDefinition*`        | `FindDataItem(RIDataId)`              |
// | `capabilitiesByIdMap_`           | Lookup      | `RICapabilityId` | `CapabilityItemDefinition*`  | `FindCapability(RICapabilityId)`      |
// | `routesByIdMap_`                 | Lookup      | `RouteId`        | `RouteDefinition*`           | `FindRoute(RouteId)`                  |
// | `dataDomainMap_`                 | Lookup      | `RIDataId`       | `DomainId`                   | `FindDomainId(RIDataId)`              |
// | `capabilityRequiredDomainMap_`   | Lookup      | `RICapabilityId` | `DomainId一覧`               | `CapabilityDomains()`                 |
// | `facadeRequiredCapabilityMap_`   | Lookup      | `RIFacadeId`     | `CapabilityId一覧`           | `FacadeCapabilities()`                |
// | `capabilityDependencyMap_`       | Lookup      | `RIDataId`       | `CapabilityId一覧`           | `CapabilityDependencies()`            |
// | `facadeDependencyMap_`           | Lookup      | `RICapabilityId` | `FacadeId一覧`               | `FacadeDependencies()`                |

// | `dataItems_`                     | Enumeration | なし             | 全`DataItemDefinition`       | `GetDataItems()` `GetDataItemCount()` |
// | `capabilities_`                  | Enumeration | なし             | 全`CapabilityItemDefinition` | `GetCapabilities()` `GetCapabilityCount()` |
// | `routes_`                        | Enumeration | なし             | 全`RouteDefinition`          | `GetRoutes()` `GetRouteCount()`        |
// | `domains_`                       | Enumeration | なし             | 全Domain名                   | `GetDomains()`                         |

ProductContext::ProductContext(
    const ProductDefinition& product)
    :
    product_(product),
    dataDomainMap_(product),
    capabilityRequiredDomainMap_(product),
    facadeRequiredCapabilityMap_(product),
    capabilityDependencyMap_(product),
    facadeDependencyMap_(product)
{
    const auto* routes = rim::GetRoutes(product_);
    for (std::size_t i = 0; i < rim::GetRouteCount(product_); ++i)
    {
        const auto& route = routes[i];
        routesByIdMap_.emplace(route.id,&route);
        routes_.push_back(&route);
    }

    const auto* dataItems = rim::GetDataItems(product_);
    for (std::size_t i = 0; i < rim::GetDataItemCount(product_); ++i)
    {
        const auto& item = dataItems[i];
        dataItemsByIdMap_.emplace(item.id,&item);
        dataItems_.push_back(&item);
    }

    const auto* capabilities = rim::GetCapabilities(product_);
    for (std::size_t i = 0; i < rim::GetCapabilityCount(product_); ++i)
    {
        const auto& capability = capabilities[i];
        capabilitiesByIdMap_.emplace(capability.id,&capability);
        capabilities_.push_back(&capability);
    }


    const auto* domains = rim::GetDomains(product_);
    for (std::size_t i = 0; i < rim::GetDomainCount(product_); ++i)
    {
        domains_.push_back(domains[i].name);
    }

    DomainId nextDomainId =
    static_cast<DomainId>(
        domains_.size() + 1);

    for (std::size_t i = 0;
        i < rim::GetCapabilityCount(product_);
        ++i)
    {
        const auto& capability =
            capabilities[i];

        capabilityDomainsByIdMap_.emplace(
            capability.id,
            nextDomainId++);
    }

    const auto* facades = rim::GetFacades(product_);

    for (std::size_t i = 0;i < rim::GetFacadeCount(product_);++i)
    {
        const auto& facade = facades[i];
        facadesByIdMap_.emplace(facade.id,&facade);
        facades_.push_back(&facade);
    }

    for (std::size_t i = 0;
        i < product_.facadeCount;
        ++i)
    {
        const auto& facade =
            product_.facades[i];

        facadeDomainsByIdMap_.emplace(
            facade.id,
            nextDomainId++);
    }

}

const DataDomainMap&
ProductContext::DataDomains() const
{
    return dataDomainMap_;
}

const CapabilityRequiredDomainMap&
ProductContext::CapabilityDomains() const
{
    return capabilityRequiredDomainMap_;
}

const FacadeRequiredCapabilityMap&
ProductContext::FacadeCapabilities() const
{
    return facadeRequiredCapabilityMap_;
}

const CapabilityDependencyMap&
ProductContext::CapabilityDependencies() const
{
    return capabilityDependencyMap_;
}

const FacadeDependencyMap&
ProductContext::FacadeDependencies() const
{
    return facadeDependencyMap_;
}

const DataItemDefinition*
ProductContext::FindDataItem(RIDataId id) const
{
    auto it =
        dataItemsByIdMap_.find(id);

    return it ==
        dataItemsByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindCapabilityDomainId(
    RICapabilityId id) const
{
    const auto it =
        capabilityDomainsByIdMap_.find(id);

    return it ==
        capabilityDomainsByIdMap_.end()
        ? kInvalidDomainId
        : it->second;
}

const FacadeItemDefinition*
ProductContext::FindFacade(
    RIFacadeId id) const
{
    const auto it =
        facadesByIdMap_.find(id);

    return it ==
        facadesByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindFacadeDomainId(
    RIFacadeId id) const
{
    const auto it =
        facadeDomainsByIdMap_.find(id);

    return it ==
        facadeDomainsByIdMap_.end()
        ? kInvalidDomainId
        : it->second;
}

std::vector<std::string_view>
ProductContext::GetDomains() const
{
    return domains_;
}

const RouteDefinition*
ProductContext::GetRoutes() const
{
    return routes_.empty()
        ? nullptr
        : routes_.front();
}

std::size_t
ProductContext::GetRouteCount() const
{
    return routes_.size();
}

const DataItemDefinition*
ProductContext::GetDataItems() const
{
    return dataItems_.empty()
        ? nullptr
        : dataItems_.front();
}

std::size_t
ProductContext::GetDataItemCount() const
{
    return dataItems_.size();
}

const CapabilityItemDefinition*
ProductContext::GetCapabilities() const
{
    return capabilities_.empty()
        ? nullptr
        : capabilities_.front();
}

std::size_t
ProductContext::GetCapabilityCount() const
{
    return capabilities_.size();
}

const CapabilityItemDefinition*
ProductContext::FindCapability(RICapabilityId id) const
{
    auto it =
        capabilitiesByIdMap_.find(id);

    return it ==
        capabilitiesByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindDataDomainId(RIDataId id) const
{
    return dataDomainMap_.Find(
        id);
}

const RouteDefinition*
ProductContext::FindRoute(
    std::uint32_t routeId) const
{
    auto it =
        routesByIdMap_.find(routeId);

    return it ==
        routesByIdMap_.end()
        ? nullptr
        : it->second;
}

const RouteDefinition*
ProductContext::ResolveRoute(
    const NotificationTarget& target) const
{
    switch (target.type)
    {
    case NotificationTargetType::Data:
    {
        const auto* item =FindDataItem(static_cast<RIDataId>(target.id));

        if (item == nullptr)
        {
            return nullptr;
        }

        return FindRoute(item->routeId);
    }

    case NotificationTargetType::Capability:
    {
        const auto* capability =FindCapability(static_cast<RICapabilityId>(target.id));

        if (capability == nullptr)
        {
            return nullptr;
        }

        return FindRoute(capability->routeId);
    }

    case NotificationTargetType::Facade:
    {
        const auto* facade =FindFacade(static_cast<RIFacadeId>(target.id));

        if (facade == nullptr)
        {
            return nullptr;
        }

        return FindRoute(facade->routeId);
    }

    default:
        return nullptr;
    }
}

}