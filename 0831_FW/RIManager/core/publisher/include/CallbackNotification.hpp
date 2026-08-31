#pragma once

#include "SubscriptionId.hpp"
#include "RIMId.hpp"
#include "NotificationTrigger.hpp"

namespace rim
{
    struct CallbackNotification
    {
        SubscriptionId subscriptionId;
        RIMId target;
        NotificationTrigger trigger;
    };
}