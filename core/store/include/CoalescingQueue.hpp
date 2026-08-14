#pragma once

#include <mutex>
#include <unordered_map>

#include "DataStoreQueue.hpp"
#include "RIMDataItem.hpp"
#include "rim_data_id.h"

namespace rim
{

class CoalescingQueue : public DataStoreQueue
{
public:

    bool push(const RIMDataItem& item) override;
    bool Pop(RIMDataItem& item) override;
    bool TryPop(RIMDataItem& item) override;
    void shutdown() override;

private:

    std::unordered_map<RIDataId, RIMDataItem> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool shutdown_ = false;
};

}