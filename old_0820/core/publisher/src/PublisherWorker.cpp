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
                    PublisherInput input{};
                        if (queue_.WaitAndPopFor(
                            input,
                            std::chrono::milliseconds(100))) 
                        {
                            //変更通知
                            manager_.Publish(input.target,NotificationTrigger::OnChange);
                        }
                    
                    //定期通知
                    manager_.ProcessPeriodicNotifications();
                }
            }
        );
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

    manager_.Publish(
    input.target,
    NotificationTrigger::OnChange);

    manager_.ProcessPeriodicNotifications();

    return true;
}


} // namespace rim