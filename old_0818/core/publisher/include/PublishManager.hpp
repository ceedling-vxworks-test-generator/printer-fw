#pragma once

#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"
#include "NotificationRateLimiter.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class PublishManager
{
public:

    PublishManager(
        ChangeNotifyManager& notifyManager,
        PeriodicNotifyManager& periodicNotifyManager,
        SubscriptionStore& subscriptionStore,
        const ProductDefinition& product)
        : notifyManager_(notifyManager)
        , periodicNotifyManager_(periodicNotifyManager)
        , subscriptionStore_(subscriptionStore)
        ,product_(product)
    {
    }

    void ProcessPeriodicNotifications();

    void Publish(const NotificationTarget& target,NotificationTrigger trigger);

private:

    ChangeNotifyManager& notifyManager_;

    PeriodicNotifyManager& periodicNotifyManager_;
    
    SubscriptionStore& subscriptionStore_;
    
    NotificationRateLimiter rateLimiter_;

    const ProductDefinition& product_;
    
};

} // namespace rim