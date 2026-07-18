#pragma once

#include <cstdint>
#include <unordered_set>

namespace rim
{

class SubscriptionRegistry
{
public:

    bool Subscribe(
        std::uint32_t subscriberId)
    {
        return
            subscribers_.insert(
                subscriberId).second;
    }

    bool Unsubscribe(
        std::uint32_t subscriberId)
    {
        return
            subscribers_.erase(
                subscriberId)
            != 0U;
    }

    bool IsSubscribed(
        std::uint32_t subscriberId) const
    {
        return
            subscribers_.find(
                subscriberId)
            != subscribers_.end();
    }

private:

    std::unordered_set<
        std::uint32_t>
        subscribers_;
};

}