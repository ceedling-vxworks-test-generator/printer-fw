#pragma once

#include <vector>
#include <mutex>

#include "SubscriptionInfo.hpp"

namespace rim
{

class SubscriptionStore
{
public:

    SubscriptionId
    CreateSubscriptionId();

    void Register(
        const SubscriptionInfo& info);

    bool Find(
    SubscriptionId id,
    SubscriptionInfo& out) const;

    void GetSubscriptions(
    const RIMId& target,
    NotificationTrigger trigger,
    std::vector<SubscriptionInfo>& subscriptions) const;

private:

    SubscriptionId nextId_{1};
    std::vector<
        SubscriptionInfo>
            subscriptions_;
    mutable std::mutex mutex_;
    
};

}