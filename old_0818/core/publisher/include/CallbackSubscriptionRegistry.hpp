#pragma once

#include <unordered_map>
#include <mutex>

#include "CallbackSubscriber.hpp"

namespace rim
{
struct NotificationSubscription
{

    SubscriptionId id;
    NotificationCallback callback;
    
};

class CallbackSubscriptionRegistry
{
public:

    bool Unsubscribe(
        SubscriptionId id)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        return
            notifications_.erase(id)
            > 0U;
    }

    void Subscribe(
        SubscriptionId id,
        NotificationCallback callback)
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        notifications_[id] =
            std::move(callback);
    }

    void Notify(
        SubscriptionId id,
        const NotificationMessage& message)
    {
        NotificationCallback callback;

        {
            std::lock_guard<std::mutex>
                lock(mutex_);

            auto it =
                notifications_.find(id);

            if (it != notifications_.end())
            {
                callback =
                    it->second;
            }
        }

        if (callback)
        {
            callback(
                id,
                message);
        }
    }

private:

    std::unordered_map<
    SubscriptionId,
    NotificationCallback>
        notifications_;

    mutable std::mutex
    mutex_;
};

} // namespace rim