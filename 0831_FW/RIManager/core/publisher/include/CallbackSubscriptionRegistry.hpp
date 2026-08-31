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

    void Subscribe(
        SubscriptionId id,
        NotificationCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        notifications_[id] =
            std::move(callback);

        // TODO: ログ追加候補
        // 購読登録時に SubscriptionId と登録済み件数を出力すると
        // 購読状況の追跡とメモリ増加調査に利用できる。
    }

    void Notify(
        SubscriptionId id,
        const NotificationMessage& message)
    {
        NotificationCallback callback;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            auto it = notifications_.find(id);

            // TODO: ログ追加候補
            // 対応する SubscriptionId が見つからなかった場合は
            // 通知経路不整合の調査用ログを出力できる。

            if (it != notifications_.end())
            {
                callback = it->second;
            }
        }

        if (callback)
        {
            callback(id,message);
        }
        // TODO: ログ追加候補
        // コールバック実行回数や対象Targetを出力すると
        // 通知頻度の分析に利用できる。
    }

private:

    std::unordered_map<
    SubscriptionId,
    NotificationCallback>
        notifications_;

    mutable std::mutex mutex_;
};

} // namespace rim