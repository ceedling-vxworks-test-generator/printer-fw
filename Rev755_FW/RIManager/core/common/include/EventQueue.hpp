#pragma once

#include <mutex>
#include <list>
#include <chrono>
#include <condition_variable>
#include <utility>
#include "IQueue.hpp"


namespace rim
{

template<
    typename T,
    typename TPolicy>
class EventQueue
    : public IQueue<T>
{
public:

    EventQueue() = default;

    explicit EventQueue(
        TPolicy policy)
        : policy_(
            std::move(policy))
    {
    }

    bool Push(
        const T& event)
    {
        {
            std::lock_guard<std::mutex>
                lock(mutex_);

            if (shutdown_)
            {

                // TODO: ログ追加候補
                // Shutdown後にイベント投入が要求された場合は
                // 発生回数とイベント種別を出力すると
                // ライフサイクル不整合の調査に利用できる。

                return false;
            }

            policy_.Insert(events_,event);

            // TODO: ログ追加候補
            // Queue投入後の件数を出力すると
            // イベント滞留状況の確認に利用できる。

        }

        cv_.notify_one();

        return true;
    }

    bool TryPop(
        T& event)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (events_.empty())
        {

            // TODO: ログ追加候補
            // 空Queue参照の発生頻度を記録すると
            // ポーリング過多や負荷調査に利用できる。

            return false;
        }

        event = std::move(events_.front());

        events_.pop_front();

        return true;
    }

    bool WaitAndPop(
        T& event)
    {
        std::unique_lock<std::mutex>
            lock(mutex_);

        cv_.wait(
            lock,
            [this]
            {
                return shutdown_
                    || !events_.empty();
            });

        if (shutdown_
            && events_.empty())
        {

            // TODO: ログ追加候補
            // Shutdownによる待機解除を出力すると
            // Worker停止処理の確認に利用できる。

            return false;
        }

        event = std::move(events_.front());

        events_.pop_front();

        return true;
    }

    bool WaitAndPopFor(
        T& event,
        std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex>
            lock(mutex_);

        const bool ready =
            cv_.wait_for(
                lock,
                timeout,
                [this]
                {
                    return shutdown_
                        || !events_.empty();
                });

        if (!ready)
        {

            // TODO: ログ追加候補
            // Queue待機タイムアウト発生回数を記録すると
            // イベント流量の監視に利用できる。

            return false;
        }

        if (shutdown_
            && events_.empty())
        {
            return false;
        }

        event = std::move(events_.front());

        events_.pop_front();

        return true;
    }

    void Shutdown()
    {
        {
            std::lock_guard<std::mutex>
                lock(mutex_);

            shutdown_ = true;

            // TODO: ログ追加候補
            // Queue Shutdown開始を出力すると
            // 停止シーケンス確認に利用できる。
            
        }

        cv_.notify_all();
    }

private:

    std::mutex mutex_;

    std::condition_variable cv_;

    bool shutdown_{false};

    std::list<T>
        events_;

    TPolicy policy_;

};

}