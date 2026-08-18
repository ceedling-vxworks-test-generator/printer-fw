#include "PublishManager.hpp"

namespace rim
{

void PublishManager::
    ProcessPeriodicNotifications()
{
    std::vector<PeriodicCondition>
        due;

    periodicNotifyManager_.
        GetDueConditions(
            due);
    
            for (const auto& condition : due)
            {
                SubscriptionInfo info{};
                if (!subscriptionStore_.Find(
                    condition.subscriptionId,info)){
                        continue;
                    }

                Publish(info.target,NotificationTrigger::Periodic);
                periodicNotifyManager_.UpdateNextTime(condition.subscriptionId);
            }
}

void PublishManager::Publish(
    const NotificationTarget& target,
    NotificationTrigger trigger)
{
    notifyManager_.Notify(
        target,
        trigger);
}

} // namespace rim