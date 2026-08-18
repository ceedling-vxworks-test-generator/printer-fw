#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include "rim_data_id.h"

#include "ProductDefinition.hpp"
#include "DataStoreQueue.hpp"

namespace rim
{

class DataStoreQueue;

class RouteProvider
{
public:

    RouteProvider() = default;
    // void Initialize(const IDataDefinitionProvider& definitionProvider);
    void Initialize(const ProductDefinition& product);
    DataStoreQueue* find(RIDataId id) const;

    // const std::unordered_map<DataLabel, std::unique_ptr<DataStoreQueue>>& GetQueues() const
    const std::unordered_map<std::string_view, std::unique_ptr<DataStoreQueue>>& GetQueues() const
    {
        return queues_;
    }
private:

    // std::unordered_map<DataLabel, std::unique_ptr<DataStoreQueue>> queues_;
    std::unordered_map<std::string_view, std::unique_ptr<DataStoreQueue>> queues_;
    std::unordered_map<RIDataId, DataStoreQueue*> routingTable_;
    // std::vector<std::unique_ptr<DataStoreQueue>> queues_;
};

} // namespace rim