#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

#include "publisher/PublisherInput.hpp"

namespace rim
{

class PublisherInputQueue
{
public:

    void Push(
        const PublisherInput& input)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        queue_.push(
            input);

        cv_.notify_one();
    }

    bool TryPop(
        PublisherInput& input)
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
        PublisherInput& input)
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

    std::queue<PublisherInput>
        queue_;
};

} // namespace rim
