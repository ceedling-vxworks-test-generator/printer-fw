#pragma once

#include <iostream>
#include <memory>

#include "PublisherInputQueue.hpp"
#include "ProductDefinition.hpp"
#include "RouteProvider.hpp"

namespace rim
{

class DataStoreDispatcher;
class CapabilityWorker;

class CapabilityManager;

class DomainStorageRegistry;
class IRIMSnapshotReader;

class RoutePipeline
{
public:

    RoutePipeline(
        const ProductDefinition& product,
        const RouteQueues& queues,
        DomainStorageRegistry& domainStore,
        IRIMSnapshotReader& snapshotReader,
        CapabilityManager& capabilityManager,
        PublisherInputQueue& publisherQueue);

    ~RoutePipeline();

    void Start();

    void Stop();

    bool ExecuteOnce();

private:

    bool stopped_{false};

    std::unique_ptr<DataStoreDispatcher> dispatcher_;
    std::unique_ptr<CapabilityWorker> capabilityWorker_;
};

}