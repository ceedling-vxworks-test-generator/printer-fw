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

        // TODO: ログ追加候補
        // Target・Trigger・取得件数を出力すると
        // 通知対象選定結果の確認に利用できる。

        for (const auto& info : subscriptions){
            if ( info.method == DeliveryMethod::Mailbox ){
                const bool delivered =mailboxManager_.GetMailbox(info.id).Push({target, trigger});

                // TODO: ログ追加候補
                // Mailbox配送先SubscriptionIdと通知対象を出力すると
                // 通知経路の追跡が容易になる。

                if (!delivered)
                {
                    // TODO: ログ追加候補
                    // Mailbox Overflow発生時に
                    // SubscriptionIdと対象Targetを出力する。

                    ++mailboxOverflowCount_;
                }
            }
            else if ( info.method == DeliveryMethod::Callback ){
                callbackQueue_.Push({ info.id, target, trigger});

                // TODO: ログ追加候補
                // CallbackQueue投入時のSubscriptionIdとTargetを出力すると
                // 通知配送遅延の調査に利用できる。
            }
            else if ( info.method == DeliveryMethod::Custom )
            {
                if (customHandler_){
                    const bool delivered =
                        customHandler_->Deliver(info,{target, trigger});

                    if (!delivered){

                        // TODO: ログ追加候補
                        // Custom通知失敗時に
                        // SubscriptionIdとTargetを出力する。

                        ++customDeliveryFailureCount_;
                    }
                }

                // TODO: ログ追加候補
                // Custom通知要求が存在するにもかかわらず
                // Handler未設定の場合は警告ログを出力する。
                
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
    SubscriberMailboxManager& mailboxManager_;
    CallbackSubscriptionRegistry& callbackRegistry_;
    ICustomDeliveryHandler* customHandler_{nullptr};
    std::atomic<uint64_t> mailboxOverflowCount_{0};
    std::atomic<uint64_t> customDeliveryFailureCount_{0};
    CallbackQueue& callbackQueue_;

};

} // namespace rim