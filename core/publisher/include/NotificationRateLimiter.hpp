#pragma once

#include <chrono>
#include <mutex>
#include <unordered_map>

#include "NotificationTargetKey.hpp"

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

        const auto now = std::chrono::steady_clock::now();

        std::lock_guard<std::mutex> lock(mutex_);

        auto key = MakeNotificationTargetKey(target);

        auto it = lastNotifyTimes_.find(key);

        if (it != lastNotifyTimes_.end())
        {
            if (now - it->second < minimumInterval)
            {
                // TODO: ログ追加候補
                // 通知抑制発生時に TargetType、TargetId、
                // minimumNotifyIntervalMs を出力すると
                // 通知が来ない事象の調査に利用できる。

                return false;
            }
        }

        lastNotifyTimes_[key] = now;

        // TODO: ログ追加候補
        // RateLimit通過時の TargetType、TargetId を出力すると
        // 通知送信履歴の追跡に利用できる。

        return true;
    }

private:

    std::mutex mutex_;
    std::unordered_map<
    std::uint64_t,
    std::chrono::steady_clock::time_point>
    lastNotifyTimes_;
};

}