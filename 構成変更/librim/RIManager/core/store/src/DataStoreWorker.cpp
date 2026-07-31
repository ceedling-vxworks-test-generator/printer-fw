#include "DataStoreWorker.hpp"

namespace rim
{

void DataStoreWorker::Run()
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
                    RIMDataItem item{};

                    if (!queue_.WaitAndPop(
                            item))
                    {
                        break;
                    }

                    store_.Store(
                        item);

                    const auto snapshot =
                        reader_.Read();

                    capabilityQueue_.Push(
                        snapshot);
                }
            });
}

void DataStoreWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool DataStoreWorker::ExecuteOnce()
{
    RIMDataItem item{};

    if (!queue_.TryPop(
            item))
    {
        return false;
    }

    store_.Store(
        item);

    const RIMSnapshot snapshot =
        reader_.Read();

    capabilityQueue_.Push(
        snapshot);

    return true;
}

} // namespace rim