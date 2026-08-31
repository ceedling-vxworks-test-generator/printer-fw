#pragma once

#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"
#include "NotificationRateLimiter.hpp"
#include "ProductContext.hpp"

namespace rim
{

class PublishManager
{
public:

    PublishManager(
        ChangeNotifyManager& notifyManager,
        PeriodicNotifyManager& periodicNotifyManager,
        SubscriptionStore& subscriptionStore,
        const ProductContext& context)
        : notifyManager_(notifyManager)
        , periodicNotifyManager_(periodicNotifyManager)
        , subscriptionStore_(subscriptionStore)
        , context_(context)
    {
    }

    void ProcessPeriodicNotifications();

    void Publish(const RIMId& target,NotificationTrigger trigger);

private:

    ChangeNotifyManager& notifyManager_;
    PeriodicNotifyManager& periodicNotifyManager_;
    SubscriptionStore& subscriptionStore_;
    NotificationRateLimiter rateLimiter_;
    const ProductContext& context_;
    
};

} // namespace rim
