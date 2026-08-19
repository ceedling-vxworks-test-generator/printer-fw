#include "RoutePipeline.hpp"

#include "DataStoreDispatcher.hpp"
#include "CapabilityWorker.hpp"
#include "DomainStorageRegistry.hpp"
#include "IRIMSnapshotReader.hpp"
#include "RouteProvider.hpp"

namespace rim
{

RoutePipeline::RoutePipeline(
    const ProductDefinition& product,
    const RouteQueues& queues,
    DomainStorageRegistry& domainStore,
    IRIMSnapshotReader& snapshotReader,
    CapabilityManager& capabilityManager,
    PublisherInputQueue& publisherQueue)
{
    dispatcher_ =
        std::make_unique<DataStoreDispatcher>(
            product,
            *queues.storeQueue,
            domainStore,
            snapshotReader,
            *queues.capabilityQueue);

    capabilityWorker_ =
        std::make_unique<CapabilityWorker>(
            *queues.capabilityQueue,
            capabilityManager,
            publisherQueue);
}

RoutePipeline::~RoutePipeline()
{
    Stop();
}

void
RoutePipeline::Start()
{
    dispatcher_->start();
    capabilityWorker_->Run();
}

void
RoutePipeline::Stop()
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
RoutePipeline::ExecuteOnce()
{
    if (!dispatcher_->ExecuteOnce())
    {
        return false;
    }

    return capabilityWorker_->ExecuteOnce();
}

}