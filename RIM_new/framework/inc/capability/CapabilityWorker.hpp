#pragma once

#include <atomic>
#include <thread>

#include "capability/CapabilityInputQueue.hpp"
#include "capability/CapabilityManager.hpp"

#include "publisher/PublisherInputQueue.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        CapabilityInputQueue& queue,
        CapabilityManager& manager,
        PublisherInputQueue& publisherQueue)
        : queue_(queue)
        , manager_(manager)
        , publisherQueue_(publisherQueue)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    CapabilityInputQueue& queue_;

    CapabilityManager& manager_;

    PublisherInputQueue& publisherQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;};

} // namespace rim