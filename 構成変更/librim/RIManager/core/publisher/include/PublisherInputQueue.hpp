#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

#include "CapabilityChangeSet.hpp"

namespace rim
{

class PublisherInputQueue
{
public:

    void Push(
        const CapabilityChangeSet& input)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        queue_.push(
            input);

        cv_.notify_one();
    }

    bool TryPop(
        CapabilityChangeSet& input)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (queue_.empty())
        {
            return false;
        }

        input =
            queue_.front();

        queue_.pop();

        return true;
    }

    bool WaitAndPop(
        CapabilityChangeSet& input)
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

        input =
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

    std::queue<CapabilityChangeSet>
        queue_;
};

} // namespace rim