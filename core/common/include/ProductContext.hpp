#pragma once

#include <string_view>
#include <vector>
#include <unordered_map>

#include "ProductDefinition.hpp"
#include "DataDomainMap.hpp"
#include "CapabilityRequiredDomainMap.hpp"
#include "FacadeRequiredCapabilityMap.hpp"
#include "CapabilityDependencyMap.hpp"
#include "DomainDefinition.hpp"
#include "DataItemDefinition.hpp"
#include "CapabilityItemDefinition.hpp"
#include "FacadeItemDefinition.hpp"
#include "RouteDefinition.hpp"
#include "NotificationTarget.hpp"
#include "FacadeItemDefinition.hpp"
#include "FacadeDependencyMap.hpp"

namespace rim
{

class ProductContext
{
public:

    explicit ProductContext(
        const ProductDefinition& product);

    const DataDomainMap&
    DataDomains() const;

    const CapabilityRequiredDomainMap&
    CapabilityDomains() const;

    const FacadeRequiredCapabilityMap&
    FacadeCapabilities() const;

    const CapabilityDependencyMap&
    CapabilityDependencies() const;

    const DataItemDefinition*
    FindDataItem(
        RIDataId id) const;

    std::vector<std::string_view>
    GetDomains() const;

    const RouteDefinition*
    GetRoutes() const;

    std::size_t
    GetRouteCount() const;

    const DataItemDefinition*
    GetDataItems() const;

    std::size_t
    GetDataItemCount() const;

    const CapabilityItemDefinition*
    GetCapabilities() const;

    std::size_t
    GetCapabilityCount() const;

    const CapabilityItemDefinition*
    FindCapability(
        RICapabilityId id) const;

    DomainId
    FindDataDomainId(
        RIDataId id) const;

    DomainId
    FindCapabilityDomainId(
        RICapabilityId id) const;

    const FacadeItemDefinition*
    FindFacade(
        RIFacadeId id) const;

    DomainId
    FindFacadeDomainId(
        RIFacadeId id) const;

    const RouteDefinition*
    FindRoute(
        std::uint32_t routeId) const;

    const RouteDefinition*
    ResolveRoute(
        const NotificationTarget& target) const;

    const FacadeDependencyMap&
    FacadeDependencies() const;

private:

    const ProductDefinition&
        product_;

    std::vector<std::string_view>
        domains_;

    std::vector<const RouteDefinition*>
        routes_;

    std::vector<const DataItemDefinition*>
        dataItems_;

    std::vector<const CapabilityItemDefinition*>
        capabilities_;

    DataDomainMap
        dataDomainMap_;

    CapabilityRequiredDomainMap
        capabilityRequiredDomainMap_;

    FacadeRequiredCapabilityMap
        facadeRequiredCapabilityMap_;

    CapabilityDependencyMap
        capabilityDependencyMap_;
    
    FacadeDependencyMap
        facadeDependencyMap_;

    std::unordered_map<
        RIDataId,
        const DataItemDefinition*>
            dataItemsByIdMap_;

    std::unordered_map<
        RICapabilityId,
        const CapabilityItemDefinition*>
            capabilitiesByIdMap_;

    std::unordered_map<
        RICapabilityId,
        DomainId>
            capabilityDomainsByIdMap_;

    std::vector<
        const FacadeItemDefinition*>
            facades_;

    std::unordered_map<
        RIFacadeId,
        const FacadeItemDefinition*>
            facadesByIdMap_;

    std::unordered_map<
        RIFacadeId,
        DomainId>
            facadeDomainsByIdMap_;

    std::unordered_map<
    std::uint32_t,
    const RouteDefinition*>
        routesByIdMap_;
};

}