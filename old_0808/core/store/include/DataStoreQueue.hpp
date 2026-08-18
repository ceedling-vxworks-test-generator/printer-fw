#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

#include "RIMDataItem.hpp"

namespace rim
{

class RIMDataItem;

class DataStoreQueue
{
public:

    virtual ~DataStoreQueue() = default;

    virtual bool push(const RIMDataItem& item) = 0;
    virtual bool Pop(RIMDataItem& item) = 0;
    virtual bool TryPop(RIMDataItem& item) = 0;
    virtual void shutdown() = 0;

private:

};

} // namespace rim