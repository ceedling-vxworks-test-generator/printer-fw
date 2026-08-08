#pragma once

#include <iostream>
#include <memory>

#include "DataStoreQueue.hpp"
#include "CapabilityInputQueue.hpp"
#include "PublisherInputQueue.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class DataStoreDispatcher;
class CapabilityWorker;

class CapabilityManager;
class CapabilityStore;

class ValueStore;
class IChangeChecker;
class IRIMSnapshotReader;

class RoutePipeline
{
public:
    RoutePipeline(
        const ProductDefinition& product,
        DataStoreQueue& queue,
        ValueStore& valueStore,
        const IChangeChecker& changeChecker,
        IRIMSnapshotReader& snapshotReader,
        CapabilityManager& capabilityManager,
        PublisherInputQueue& publisherQueue);

    ~RoutePipeline();

    void Start();

    void Stop();

    bool ExecuteOnce();

private:

    CapabilityInputQueue capabilityQueue_;
    std::unique_ptr<DataStoreDispatcher> dispatcher_;
    std::unique_ptr<CapabilityWorker> capabilityWorker_;
};

}