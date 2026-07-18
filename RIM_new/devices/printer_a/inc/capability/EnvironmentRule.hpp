#pragma once

#include "capability/EnvironmentCapability.hpp"
#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class EnvironmentRule
{
public:

    EnvironmentCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        EnvironmentCapability cap{};

        cap.temperature =
            snapshot.temperature;

        cap.humidity =
            snapshot.humidity;

        return cap;
    }
};

} // namespace rim