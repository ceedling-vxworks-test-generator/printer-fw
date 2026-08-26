#pragma once

#include "SubscriptionId.hpp"
#include "DeliveryMethod.hpp"
#include "NotificationTrigger.hpp"
#include "NotificationTarget.hpp"

namespace rim
{

struct SubscriptionInfo
{

    SubscriptionId id;
    NotificationTarget target;//type + ID
    DeliveryMethod method;//mailBoxかcallback
    NotificationTrigger trigger;//定期通知か変更通知か
    
};

}