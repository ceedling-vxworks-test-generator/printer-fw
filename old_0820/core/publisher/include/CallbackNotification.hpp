#pragma once

#include "SubscriptionId.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTrigger.hpp"

namespace rim
{

struct CallbackNotification
{
    SubscriptionId subscriptionId;

    NotificationTarget target;

    NotificationTrigger trigger;
};

}