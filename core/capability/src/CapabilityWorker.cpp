#include "CapabilityWorker.hpp"

#include "NotificationTargetType.hpp"

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
    if (changeData.changedDataId == RI_INVALID_DATA_ID)
    {
        return;
    }

    PublishDataChanges(changeData);
    PublishCapabilityChanges(changeData.changedDataId);
}

void CapabilityWorker::PublishDataChanges(const CapabilityInput& change)
{
    PublisherInput input{};

    input.target.type = NotificationTargetType::Data;
    input.target.id = static_cast<std::uint32_t>(change.changedDataId);
    
    const auto* route =productContext_.ResolveRoute(input.target);
    if (route != nullptr)
    {
        input.compressionPolicy =route->compressionPolicy;
    }

    publisherQueue_.Push(input);
}

void CapabilityWorker::PublishCapabilityChanges(const RIDataId dataId)
{
    const auto& capabilities = productContext_.CapabilityDependencies().Find(dataId);

    for (const auto* capability : capabilities)
    {
        const auto snapshot = capabilitySnapshotProvider_.Create(capability->id);
        const bool changed = capabilityManager_.Evaluate(snapshot, capability);

        if (!changed)
        {
            continue;
        }

        PublisherInput input{};

        input.target.type = NotificationTargetType::Capability;
        input.target.id = static_cast<std::uint32_t>(capability->id);
        
        const auto* route =productContext_.ResolveRoute(input.target);
        if (route != nullptr)
        {
            input.compressionPolicy =route->compressionPolicy;
        }

        publisherQueue_.Push(input);

        PublishFacadeChanges(capability->id);
    }
}

void CapabilityWorker::PublishFacadeChanges(const RICapabilityId capabilityId)
{
    const auto& facades = productContext_.FacadeDependencies().Find(capabilityId);

    for (const auto* facade : facades)
    {
        const auto snapshot = facadeSnapshotProvider_.Create(facade->id);
        const bool changed = facadeManager_.Evaluate(snapshot, facade);

        if (!changed)
        {
            continue;
        }

        PublisherInput input{};

        input.target.type = NotificationTargetType::Facade;
        input.target.id = static_cast<std::uint32_t>(facade->id);

        const auto* route =productContext_.ResolveRoute(input.target);
        if (route != nullptr)
        {
            input.compressionPolicy =route->compressionPolicy;
        }


        publisherQueue_.Push(input);
    }
}

} // namespace rim