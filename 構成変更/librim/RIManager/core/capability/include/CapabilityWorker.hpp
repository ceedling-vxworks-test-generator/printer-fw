#pragma once

#include <atomic>
#include <thread>

#include "CapabilityInputQueue.hpp"
#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"
#include "CapabilityChangeDetector.hpp"

#include "PublisherInputQueue.hpp"

namespace rim
{

class CapabilityWorker
{
public:

    CapabilityWorker(
        CapabilityInputQueue& queue,
        CapabilityManager& manager,
        MachineCapabilityStore& store,
        PublisherInputQueue& publisherQueue)
        : queue_(queue)
        , manager_(manager)
        , store_(store)
        , publisherQueue_(publisherQueue)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    CapabilityInputQueue& queue_;

    CapabilityManager& manager_;

    MachineCapabilityStore& store_;

    CapabilityChangeDetector
        changeDetector_;

    PublisherInputQueue& publisherQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim