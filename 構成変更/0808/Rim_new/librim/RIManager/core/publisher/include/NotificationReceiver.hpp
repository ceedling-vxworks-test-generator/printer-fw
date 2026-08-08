#pragma once

#include "SubscriberMailboxManager.hpp"
#include "SubscriptionId.hpp"
#include "NotificationMessage.hpp"

namespace rim
{

class NotificationReceiver
{
public:

    explicit NotificationReceiver(
        SubscriberMailboxManager& mailboxManager)
        : mailboxManager_(mailboxManager)
    {
    }

    bool TryGetNotification(
        SubscriptionId subscriptionId,
        NotificationMessage& message)
    {
        return mailboxManager_
            .GetMailbox(
                subscriptionId)
            .Pop(
                message);
    }

    uint32_t GetMailboxCount(SubscriptionId id){
        return mailboxManager_
            .GetMailbox(id)
            .Count();
    }

private:

    SubscriberMailboxManager&
        mailboxManager_;
        
};

} // namespace rim