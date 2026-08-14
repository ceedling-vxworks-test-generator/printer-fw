#pragma once

#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"

namespace rim
{

class ChangeNotifyManager
{
public:

    ChangeNotifyManager(
    SubscriptionStore& subscriptionStore,
    SubscriberMailboxManager& mailboxManager,
    CallbackSubscriptionRegistry& callbackRegistry)
    : store_(subscriptionStore)
    , mailboxManager_(mailboxManager)
    , callbackRegistry_(callbackRegistry)
    {
    }

    void Notify(const NotificationTarget& target,NotificationTrigger trigger){
        std::vector<SubscriptionInfo>
            subscriptions;

        store_.GetSubscriptions(target,trigger,subscriptions);

        for (const auto& info : subscriptions){
            if (info.method ==DeliveryMethod::Mailbox){
                mailboxManager_.GetMailbox(info.id).Push({target,trigger});
            }
            else if (info.method == DeliveryMethod::Callback){
                callbackRegistry_.Notify(info.id,{target,trigger});
            }
        }
    }

private:

    SubscriptionStore& store_;

    SubscriberMailboxManager&
        mailboxManager_;

    CallbackSubscriptionRegistry&
        callbackRegistry_;
};

} // namespace rim