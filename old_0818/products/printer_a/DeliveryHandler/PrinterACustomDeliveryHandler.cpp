#include "DeliveryHandler/PrinterACustomDeliveryHandler.hpp"

namespace rim
{

bool PrinterACustomDeliveryHandler::Deliver(
    const SubscriptionInfo& info,
    const NotificationMessage& message)
{
    return true;
}

}
