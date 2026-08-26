#include "RouteExecutor.hpp"

#include "DataStoreWorker.hpp"
#include "CapabilityWorker.hpp"

#include "ICapabilitySnapshotProvider.hpp"
#include "SnapshotAccessor.hpp"

#include "DomainStorageRegistry.hpp"
#include "RouteProvider.hpp"

namespace rim
{

RouteExecutor::RouteExecutor(
    const ProductDefinition& product,
    const RouteQueues& queues,
    DomainStorageRegistry& domainStore,
    CapabilityManager& capabilityManager,
    PublisherInputQueue& publisherQueue,
    const ICapabilitySnapshotProvider& snapshotProvider)
{
    dispatcher_ =
        std::make_unique<DataStoreWorker>(
            product,
            *queues.storeQueue,
            domainStore,
            *queues.capabilityQueue);

    capabilityWorker_ =
        std::make_unique<CapabilityWorker>(
            *queues.capabilityQueue,
            capabilityManager,
            publisherQueue,
            snapshotProvider);
}

RouteExecutor::~RouteExecutor()
{
    Stop();
}

void
RouteExecutor::Start()
{
    dispatcher_->start();
    capabilityWorker_->Run();
}

void
RouteExecutor::Stop()
{
    if (stopped_)
    {
        return;
    }

    stopped_ = true;

    capabilityWorker_->Stop();
    dispatcher_->stop();
}

bool
RouteExecutor::ExecuteOnce()
{
    if (!dispatcher_->ExecuteOnce())
    {
        return false;
    }

    return capabilityWorker_->ExecuteOnce();
}

} // namespace rim