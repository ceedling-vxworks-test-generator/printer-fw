#pragma once

#include "SubscriptionInfo.hpp"
#include "NotificationMessage.hpp"

namespace rim
{

class ICustomDeliveryHandler
{
public:

    virtual ~ICustomDeliveryHandler() = default;

    virtual bool Deliver(
        const SubscriptionInfo& info,
        const NotificationMessage& message) = 0;
};

}