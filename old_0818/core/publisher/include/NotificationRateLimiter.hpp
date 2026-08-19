#pragma once

#include <chrono>
#include <mutex>
#include <unordered_map>

#include "NotificationTarget.hpp"

namespace rim
{

class NotificationRateLimiter
{
public:

    bool ShouldNotify(
        const NotificationTarget& target,
        std::uint32_t minimumNotifyIntervalMs)
    {
        const auto minimumInterval =
            std::chrono::milliseconds(
                minimumNotifyIntervalMs);

        const auto now =
            std::chrono::steady_clock::now();

        std::lock_guard<std::mutex>
            lock(mutex_);

        auto it =
            lastNotifyTimes_.find(
                target.id);

        if (it !=
            lastNotifyTimes_.end())
        {
            if (now - it->second <
                minimumInterval)
            {
                return false;
            }
        }

        lastNotifyTimes_[target.id] =
            now;

        return true;
    }

private:

    std::mutex
        mutex_;

    std::unordered_map<
        uint32_t,
        std::chrono::steady_clock::time_point>
        lastNotifyTimes_;
};

}