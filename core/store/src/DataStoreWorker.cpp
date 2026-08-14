#include <cstdio>
#include <iostream>
#include <cassert>
#include "DataStoreWorker.hpp"

#include "DataItemDefinition.hpp"
#include "ProductDefinition.hpp"
#include "BinaryStoreValue.hpp"
#include "BinaryInput.hpp"

namespace rim
{

void DataStoreWorker::Run()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            [this]
            {
                while (running_)
                {
                    RIMDataItem item{};

                    if (!queue_.WaitAndPop(
                            item))
                    {
                        break;
                    }

                    auto queue = routeProvider_.find(item.id);

                    if (queue == nullptr)
                    {
                        continue;
                    }

                    queue->push(item);

                }
            });
}

void DataStoreWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool DataStoreWorker::ExecuteOnce()
{
    RIMDataItem item{};

    if (!queue_.TryPop(
            item))
    {
        return false;
    }

    auto queue = routeProvider_.find(item.id);

    if (queue== nullptr)
    {
        return false;
    }

    queue->push(item);

    return true;
}

} // namespace rim