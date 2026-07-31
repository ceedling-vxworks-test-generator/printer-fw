#include "CapabilityWorker.hpp"

namespace rim
{

void CapabilityWorker::ProcessSnapshot(
    const RIMSnapshot& snapshot)
{
    evaluator_.Evaluate(
        snapshot,
        store_);

    const auto changes =
        changeTracker_.Detect(
            store_);

    if (!changes.Empty())
    {
        publisherQueue_.Push(
            changes);
    }
}

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

                    ProcessSnapshot(
                        snapshot);
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

    ProcessSnapshot(
        snapshot);

    return true;
}

} // namespace rim
