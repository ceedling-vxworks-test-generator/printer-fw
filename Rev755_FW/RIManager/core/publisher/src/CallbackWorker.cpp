#include "CallbackWorker.hpp"

namespace rim
{

void CallbackWorker::Run(
    std::size_t workerCount)
{
    // TODO: ログ追加候補
    // CallbackWorker起動時に
    // workerCountを出力する。

    if (running_)
    {
        return;
    }

    if (workerCount == 0)
    {
        workerCount = 1;
    }

    running_ = true;

    for (std::size_t i = 0; i < workerCount; ++i)
    {
        workerThreads_.emplace_back(
            [this]
            {
                WorkerLoop();
            });
    }
}

void CallbackWorker::WorkerLoop()
{
    while (running_)
    {
        CallbackNotification
            notification;

        if (!queue_.WaitAndPop(
                notification))
        {
            // TODO: ログ追加候補
            // Queue停止や通知取得失敗時に発生回数を出力すると
            // CallbackWorker停止・異常終了調査に利用できる。
            
            continue;
        }

        // TODO: ログ追加候補
        // Callback実行前に SubscriptionId、TargetType、
        // TargetId、Trigger を出力すると通知経路追跡に利用できる。

        registry_.Notify(
            notification.subscriptionId,
            {
                notification.target,
                notification.trigger
            });
    }
}

void CallbackWorker::Stop()
{
    // TODO: ログ追加候補
    // CallbackWorker停止要求を出力する。
    
    running_ = false;

    queue_.Shutdown();

    for (auto& thread :
         workerThreads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    workerThreads_.clear();
}

}