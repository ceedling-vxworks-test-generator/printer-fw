#include "publisher/PublisherWorker.hpp"

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
                    PublisherInput input{};

                    if (!queue_.WaitAndPop(
                            input))
                    {
                        break;
                    }

                    manager_.HasEnvironmentChanged();

                    manager_.HasErrorChanged();

                    manager_.HasPrintReadyChanged();
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
    PublisherInput input{};

    if (!queue_.TryPop(
            input))
    {
        return false;
    }

    manager_.HasEnvironmentChanged();

    manager_.HasErrorChanged();

    manager_.HasPrintReadyChanged();

    return true;
}

} // namespace rim