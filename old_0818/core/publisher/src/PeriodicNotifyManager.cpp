#include "PeriodicNotifyManager.hpp"

#include <chrono>

namespace rim
{

void PeriodicNotifyManager::Register(
    const PeriodicCondition& condition)
{
    std::lock_guard<std::mutex>
    lock(mutex_);
    conditions_[condition.subscriptionId] = condition;
}

bool PeriodicNotifyManager::Remove(
    SubscriptionId subscriptionId)
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    return
        conditions_.erase(
            subscriptionId)
        > 0;
}

void PeriodicNotifyManager::GetDueConditions(
    std::vector<PeriodicCondition>& out)
    const
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    out.clear();

    const auto now =
        std::chrono::steady_clock::now();

    for (const auto& pair
        : conditions_)
    {
        const auto& condition =
            pair.second;

        if (condition.nextNotifyTime
            <= now)
        {
            out.push_back(
                condition);
        }
    }
}

void PeriodicNotifyManager::UpdateNextTime(
    SubscriptionId subscriptionId)
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    auto it =
        conditions_.find(
            subscriptionId);

    if (it ==
        conditions_.end())
    {
        return;
    }

    it->second.nextNotifyTime +=
        it->second.interval;
}

} // namespace rim