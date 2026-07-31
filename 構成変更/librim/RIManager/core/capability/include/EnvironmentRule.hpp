#pragma once

#include "EnvironmentCapability.hpp"
#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class EnvironmentRule
{
public:

    EnvironmentCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        EnvironmentCapability cap{};

        snapshot.TryGetDouble(
            RIMDataId::kTemperatureSensorA,
            cap.temperature);

        snapshot.TryGetDouble(
            RIMDataId::kHumiditySensor,
            cap.humidity);

        return cap;
    }
};

} // namespace rim