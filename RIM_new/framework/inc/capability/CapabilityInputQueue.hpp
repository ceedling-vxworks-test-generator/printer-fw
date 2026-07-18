#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class CapabilityInputQueue
{
public:

    void Push(
        const RIMSnapshot& snapshot)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        queue_.push(
            snapshot);

        cv_.notify_one();

    }

    bool TryPop(
        RIMSnapshot& snapshot)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (queue_.empty())
        {
            return false;
        }

        snapshot =
            queue_.front();

        queue_.pop();

        return true;
    }

    bool WaitAndPop(
        RIMSnapshot& snapshot)
    {
        std::unique_lock<std::mutex>
            lock(mutex_);

        cv_.wait(
            lock,
            [this]
            {
                return shutdown_
                    || !queue_.empty();
            });

        if (shutdown_
            && queue_.empty())
        {
            return false;
        }

        snapshot =
            queue_.front();

        queue_.pop();

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

    std::queue<RIMSnapshot>
        queue_;
};

}