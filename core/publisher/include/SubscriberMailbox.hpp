#pragma once

#include <queue>
#include <mutex>

#include "NotificationMessage.hpp"
#include "rim_capability_id.h"

namespace rim
{

class SubscriberMailbox
{
public:

    void Push(
        const NotificationMessage& message)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        notificationQueue_.push(
            message);
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
    
private:

    mutable std::mutex
        mutex_;

    std::queue<NotificationMessage>
        notificationQueue_;

};

} // namespace rim