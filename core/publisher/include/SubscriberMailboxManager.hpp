#pragma once

#include <unordered_map>
#include <mutex>

#include "SubscriptionId.hpp"
#include "SubscriberMailbox.hpp"

namespace rim
{

class SubscriberMailboxManager
{
public:

    SubscriberMailbox&
    GetMailbox(
        SubscriptionId id);

    uint64_t GetOverflowCount(
    SubscriptionId id);

private:

    std::unordered_map
    <SubscriptionId,
     SubscriberMailbox> 
    mailboxes_;

    mutable std::mutex mutex_;
};

}