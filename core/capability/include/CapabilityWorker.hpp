#pragma once

#include <atomic>
#include <thread>

#include "CapabilityManager.hpp"
#include "CapabilityInput.hpp"
#include "FacadeManager.hpp"
#include "IQueue.hpp"
#include "PublisherInput.hpp"
#include "PublisherInputQueue.hpp"
#include "ICapabilitySnapshotProvider.hpp"
#include "IFacadeSnapshotProvider.hpp"
#include "ProductContext.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        IQueue<CapabilityInput>& queue,
        CapabilityManager& capabilityManager,
        FacadeManager& facadeManager,
        PublisherInputQueue& publisherQueue,
        const ICapabilitySnapshotProvider& capabilitySnapshotProvider,
        const IFacadeSnapshotProvider& facadeSnapshotProvider,
        const ProductContext& productContext);
    ~CapabilityWorker();

    void start();

    void stop();

    bool ExecuteOnce();

private:

    void WorkerLoop();

    void Process(const CapabilityInput& item);

    void PublishDataChanges(const CapabilityInput& change);
    void PublishCapabilityChanges(const RIDataId dataId);
    void PublishFacadeChanges(const RICapabilityId capabilityId);

    IQueue<CapabilityInput>& queue_;
    CapabilityManager& capabilityManager_;
    FacadeManager& facadeManager_;
    PublisherInputQueue& publisherQueue_;
    const ICapabilitySnapshotProvider& capabilitySnapshotProvider_;
    const IFacadeSnapshotProvider& facadeSnapshotProvider_;
    std::atomic<bool> running_{false};
    std::thread workerThread_;
    const ProductContext& productContext_;
};

} // namespace rim