#include <cassert>
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"
#include "RouteDefinition.hpp"
#include "DataItemDefinition.hpp"

namespace rim
{

void RouteProvider::Initialize(const ProductContext& context)
{
    queues_.clear();
    routingTable_.clear();

    // TODO: ログ追加候補
    // RouteProvider初期化開始を出力すると
    // 定義再ロードの確認に利用できる。

    const auto* routes = context.GetRoutes();
    const auto* items = context.GetDataItems();

    for (std::size_t i = 0; i < context.GetRouteCount(); ++i)
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
                std::move(storeQueue),
                std::move(capabilityQueue)
            });

        // TODO: ログ追加候補
        // Route生成時に Route名とQueuePolicyを出力すると
        // 初期設定確認に利用できる。
    }

    for (std::size_t i = 0; i < context.GetDataItemCount(); ++i)
    {
        const auto& item = items[i];

        auto* route = context.FindRoute(item.routeId);
        assert(route != nullptr);
        auto queueIt = queues_.find(route->name);

        // auto queueIt = queues_.find(route.id);

        assert(queueIt != queues_.end());

        routingTable_.emplace(item.id, &queueIt->second);

        // TODO: ログ追加候補
        // DataIdとRoute名の対応を出力すると
        // Routing定義確認に利用できる。

    }
}

//DataID -> RouteQueues 
RouteQueues* RouteProvider::Find(RIDataId id) const
{
    const auto it = routingTable_.find(id);

    // TODO: ログ追加候補
    // Route未検出時に DataId を出力すると
    // 定義不整合の調査に利用できる。

    return it == routingTable_.end() ? nullptr : it->second;
}

} // namespace rim