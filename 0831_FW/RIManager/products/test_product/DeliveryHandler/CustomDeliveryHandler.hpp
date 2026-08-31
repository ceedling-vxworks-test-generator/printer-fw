#pragma once

#include "ICustomDeliveryHandler.hpp"

namespace rim
{

class CustomDeliveryHandler
    : public ICustomDeliveryHandler
{
public:

    bool Deliver(
        const SubscriptionInfo& info,
        const NotificationMessage& message)
        override;

private:

};

}