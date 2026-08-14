#include "SubscriberMailboxManager.hpp"

namespace rim
{

SubscriberMailbox&
SubscriberMailboxManager::GetMailbox(
    SubscriptionId id)
{
    return mailboxes_[id];
}

bool SubscriberMailboxManager::RemoveMailbox(
    SubscriptionId id)
{
    std::lock_guard<std::mutex>
        lock(mutex_);
    return mailboxes_.erase(id) > 0U;
}
} // namespace rim