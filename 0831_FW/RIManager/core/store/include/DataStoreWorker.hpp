#pragma once

#include <atomic>
#include <thread>

#include "PartitionStorageRegistry.hpp"
#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMDataItem.hpp"
#include "ProductContext.hpp"

namespace rim
{

class DataStoreWorker
{
public:
    DataStoreWorker(
        const ProductContext& context,
        IQueue<RIMDataItem>& queue,
        PartitionStorageRegistry& domainStore,
        IQueue<CapabilityInput>& capabilityQueue)
        :
        context_(context),
        queue_(queue),
        domainStore_(domainStore),
        capabilityQueue_(capabilityQueue)
    {
    }

    ~DataStoreWorker();

    void start();

    void stop();

    bool ExecuteOnce();

private:

    void WorkerLoop();

    void Process(RIMDataItem& item);

    const ProductContext& context_;
    IQueue<RIMDataItem>& queue_;
    PartitionStorageRegistry& domainStore_;
    IQueue<CapabilityInput>& capabilityQueue_;
    std::atomic<bool> running_{false};
    std::thread workerThread_;
};

} // namespace rim
