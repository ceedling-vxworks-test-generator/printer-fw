#pragma once

#include <atomic>

#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "ICustomDeliveryHandler.hpp"
#include "CallbackQueue.hpp"

namespace rim
{

class ChangeNotifyManager
{
public:

    ChangeNotifyManager(
        SubscriptionStore& subscriptionStore,
        SubscriberMailboxManager& mailboxManager,
        CallbackSubscriptionRegistry& callbackRegistry,
        CallbackQueue& callbackQueue)
        : store_(subscriptionStore)
        , mailboxManager_(mailboxManager)
        , callbackRegistry_(callbackRegistry)
        , callbackQueue_(callbackQueue)
    {
    }

    void Notify(const NotificationTarget& target,NotificationTrigger trigger){
        std::vector<SubscriptionInfo>
            subscriptions;

        store_.GetSubscriptions(target,trigger,subscriptions);

        for (const auto& info : subscriptions){
            if ( info.method == DeliveryMethod::Mailbox ){
                const bool delivered =mailboxManager_.GetMailbox(info.id).Push({target, trigger});
                if (!delivered)
                {
                    // Mailbox Overflow
                    ++mailboxOverflowCount_;
                }
            }
            else if ( info.method == DeliveryMethod::Callback ){
                callbackQueue_.Push({ info.id, target, trigger});
            }
            else if ( info.method == DeliveryMethod::Custom )
            {
                if (customHandler_){
                    const bool delivered =
                        customHandler_->Deliver(info,{target, trigger});

                    if (!delivered){
                        ++customDeliveryFailureCount_;
                    }
                }
            }
        }
    }

    uint64_t GetMailboxOverflowCount() const
    {
        return mailboxOverflowCount_;
    }

    void SetCustomDeliveryHandler(
        ICustomDeliveryHandler* handler)
    {
        customHandler_ = handler;
    }

    uint64_t GetCustomDeliveryFailureCount() const
    {
        return customDeliveryFailureCount_.load();
    }

private:

    SubscriptionStore& store_;

    SubscriberMailboxManager&
        mailboxManager_;

    CallbackSubscriptionRegistry&
        callbackRegistry_;
    
    ICustomDeliveryHandler*
        customHandler_{nullptr};

    std::atomic<uint64_t> mailboxOverflowCount_{0};
    std::atomic<uint64_t> customDeliveryFailureCount_{0};
    CallbackQueue& callbackQueue_;
};

} // namespace rim