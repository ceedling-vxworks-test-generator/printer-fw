#include "PublisherWorker.hpp"

namespace rim
{

void PublisherWorker::Run()
{
    // TODO: ログ追加候補
    // PublisherWorker起動時に状態を出力すると
    // スレッド起動確認に利用できる。

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
                            // TODO: ログ追加候補
                            // OnChange通知処理時に TargetType、TargetId を出力すると
                            // 発行通知の追跡に利用できる。
                            
                            //変更通知
                            manager_.Publish(input.target,NotificationTrigger::OnChange);
                        }

                    // TODO: ログ追加候補
                    // 定期通知処理呼び出し回数を出力すると
                    // Worker動作確認に利用できる。

                    //定期通知
                    manager_.ProcessPeriodicNotifications();
                }
            }
        );
}

void PublisherWorker::Stop()
{
    // TODO: ログ追加候補
    // PublisherWorker停止要求を出力すると
    // シャットダウン調査に利用できる。
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

    if (!queue_.TryPop(input))
    {
        return false;
    }

    manager_.Publish( input.target, NotificationTrigger::OnChange);

    manager_.ProcessPeriodicNotifications();

    return true;
}


} // namespace rim