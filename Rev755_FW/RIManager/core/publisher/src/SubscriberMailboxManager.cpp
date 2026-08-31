#include "SubscriberMailboxManager.hpp"

namespace rim
{

SubscriberMailbox&
SubscriberMailboxManager::GetMailbox(
    SubscriptionId id)
{
    // TODO: ログ追加候補
    // Mailbox新規生成時に SubscriptionId を出力すると
    // 購読数増加の確認に利用できる。
    // ※既存Mailbox取得時は不要。
    std::lock_guard<std::mutex> lock(mutex_);
    return mailboxes_[id];
}

uint64_t SubscriberMailboxManager::GetOverflowCount(
    SubscriptionId id)
{
    // TODO: ログ追加候補
    // Overflow件数取得時に SubscriptionId と件数を出力すると
    // 配送詰まり解析に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);

    auto it = mailboxes_.find(id);

    if (it == mailboxes_.end())
    {
        return 0;
    }

    return it->second.GetOverflowCount();
}
} // namespace rim