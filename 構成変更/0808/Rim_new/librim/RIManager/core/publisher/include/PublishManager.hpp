#pragma once

#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

namespace rim
{

class PublishManager
{
public:

    PublishManager(
        ChangeNotifyManager& notifyManager,
        PeriodicNotifyManager& periodicNotifyManager,
        SubscriptionStore& subscriptionStore)
        : notifyManager_(notifyManager)
        , periodicNotifyManager_(periodicNotifyManager)
        , subscriptionStore_(subscriptionStore)
    {
    }

    void ProcessPeriodicNotifications();

    void Publish(const NotificationTarget& target,NotificationTrigger trigger);

private:

    ChangeNotifyManager& notifyManager_;

    PeriodicNotifyManager& periodicNotifyManager_;
    
    SubscriptionStore& subscriptionStore_;
    
};

} // namespace rim