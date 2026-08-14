#pragma once

#include <vector>
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
        bool removed = false;

        for (auto it = notifications_.begin();
            it != notifications_.end();)
        {
            if (it->id == id)
            {
                it = notifications_.erase(it);
                removed = true;
            }
            else
            {
                ++it;
            }
        }

        return removed;
    }

    void Subscribe(
        SubscriptionId id,
        NotificationCallback callback)
    {
        std::lock_guard<std::mutex>
        lock(mutex_);
        notifications_.push_back(
        {
            id,
            std::move(callback)
        });
    }

    void Notify(
        SubscriptionId id,
        const NotificationMessage& message)
    {
        NotificationCallback callback;

        {
            std::lock_guard<std::mutex>
                lock(mutex_);

            for (const auto& entry
                : notifications_)
            {
                if (entry.id != id)
                {
                    continue;
                }

                callback =
                    entry.callback;

                break;
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

    std::vector<
    NotificationSubscription>
        notifications_;

    mutable std::mutex
    mutex_;
};

} // namespace rim