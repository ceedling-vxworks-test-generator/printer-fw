#include "PublisherWorker.hpp"

namespace rim
{

void PublisherWorker::Run()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            [this]
            {
                while (running_)
                {
                    CapabilityChangeSet input{};

                    if (!queue_.WaitAndPop(
                            input))
                    {
                        break;
                    }

                    for (const auto& capability :
                        input.changedCapabilities)
                    {
                        manager_.Publish(
                            capability);
                    }

                }
            });
}

void PublisherWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool PublisherWorker::ExecuteOnce()
{
    CapabilityChangeSet input{};

    if (!queue_.TryPop(
            input))
    {
        return false;
    }

    for (const auto& capability :
        input.changedCapabilities)
    {
        manager_.Publish(
            capability);
    }

    return true;
}

} // namespace rim