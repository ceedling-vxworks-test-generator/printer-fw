#pragma once

#include <queue>
#include <mutex>

#include "NotificationMessage.hpp"

namespace rim
{

class SubscriberMailbox
{
public:

    explicit SubscriberMailbox(
    std::size_t maxSize = 1000)
    : maxSize_(maxSize)
    {
    }

    bool Push(
        const NotificationMessage& message)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (notificationQueue_.size() >= maxSize_)
        {
            ++overflowCount_;
            return false;
        }

        notificationQueue_.push(
            message);

        return true;
    }

    bool Pop(
        NotificationMessage& message)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        if (notificationQueue_.empty())
        {
            return false;
        }

        message =
            notificationQueue_.front();

        notificationQueue_.pop();

        return true;
    }

    uint32_t Count() const
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        return static_cast<uint32_t>(
            notificationQueue_.size());
    }

    uint64_t GetOverflowCount() const
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        return overflowCount_;
    }
    
private:
    std::size_t maxSize_;

    uint64_t overflowCount_{0};

    mutable std::mutex mutex_;

    std::queue<NotificationMessage>
        notificationQueue_;

};

} // namespace rim