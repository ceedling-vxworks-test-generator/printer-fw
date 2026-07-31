#pragma once

#include <functional>

#include "SubscriptionId.hpp"

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"

namespace rim
{

using EnvironmentCallback =
    std::function<void(
        SubscriptionId,
        const EnvironmentCapability&)>;

using ErrorCallback =
    std::function<
        void(
            SubscriptionId,
            const ErrorCapability&)>;
            
using PrintReadyCallback =
    std::function<
        void(
            SubscriptionId,
            const PrintReadyCapability&)>;            

using ConsumableCallback =
    std::function<
        void(
            SubscriptionId,
            const ConsumableCapability&)>;

using JobCallback =
    std::function<
        void(
            SubscriptionId,
            const JobCapability&)>;
}