#include <iostream>
#include <cassert>
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"
#include "RouteDefinition.hpp"
#include "DataItemDefinition.hpp"
#include "CapabilityItemDefinition.hpp"

namespace rim
{

void RouteProvider::Initialize(const ProductDefinition& product)
{
    queues_.clear();
    routingTable_.clear();
    capabilityRouteTable_.clear();

    const auto* routes = GetRoutes(product);
    const auto* items = GetDataItems(product);

    for (std::size_t i = 0; i < GetRouteCount(product); ++i)
    {
        const auto& route = routes[i];
        auto storeQueue = QueueFactory::Create<RIMDataItem>(route.storePolicy);
        auto capabilityQueue = QueueFactory::Create<CapabilityInput>(route.capabilityPolicy);

        assert(storeQueue != nullptr);
        assert(capabilityQueue != nullptr);

        queues_.emplace(
            route.name,
            RouteQueues
            {
                &route,
                std::move(storeQueue),
                std::move(capabilityQueue)
            });
    }

    for (std::size_t i = 0; i < GetDataItemCount(product); ++i)
    {
        const auto& item = items[i];

        auto queueIt = queues_.find(item.route);

        assert(queueIt != queues_.end());

        routingTable_.emplace(item.id, &queueIt->second);
    }

    //capabilityRouteTable作成
    const auto* capabilities = GetCapabilities(product);

    for (std::size_t i = 0; i < GetCapabilityCount( product ); ++i)
    {
        const auto& capability = capabilities[i];

        auto* route = rim::FindRoute( product, capability.route );

        assert(route != nullptr);

        capabilityRouteTable_.emplace( capability.id, route );
    }
}

//DataID -> RouteQueues 
RouteQueues* RouteProvider::Find(RIDataId id) const
{
    const auto it = routingTable_.find(id);
    return it == routingTable_.end() ? nullptr : it->second;
}

const RouteDefinition*RouteProvider::FindRoute(const NotificationTarget& target) const
{
    switch (target.type)
    {
    case NotificationTargetType::Data:
        return FindDataRoute(static_cast<RIDataId>(target.id));

    case NotificationTargetType::Capability:
        return FindCapabilityRoute(static_cast<RICapabilityId>(target.id));

    default:
        return nullptr;
    }
}

//DataID -> RouteDefinition 
const RouteDefinition*RouteProvider::FindDataRoute(RIDataId id) const
{
    const auto* queues = Find(id);
    return queues == nullptr ? nullptr : queues->route;
}

//CapabilityID -> RouteDefinition 
const RouteDefinition* RouteProvider::FindCapabilityRoute(RICapabilityId id) const
{
    const auto it = capabilityRouteTable_.find(id);
    return it == capabilityRouteTable_.end() ? nullptr : it->second;
}

} // namespace rim