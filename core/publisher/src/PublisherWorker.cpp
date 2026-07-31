#include "PublisherWorker.hpp"

#include <cstddef>

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
                    CapabilityChangeList input{};

                    if (!queue_.WaitAndPop(
                            input))
                    {
                        break;
                    }

                    for (std::size_t i = 0;
                        i < input.Size();
                        ++i)
                    {
                        manager_.Publish(
                            input.At(i));
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
    CapabilityChangeList input{};

    if (!queue_.TryPop(
            input))
    {
        return false;
    }

    for (std::size_t i = 0;
        i < input.Size();
        ++i)
    {
        manager_.Publish(
            input.At(i));
    }

    return true;
}

} // namespace rim