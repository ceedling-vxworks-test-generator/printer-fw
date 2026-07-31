#pragma once

#include "SubscriberMailbox.hpp"

namespace rim
{

class NotificationReceiver
{
public:

    explicit NotificationReceiver(
        SubscriberMailbox& mailbox)
        : mailbox_(mailbox)
    {
    }

    bool TryGetEnvironment(
        EnvironmentCapability& capability)
    {
        return mailbox_.Pop(
            capability);
    }

    bool TryGetError(
        ErrorCapability& capability)
    {
        return mailbox_.Pop(
            capability);
    }

    bool TryGetPrintReady(
        PrintReadyCapability& capability)
    {
        return mailbox_.Pop(
            capability);
    }

private:

    SubscriberMailbox& mailbox_;
};

} // namespace rim