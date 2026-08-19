#pragma once

#include <atomic>
#include <thread>

#include "CapabilityManager.hpp"
#include "IQueue.hpp"
#include "CapabilityInput.hpp"
#include "PublisherInput.hpp"
#include "PublisherInputQueue.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        IQueue<CapabilityInput>& queue,
        CapabilityManager& manager,
        PublisherInputQueue& publisherQueue);
        
    ~CapabilityWorker();

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    void Process(const CapabilityInput& capabilityInput);
    void PublishCapabilityChanges(const CapabilityChangeSet& changes);

    IQueue<CapabilityInput>& queue_;
    CapabilityManager& manager_;
    PublisherInputQueue& publisherQueue_;
    std::atomic<bool> running_{false};
    std::thread workerThread_;
};

} // namespace rim