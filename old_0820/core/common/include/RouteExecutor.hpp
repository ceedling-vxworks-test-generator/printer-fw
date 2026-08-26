#pragma once

#include <memory>

#include "PublisherInputQueue.hpp"
#include "ProductDefinition.hpp"
#include "RouteProvider.hpp"

namespace rim
{

class DataStoreWorker;
class CapabilityWorker;

class CapabilityManager;
class DomainStorageRegistry;

class CapabilitySnapshotResolver;
class SnapshotAccessor;
class ICapabilitySnapshotProvider;

class RouteExecutor
{
public:

    RouteExecutor(
        const ProductDefinition& product,
        const RouteQueues& queues,
        DomainStorageRegistry& domainStore,
        CapabilityManager& capabilityManager,
        PublisherInputQueue& publisherQueue,
        const ICapabilitySnapshotProvider& snapshotProvider);

    ~RouteExecutor();

    void Start();

    void Stop();

    bool ExecuteOnce();

private:

    bool stopped_{false};

    std::unique_ptr<DataStoreWorker>
        dispatcher_;

    std::unique_ptr<CapabilityWorker>
        capabilityWorker_;
};

} // namespace rim