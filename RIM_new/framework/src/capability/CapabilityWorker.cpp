#include "capability/CapabilityWorker.hpp"

namespace rim
{

void CapabilityWorker::Run()
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
                    RIMSnapshot snapshot{};

                    if (!queue_.WaitAndPop(
                            snapshot))
                    {
                        break;
                    }

                    manager_.Evaluate(
                        snapshot);

                    publisherQueue_.Push(
                        PublisherInput{});
                }
            });
}

void CapabilityWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool CapabilityWorker::ExecuteOnce()
{
    RIMSnapshot snapshot{};

    if (!queue_.TryPop(
            snapshot))
    {
        return false;
    }

    manager_.Evaluate(
        snapshot);

    publisherQueue_.Push(
        PublisherInput{});

    return true;
}

} // namespace rim
