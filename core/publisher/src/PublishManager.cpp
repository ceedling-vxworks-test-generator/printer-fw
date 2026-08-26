#include "PublishManager.hpp"
#include "RouteDefinition.hpp"

namespace rim
{

void PublishManager::
    ProcessPeriodicNotifications()
{
    std::vector<PeriodicCondition> due;

    periodicNotifyManager_.GetDueConditions(due);

    // TODO: ログ追加候補
    // 定期通知処理開始時に Due件数を出力すると
    // 定期通知実行状況の確認に利用できる。
    
            for (const auto& condition : due)
            {
                SubscriptionInfo info{};
                if (!subscriptionStore_.Find( condition.subscriptionId,info )){
                        continue;
                    }

                Publish( info.target,NotificationTrigger::Periodic );
                periodicNotifyManager_.UpdateNextTime( condition.subscriptionId );
            }
}

void PublishManager::Publish(
    const NotificationTarget& target,
    NotificationTrigger trigger)
{
    // TODO: ログ追加候補
    // Publish要求時に TargetType、TargetId、Trigger を出力すると
    // 通知発行経路の追跡に利用できる。

    if (trigger == NotificationTrigger::OnChange)
    {
        const auto* route = context_.ResolveRoute(target);

        if (route == nullptr)
        {
            // TODO: ログ追加候補
            // Route未定義のTargetを検出した場合に
            // TargetTypeとTargetIdを出力する。
            
            return;
        }

        // TODO: ログ追加候補
        // RateLimitによる通知抑制時に
        // TargetType、TargetId、最小通知間隔を出力する。

        if (!rateLimiter_.ShouldNotify(
                target,
                route->minimumNotifyIntervalMs))
        {
            return;
        }

    }

    notifyManager_.Notify( target,trigger );
}

} // namespace rim