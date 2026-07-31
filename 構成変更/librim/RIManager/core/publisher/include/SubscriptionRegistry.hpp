#pragma once

#include <cstdint>
#include <unordered_set>

#include "SubscriptionId.hpp"

namespace rim
{

class SubscriptionRegistry
{
public:

    SubscriptionId Subscribe()
    {
        const auto id =
            nextId_++;

        subscribers_.insert(
            id);

        return id;
    }

    bool Unsubscribe(
        SubscriptionId id)
    {
        return
            subscribers_.erase(id)
            != 0U;
    }

    bool IsSubscribed(
        SubscriptionId id) const
    {
        return
            subscribers_.find(id)
            != subscribers_.end();
    }

private:

    SubscriptionId
        nextId_{1};

    std::unordered_set<
        SubscriptionId>
        subscribers_;
};

}