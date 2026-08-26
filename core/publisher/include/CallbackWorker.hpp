#pragma once

#include <atomic>
#include <thread>
#include <vector>

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

    void Run(std::size_t workerCount = 1);

    void Stop();

private:

    void WorkerLoop();
    CallbackQueue& queue_;
    CallbackSubscriptionRegistry& registry_;
    std::atomic<bool> running_{false};
    std::vector<std::thread> workerThreads_;
    
};

}