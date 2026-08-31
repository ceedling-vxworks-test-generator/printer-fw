#pragma once

#include <memory>

#include "PublisherInputQueue.hpp"
#include "RouteProvider.hpp"
#include "ProductContext.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class DataStoreWorker;
class CapabilityWorker;

class CapabilityManager;
class FacadeManager;
class PartitionStorageRegistry;

class CapabilitySnapshotResolver;
class FacadeSnapshotResolver;
class SnapshotAccessor;
class ICapabilitySnapshotProvider;
class IFacadeSnapshotProvider;

class RouteExecutor
{
public:

    RouteExecutor(
        const ProductContext& context,
        const RouteQueues& queues,
        PartitionStorageRegistry& domainStore,
        CapabilityManager& capabilityManager,
        FacadeManager& facadeManager,
        PublisherInputQueue& publisherQueue,
        const ICapabilitySnapshotProvider& capabilitySnapshotProvider,
        const IFacadeSnapshotProvider& facadeSnapshotProvider);

    ~RouteExecutor();

    void Start();

    void Stop();

    bool ExecuteOnce();

private:

    bool stopped_{false};

    std::unique_ptr<DataStoreWorker>
        dataStoreWorker_;

    std::unique_ptr<CapabilityWorker>
        capabilityWorker_;
};

} // namespace rim