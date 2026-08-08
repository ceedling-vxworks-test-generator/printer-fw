#include <iostream>
#include <cassert>
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"
#include "RouteDefinition.hpp"
#include "DataItemDefinition.hpp"

namespace rim
{

// void RouteProvider::Initialize(const IDataDefinitionProvider& definitionProvider)
void RouteProvider::Initialize(const ProductDefinition& product)
{
    queues_.clear();
    routingTable_.clear();

    const auto* routes = GetRoutes(product);
    const auto* items = GetDataItems(product);

    for (std::size_t i = 0; i < GetRouteCount(product); ++i)
    {
        const auto& route = routes[i];
        queues_.emplace(route.name, QueueFactory::Create(route.policy));
    }

    for (std::size_t i = 0; i < GetDataItemCount(product); ++i)
    {
        const auto& item = items[i];

        auto queueIt = queues_.find(item.route);

        assert(queueIt != queues_.end());

        routingTable_.emplace(item.id, queueIt->second.get());
    }
}

DataStoreQueue* RouteProvider::find(RIDataId id) const
{
    const auto it = routingTable_.find(id);
    return it == routingTable_.end() ? nullptr : it->second;
}

} // namespace rim