#pragma once

#include "SubscriberMailbox.hpp"
#include "SubscriptionRegistry.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "ConsumableCapability.hpp"

namespace rim
{

class ChangeNotifyManager
{
public:

    ChangeNotifyManager(
        SubscriberMailbox& mailbox,
        CallbackSubscriptionRegistry& callbackRegistry)
        : mailbox_(mailbox)
        , callbackRegistry_(callbackRegistry)
    {
    }

    void Notify(
        const EnvironmentCapability& capability)
    {
        mailbox_.Push(
            capability);

        callbackRegistry_.NotifyEnvironment(
            capability);
    }

    void Notify(
        const ErrorCapability& capability)
    {
        mailbox_.Push(
            capability);

        callbackRegistry_.NotifyError(
            capability);
    }

    void Notify(
        const PrintReadyCapability& capability)
    {
        mailbox_.Push(
            capability);

        callbackRegistry_.NotifyPrintReady(
            capability);
    }

    void Notify(
        const ConsumableCapability& capability)
    {
        callbackRegistry_.
            NotifyConsumable(
                capability);
    }

    void Notify(
        const JobCapability& capability)
    {
        callbackRegistry_.
            NotifyJob(
                capability);
    }

private:

    SubscriberMailbox& mailbox_;

    CallbackSubscriptionRegistry&
        callbackRegistry_;
};

} // namespace rim