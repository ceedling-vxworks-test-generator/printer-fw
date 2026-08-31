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
        // TODO: ログ追加候補
        // Notification取得失敗時に SubscriptionId を出力すると
        // Mailbox未配送や取得タイミング不一致の調査に利用できる。

        return mailboxManager_.GetMailbox(subscriptionId).Pop(message);
    }

    uint32_t GetMailboxCount(SubscriptionId id){

        // TODO: ログ追加候補
        // Mailbox件数取得時に SubscriptionId と件数を出力すると
        // 通知滞留状況の確認に利用できる。

        return mailboxManager_.GetMailbox(id).Count();
    }

private:

    SubscriberMailboxManager& mailboxManager_;
        
};

} // namespace rim