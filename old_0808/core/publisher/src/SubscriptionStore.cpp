#include "SubscriptionStore.hpp"

namespace rim
{

SubscriptionId
SubscriptionStore::CreateSubscriptionId()
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    return nextId_++;
}

void SubscriptionStore::Register(
    const SubscriptionInfo& info)
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    subscriptions_.push_back(
        info);
}

bool SubscriptionStore::Find(
    SubscriptionId id,
    SubscriptionInfo& out) const
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    for (const auto& info
        : subscriptions_)
    {
        if (info.id == id)
        {
            out = info;
            return true;
        }
    }

    return false;
}

bool SubscriptionStore::Remove(
    SubscriptionId id)
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    for (auto it =
            subscriptions_.begin();
         it != subscriptions_.end();
         ++it)
    {
        if (it->id == id)
        {
            subscriptions_.erase(it);

            return true;
        }
    }

    return false;
}

void SubscriptionStore::GetSubscriptions(
    const NotificationTarget& target,
    NotificationTrigger trigger,
    std::vector<SubscriptionInfo>& subscriptions) const
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    subscriptions.clear();

    for (const auto& info
        : subscriptions_)
    {
        if (info.target.type != target.type)
        {
            continue;
        }

        if (info.target.id != target.id)
        {
            continue;
        }

        if (info.trigger != trigger)
        {
            continue;
        }

        subscriptions.push_back(
            info);
    }
}

} // namespace rim