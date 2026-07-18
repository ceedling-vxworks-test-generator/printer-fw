#pragma once

#include <atomic>
#include <thread>

#include "publisher/PublishManager.hpp"
#include "publisher/PublisherInputQueue.hpp"

namespace rim
{

class PublisherWorker
{
public:

    PublisherWorker(
        PublisherInputQueue& queue,
        PublishManager& manager)
        : queue_(queue)
        , manager_(manager)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    PublisherInputQueue& queue_;

    PublishManager& manager_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;};

} // namespace rim