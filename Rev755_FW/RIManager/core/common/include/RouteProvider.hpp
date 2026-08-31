#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include "RIId.hpp"

#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMDataItem.hpp"
#include "ProductContext.hpp"

namespace rim
{

struct RouteQueues
{
    std::unique_ptr<IQueue<RIMDataItem>> storeQueue;
    std::unique_ptr<IQueue<CapabilityInput>> capabilityQueue;
};

class RouteProvider
{
public:

    RouteProvider() = default;

    void Initialize(const ProductContext& context);

    RouteQueues* Find(RIDataId id) const;

    const auto& GetQueues() const
    {
        return queues_;
    }

private:

    std::unordered_map<std::string_view, RouteQueues> queues_;
    std::unordered_map<RIDataId, RouteQueues*> routingTable_;
};

} // namespace rim