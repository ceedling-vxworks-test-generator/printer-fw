#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include "RIId.hpp"

#include "ProductDefinition.hpp"
#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMDataItem.hpp"
#include "RouteDefinition.hpp"

namespace rim
{

struct RouteQueues
{
    const RouteDefinition* route;

    std::unique_ptr<IQueue<RIMDataItem>> storeQueue;
    std::unique_ptr<IQueue<CapabilityInput>> capabilityQueue;
};

class RouteProvider
{
public:

    RouteProvider() = default;

    void Initialize(const ProductDefinition& product);

    RouteQueues* Find(RIDataId id) const;

    const auto& GetQueues() const
    {
        return queues_;
    }

    const RouteDefinition*FindRoute(const NotificationTarget& target) const;

private:

    std::unordered_map<std::string_view, RouteQueues> queues_;
    std::unordered_map<RIDataId, RouteQueues*> routingTable_;
    std::unordered_map<RICapabilityId, const RouteDefinition*> capabilityRouteTable_;
    // std::unordered_map<RIFacadeId,const RouteDefinition*> facadeRouteTable_;

    const RouteDefinition*FindDataRoute(RIDataId id) const;
    const RouteDefinition*FindCapabilityRoute(RICapabilityId id) const;
};

} // namespace rim