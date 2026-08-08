#include "RoutePipeline.hpp"

#include "DataStoreDispatcher.hpp"
#include "CapabilityWorker.hpp"
#include "ValueStore.hpp"
#include "IChangeChecker.hpp"
#include "IRIMSnapshotReader.hpp"

namespace rim
{

RoutePipeline::RoutePipeline(
    const ProductDefinition& product,
    DataStoreQueue& queue,
    ValueStore& valueStore,
    const IChangeChecker& changeChecker,
    IRIMSnapshotReader& snapshotReader,
    CapabilityManager& capabilityManager,
    PublisherInputQueue& publisherQueue)
{
    dispatcher_ =
        std::make_unique<DataStoreDispatcher>(
            product,
            queue,
            valueStore,
            changeChecker,
            snapshotReader,
            capabilityQueue_,
            0);

    capabilityWorker_ =
        std::make_unique<CapabilityWorker>(
            capabilityQueue_,
            capabilityManager,
            publisherQueue);
}

RoutePipeline::~RoutePipeline()
{
    Stop();
}

void RoutePipeline::Start()
{
    dispatcher_->start();
    capabilityWorker_->Run();
}

void RoutePipeline::Stop()
{
    capabilityWorker_->Stop();
    dispatcher_->stop();
}

bool RoutePipeline::ExecuteOnce()
{
    if (!dispatcher_->ExecuteOnce())
    {
        return false;
    }

    return capabilityWorker_->ExecuteOnce();
}

}