#pragma once

#include "SubscriptionId.hpp"
#include "DeliveryMethod.hpp"
#include "NotificationTrigger.hpp"
#include "RIMId.hpp"

namespace rim
{

struct SubscriptionInfo
{
    SubscriptionId id;
    RIMId target;      //type + ID
    DeliveryMethod method;          //mailBox か callback か カスタム(別途CustomDeliveryHandlerへの実装が必要)
    NotificationTrigger trigger;    //定期通知 か 変更通知
};

}