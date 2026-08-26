#pragma once

#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"
#include "NotificationRateLimiter.hpp"
#include "RouteProvider.hpp"

namespace rim
{

class PublishManager
{
public:

    PublishManager(
        ChangeNotifyManager& notifyManager,
        PeriodicNotifyManager& periodicNotifyManager,
        SubscriptionStore& subscriptionStore,
        RouteProvider& routeProvider)
        : notifyManager_(notifyManager)
        , periodicNotifyManager_(periodicNotifyManager)
        , subscriptionStore_(subscriptionStore)
        , routeProvider_(routeProvider)
    {
    }

    void ProcessPeriodicNotifications();

    void Publish(const NotificationTarget& target,NotificationTrigger trigger);

private:

    ChangeNotifyManager& notifyManager_;
    PeriodicNotifyManager& periodicNotifyManager_;
    SubscriptionStore& subscriptionStore_;
    NotificationRateLimiter rateLimiter_;
    RouteProvider& routeProvider_;
    
};

} // namespace rim
