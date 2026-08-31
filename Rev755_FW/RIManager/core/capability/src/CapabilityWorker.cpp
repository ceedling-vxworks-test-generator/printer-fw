#include "CapabilityWorker.hpp"

#include "RIMId.hpp"

namespace rim
{

CapabilityWorker::CapabilityWorker(
    IQueue<CapabilityInput>& queue,
    CapabilityManager& capabilityManager,
    FacadeManager& facadeManager,
    PublisherInputQueue& publisherQueue,
    const ICapabilitySnapshotProvider& capabilitySnapshotProvider,
    const IFacadeSnapshotProvider& facadeSnapshotProvider,
    const ProductContext& productContext)
    : queue_(queue),
      capabilityManager_(capabilityManager),
      facadeManager_(facadeManager),
      publisherQueue_(publisherQueue),
      capabilitySnapshotProvider_(capabilitySnapshotProvider),
      facadeSnapshotProvider_(facadeSnapshotProvider),
      productContext_(productContext)
{
}

CapabilityWorker::~CapabilityWorker()
{
    stop();
}

void CapabilityWorker::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ = std::thread(&CapabilityWorker::WorkerLoop, this);
}

void CapabilityWorker::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}


void CapabilityWorker::WorkerLoop()
{
    while (running_)
    {
        CapabilityInput item{};

        if (!queue_.WaitAndPop(item))
        {
            break;
        }

        Process(item);
    }
}

bool CapabilityWorker::ExecuteOnce()
{
    CapabilityInput item{};

    if (!queue_.TryPop(item))
    {
        return false;
    }

    Process(item);

    return true;
}

void CapabilityWorker::Process(const CapabilityInput& changeData)
{
    if (changeData.changedId.type !=
        RIMIdType::Data)
    {
        return;
    }

    PublishDataChanges(changeData);

    PublishCapabilityChanges(
        changeData.changedId);
}

void CapabilityWorker::PublishDataChanges(const CapabilityInput& change)
{
    PublisherInput input{};

    input.target =
        change.changedId;

    const auto* item =
        productContext_.FindDataItem(
            input.target);

    if (item != nullptr)
    {
        input.compressionPolicy =item->publisherCompressionPolicy;
    }

    publisherQueue_.Push(input);
}

void CapabilityWorker::PublishCapabilityChanges(
    const RIMId& dataId)
{
    const auto& capabilities =
        productContext_
            .AffectedCapabilities()
            .Find(dataId);

    for (const auto* capability : capabilities)
    {
        const auto snapshot = capabilitySnapshotProvider_.Create(capability->id);
        const bool changed = capabilityManager_.Evaluate(snapshot, capability);

        if (!changed)
        {
            continue;
        }

        PublisherInput input{};

        input.target =
        {
            RIMIdType::Capability,
            static_cast<std::uint32_t>(
                capability->id)
        };

        const auto* item =
            productContext_.FindCapability(
                input.target);

        if (item != nullptr)
        {
            input.compressionPolicy =item->publisherCompressionPolicy;
        }

        publisherQueue_.Push(input);

        PublishFacadeChanges(capability->id);
    }
}

void CapabilityWorker::PublishFacadeChanges(const RICapabilityId capabilityId)
{
    const RIMId target
    {
        RIMIdType::Capability,
        static_cast<std::uint32_t>(capabilityId)
    };

    const auto& facades = productContext_.AffectedFacades().Find(capabilityId);

    for (const auto* facade : facades)
    {
        const auto snapshot = facadeSnapshotProvider_.Create(facade->id);
        const bool changed = facadeManager_.Evaluate(snapshot, facade);

        if (!changed)
        {
            continue;
        }

        PublisherInput input{};

        input.target =
        {
            RIMIdType::Facade,
            static_cast<std::uint32_t>(
                facade->id)
        };

        const auto* item =
            productContext_.FindFacade(
                input.target);
        if (item != nullptr)
        {
            input.compressionPolicy =item->publisherCompressionPolicy;
        }


        publisherQueue_.Push(input);
    }
}

} // namespace rim