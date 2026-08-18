#pragma once

#include <atomic>
#include <thread>

#include "CallbackQueue.hpp"
#include "CallbackSubscriptionRegistry.hpp"

namespace rim
{

class CallbackWorker
{
public:

    CallbackWorker(
        CallbackQueue& queue,
        CallbackSubscriptionRegistry& registry)
        : queue_(queue)
        , registry_(registry)
    {
    }

    void Run();

    void Stop();

private:

    CallbackQueue& queue_;

    CallbackSubscriptionRegistry&
        registry_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

}