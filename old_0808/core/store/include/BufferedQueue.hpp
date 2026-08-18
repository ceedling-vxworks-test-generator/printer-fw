#pragma once

#include "DataStoreQueue.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class BufferedQueue : public DataStoreQueue
{
public:

    bool push(const RIMDataItem& item) override;
    bool Pop(RIMDataItem& item) override;
    bool TryPop(RIMDataItem& item) override;
    void shutdown() override;

private:

    std::queue<RIMDataItem> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool shutdown_ = false;
};

}