#pragma once

#include <string_view>
#include <vector>
#include <unordered_map>

#include "RIMId.hpp"
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
    CapabilityRequiredDomains() const;

    const FacadeRequiredCapabilityMap&
    FacadeRequiredCapabilities() const;

    const CapabilityDependencyMap&
    AffectedCapabilities() const;

    std::vector<std::string_view>
    GetDomains() const;

    const RouteDefinition*
    GetRoutes() const;

    std::size_t
    GetRouteCount() const;

    const void*
    FindItem(
        const RIMId& id) const;

    const DataItemDefinition*
    FindDataItem(
        const RIMId& id) const;

    const CapabilityItemDefinition*
    FindCapability(
        const RIMId& id) const;

    const FacadeItemDefinition*
    FindFacade(
        const RIMId& id) const;

    DomainId
    FindDomainId(
        const RIMId& id) const;

    const DataItemDefinition*
    GetDataItems() const;

    std::size_t
    GetDataItemCount() const;

    const CapabilityItemDefinition*
    GetCapabilities() const;

    std::size_t
    GetCapabilityCount() const;

    const RouteDefinition*
    FindRoute(
        std::uint32_t routeId) const;

    const FacadeDependencyMap&
    AffectedFacades() const;

    std::vector<DomainId>
    GetCapabilityDomainIds() const;

    std::vector<DomainId>
    GetFacadeDomainIds() const;

private:

    DomainId
    FindDataDomainId(
        const RIMId& id) const;

    DomainId
    FindCapabilityDomainId(
        RIMId id) const;

    DomainId
    FindFacadeDomainId(
        RIMId id) const;

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