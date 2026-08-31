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
    const RIMId& target,
    NotificationTrigger trigger)
{
    // TODO: ログ追加候補
    // Publish要求時に TargetType、TargetId、Trigger を出力すると
    // 通知発行経路の追跡に利用できる。

    if (trigger == NotificationTrigger::OnChange)
    {
        std::uint32_t minimumNotifyIntervalMs = 0;

        switch (target.type)
        {
        case RIMIdType::Data:
        {
            const auto* item = context_.FindDataItem(target);
            if (item == nullptr)
            {
                return;
            }
            minimumNotifyIntervalMs = item->MinimumNotifyIntervalMs;
            break;
        }

        case RIMIdType::Capability:
        {
            const auto* capability = context_.FindCapability(target);
            if (capability == nullptr)
            {
                return;
            }
            minimumNotifyIntervalMs = capability->MinimumNotifyIntervalMs;
            break;
        }

        case RIMIdType::Facade:
        {
            const auto* facade = context_.FindFacade(target);
            if (facade == nullptr)
            {
                return;
            }
            minimumNotifyIntervalMs = facade->MinimumNotifyIntervalMs;
            break;
        }

        default:
            return;
        }

        // TODO: ログ追加候補
        // RateLimitによる通知抑制時に
        // TargetType、TargetId、最小通知間隔を出力する。

        if (!rateLimiter_.ShouldNotify(target,minimumNotifyIntervalMs))
        {
            return;
        }

    }

    notifyManager_.Notify( target,trigger );
}

} // namespace rim