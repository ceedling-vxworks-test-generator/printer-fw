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

uint64_t SubscriberMailboxManager::GetOverflowCount(
    SubscriptionId id)
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    auto it =
        mailboxes_.find(id);

    if (it == mailboxes_.end())
    {
        return 0;
    }

    return
        it->second
            .GetOverflowCount();
}
} // namespace rim