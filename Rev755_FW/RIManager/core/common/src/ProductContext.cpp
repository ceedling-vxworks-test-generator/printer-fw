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
// | `dataDomainMap_`                 | Lookup      | `RIDataId`       | `DomainId`                   | `FindDataDomainId(RIDataId)`          |
// | `capabilityRequiredDomainMap_`   | Lookup      | `RICapabilityId` | `DomainId一覧`               | `CapabilityRequiredDomains()`                 |
// | `facadeRequiredCapabilityMap_`   | Lookup      | `RIFacadeId`     | `CapabilityId一覧`           | `FacadeRequiredCapabilities()`                |
// | `capabilityDependencyMap_`       | Lookup      | `RIDataId`       | `CapabilityId一覧`           | `AffectedCapabilities()`            |
// | `facadeDependencyMap_`           | Lookup      | `RICapabilityId` | `FacadeId一覧`               | `AffectedFacades()`                |
// | `facadeDomainsByIdMap_`        | `RIFacadeId`     | `DomainId`                     | `FindFacadeDomainId()`            |
// | `capabilityRequiredDomainMap_` | `RICapabilityId` | `DomainId一覧`                 | `CapabilityRequiredDomains()`             |
// | `facadeRequiredCapabilityMap_` | `RIFacadeId`     | `RICapabilityId一覧`           | `FacadeRequiredCapabilities()`            |
// | `capabilityDependencyMap_`     | `RIDataId`       | `CapabilityDefinition一覧`     | `AffectedCapabilities()`        |
// | `facadeDependencyMap_`         | `RICapabilityId` | `FacadeDefinition一覧`         | `AffectedFacades()`            |

// | `dataItems_`                     | Enumeration | なし             | 全`DataItemDefinition`       | `GetDataItems()` `GetDataItemCount()` |
// | `capabilities_`                  | Enumeration | なし             | 全`CapabilityItemDefinition` | `GetCapabilities()` `GetCapabilityCount()` |
// | `routes_`                        | Enumeration | なし             | 全`RouteDefinition`          | `GetRoutes()` `GetRouteCount()`        |
// | `domains_`                       | Enumeration | なし             | 全Domain名                   | `GetDomains()`                         |
// | `dataItems_`      | 全DataItemDefinition            | `GetDataItems()`                      |
// | `capabilities_`   | 全CapabilityItemDefinition      | `GetCapabilities()`                   |
// | `facades_`        | 全FacadeItemDefinition          | `FindFacade()` `GetFacadeDomainIds()` |
// | `routes_`         | 全RouteDefinition               | `GetRoutes()`                         |
// | `domains_`        | 全Domain名                      | `GetDomains()`                        |

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
ProductContext::CapabilityRequiredDomains() const
{
    return capabilityRequiredDomainMap_;
}

const FacadeRequiredCapabilityMap&
ProductContext::FacadeRequiredCapabilities() const
{
    return facadeRequiredCapabilityMap_;
}

const CapabilityDependencyMap&
ProductContext::AffectedCapabilities() const
{
    return capabilityDependencyMap_;
}

const FacadeDependencyMap&
ProductContext::AffectedFacades() const
{
    return facadeDependencyMap_;
}

const DataItemDefinition*
ProductContext::FindDataItem(
    const RIMId& id) const
{
    if (id.type !=
        RIMIdType::Data)
    {
        return nullptr;
    }

    const auto it =
        dataItemsByIdMap_.find(
            static_cast<RIDataId>(
                id.id));

    return it ==
        dataItemsByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindCapabilityDomainId(
    RIMId id) const
{
    if (id.type !=
        RIMIdType::Capability)
    {
        return kInvalidDomainId;
    }

    const auto it =
        capabilityDomainsByIdMap_.find(
            static_cast<RICapabilityId>(
                id.id));

    if (it ==
        capabilityDomainsByIdMap_.end())
    {
        return kInvalidDomainId;
    }

    return
        it->second;
}

const FacadeItemDefinition*
ProductContext::FindFacade(
    const RIMId& id) const
{
    if (id.type !=
        RIMIdType::Facade)
    {
        return nullptr;
    }

    const auto it =
        facadesByIdMap_.find(
            static_cast<RIFacadeId>(
                id.id));

    return it ==
        facadesByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindFacadeDomainId(
    RIMId id) const
{
    if (id.type !=
        RIMIdType::Facade)
    {
        return kInvalidDomainId;
    }

    const auto it =
        facadeDomainsByIdMap_.find(
            static_cast<RIFacadeId>(
                id.id));

    if (it ==
        facadeDomainsByIdMap_.end())
    {
        return kInvalidDomainId;
    }

    return
        it->second;
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
ProductContext::FindCapability(
    const RIMId& id) const
{
    if (id.type !=
        RIMIdType::Capability)
    {
        return nullptr;
    }

    const auto it =
        capabilitiesByIdMap_.find(
            static_cast<RICapabilityId>(
                id.id));

    return it ==
        capabilitiesByIdMap_.end()
        ? nullptr
        : it->second;
}

DomainId
ProductContext::FindDataDomainId(
    const RIMId& id) const
{
    return dataDomainMap_.Find(
        id);
}

DomainId
ProductContext::FindDomainId(
    const RIMId& id) const
{
    switch (id.type)
    {
    case RIMIdType::Data:
        return FindDataDomainId(id);

    case RIMIdType::Capability:
        return FindCapabilityDomainId(id);

    case RIMIdType::Facade:
        return FindFacadeDomainId(id);

    default:
        return kInvalidDomainId;
    }
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

std::vector<DomainId>
ProductContext::GetCapabilityDomainIds() const
{
    std::vector<DomainId>
        result;

    result.reserve(
        capabilityDomainsByIdMap_.size());

    for (const auto& [id, domainId]
         : capabilityDomainsByIdMap_)
    {
        result.push_back(
            domainId);
    }

    return result;
}

std::vector<DomainId>
ProductContext::GetFacadeDomainIds() const
{
    std::vector<DomainId>
        result;

    result.reserve(
        facadeDomainsByIdMap_.size());

    for (const auto& [id, domainId]
         : facadeDomainsByIdMap_)
    {
        result.push_back(
            domainId);
    }

    return result;
}

}