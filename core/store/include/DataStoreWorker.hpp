#pragma once

#include <atomic>
#include <thread>

#include "ProductDefinition.hpp"

#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class DataStoreWorker
{
public:

    DataStoreWorker(
        const ProductDefinition& product,
        IQueue<RIMDataItem>& queue,
        DomainStorageRegistry& domainStore,
        IQueue<CapabilityInput>& capabilityQueue);

    ~DataStoreWorker();

    void start();

    void stop();

    bool ExecuteOnce();

private:

    void run();

    bool ProcessItem(
        RIMDataItem& item);

private:

    const ProductDefinition& product_;
    IQueue<RIMDataItem>& queue_;
    DomainStorageRegistry& domainStore_;
    IQueue<CapabilityInput>& capabilityQueue_;
    std::atomic<bool> running_{false};
    std::thread workerThread_;
    DataDomainMap dataDomainMap_;
};

} // namespace rim
