#pragma once

#include "publisher/SubscriberMailbox.hpp"
#include "publisher/SubscriptionRegistry.hpp"
#include "publisher/CallbackSubscriptionRegistry.hpp"

namespace rim
{

class ChangeNotifyManager
{
public:

    ChangeNotifyManager(
        SubscriptionRegistry& registry,
        SubscriberMailbox& mailbox,
        CallbackSubscriptionRegistry& callbackRegistry)
        : registry_(registry)
        , mailbox_(mailbox)
        , callbackRegistry_(callbackRegistry)
    {
    }
    
    void Notify(
        const EnvironmentCapability& capability)
    {
        if (!registry_.IsSubscribed(1))
        {
            return;
        }

        mailbox_.Push(
            capability);

        callbackRegistry_.NotifyEnvironment(
            capability);
    }

    void Notify(
        const ErrorCapability& capability)
    {
        if (!registry_.IsSubscribed(1))
        {
            return;
        }

        mailbox_.Push(
            capability);
    }

    void Notify(
        const PrintReadyCapability& capability)
    {
        if (!registry_.IsSubscribed(1))
        {
            return;
        }

        mailbox_.Push(
            capability);
    }
private:

    SubscriptionRegistry& registry_;

    SubscriberMailbox& mailbox_;

    CallbackSubscriptionRegistry&
        callbackRegistry_;
};

} // namespace rim