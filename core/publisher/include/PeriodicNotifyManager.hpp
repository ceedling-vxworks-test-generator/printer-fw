#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <mutex>

#include "PeriodicCondition.hpp"

namespace rim
{

class PeriodicNotifyManager
{
public:

    void Register(
        const PeriodicCondition& condition);

    void GetDueConditions(
        std::vector<PeriodicCondition>& out)
        const;

    void UpdateNextTime(
        SubscriptionId subscriptionId);

private:

    std::unordered_map<
        SubscriptionId,
        PeriodicCondition>
            conditions_;

    mutable std::mutex mutex_;
};

} // namespace rim