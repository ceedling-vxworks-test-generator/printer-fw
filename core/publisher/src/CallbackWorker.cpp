#include "CallbackWorker.hpp"

namespace rim
{

void CallbackWorker::Run()
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
                    CallbackNotification
                        notification;

                    if (!queue_.WaitAndPop(
                            notification))
                    {
                        continue;
                    }

                    registry_.Notify(
                        notification.subscriptionId,
                        {
                            notification.target,
                            notification.trigger
                        });
                }
            });
}

void CallbackWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

}