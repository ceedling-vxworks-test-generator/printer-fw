#pragma once

#include <mutex>
#include <list>
#include <chrono>
#include <condition_variable>

namespace rim
{

template<
    typename T,
    typename TPolicy>
class EventQueue
{
public:

    void Push(
        const T& event)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        policy_.Insert(
            events_,event);

        cv_.notify_one();
    }

    bool TryPop(
        T& event)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (events_.empty())
        {
            return false;
        }

        event =
            events_.front();

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
            return false;
        }

        event =
            events_.front();

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
            return false;
        }

        if (shutdown_
            && events_.empty())
        {
            return false;
        }

        event =
            events_.front();

        events_.pop_front();

        return true;
    }

    void Shutdown()
    {
        {
            std::lock_guard<std::mutex>
                lock(mutex_);

            shutdown_ = true;
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