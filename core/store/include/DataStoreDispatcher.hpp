#pragma once

#include <atomic>
#include <thread>

#include "IRIMSnapshotReader.hpp"
#include "ProductDefinition.hpp"

#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class DataStoreDispatcher
{
public:

    DataStoreDispatcher(
        const ProductDefinition& product,
        IQueue<RIMDataItem>& queue,
        DomainStorageRegistry& domainStore,
        IRIMSnapshotReader& reader,
        IQueue<CapabilityInput>& capabilityQueue);

    ~DataStoreDispatcher();

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
    IRIMSnapshotReader& reader_;
    DataDomainMap dataDomainMap_;
};

} // namespace rim
