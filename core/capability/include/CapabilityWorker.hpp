#pragma once

#include <atomic>
#include <thread>

#include "CapabilityInputQueue.hpp"
#include "CapabilityManager.hpp"

#include "PublisherInputQueue.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        CapabilityInputQueue& queue,
        CapabilityManager& manager,
        PublisherInputQueue& publisherQueue);
        
    ~CapabilityWorker();

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    void Process(
    const CapabilityInput& capabilityInput);

    void PublishCapabilityChanges(
    const CapabilityChangeSet& changes);

    CapabilityInputQueue& queue_;

    CapabilityManager& manager_;

    PublisherInputQueue& publisherQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim