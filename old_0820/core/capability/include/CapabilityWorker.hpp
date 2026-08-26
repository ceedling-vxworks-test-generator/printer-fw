#pragma once

#include <atomic>
#include <thread>

#include "CapabilityManager.hpp"
#include "CapabilityInput.hpp"
#include "IQueue.hpp"
#include "PublisherInput.hpp"
#include "PublisherInputQueue.hpp"
#include "ICapabilitySnapshotProvider.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        IQueue<CapabilityInput>& queue,
        CapabilityManager& manager,
        PublisherInputQueue& publisherQueue,
        const ICapabilitySnapshotProvider& snapshotProvider);

    ~CapabilityWorker();

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    void Process(
        const CapabilityInput& capabilityInput);

    void PublishCapabilityChanges(
        const CapabilityChangeSet& changes);

private:

    IQueue<CapabilityInput>&
        queue_;

    CapabilityManager&
        manager_;

    PublisherInputQueue&
        publisherQueue_;

    const ICapabilitySnapshotProvider&
        snapshotProvider_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim