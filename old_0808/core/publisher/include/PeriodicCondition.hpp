#pragma once

#include <chrono>
#include <cstdint>

#include "SubscriptionId.hpp"

namespace rim
{

struct PeriodicCondition
{

    SubscriptionId subscriptionId;
    std::chrono::milliseconds interval;
    std::chrono::steady_clock::time_point nextNotifyTime;
    
};

} // namespace rim