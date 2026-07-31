#include "CapabilityWorker.hpp"

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

                    const auto changes =
                        changeDetector_.Detect(
                            store_);

                    if (!changes.Empty())
                    {
                        publisherQueue_.Push(
                            changes);
                    }
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

    const auto changes =
        changeDetector_.Detect(
            store_);

    if (!changes.Empty())
    {
        publisherQueue_.Push(
            changes);
    }

    return true;
}

} // namespace rim
