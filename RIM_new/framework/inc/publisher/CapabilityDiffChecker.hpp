#pragma once

#include "capability/EnvironmentCapability.hpp"
#include "capability/ErrorCapability.hpp"
#include "capability/PrintReadyCapability.hpp"

namespace rim
{

class CapabilityDiffChecker
{
public:

    static bool HasChanged(
        const EnvironmentCapability& lhs,
        const EnvironmentCapability& rhs)
    {
        return
            lhs.temperature != rhs.temperature ||
            lhs.humidity != rhs.humidity;
    }

    static bool HasChanged(
        const ErrorCapability& lhs,
        const ErrorCapability& rhs)
    {
        return
            lhs.upperDoorOpen != rhs.upperDoorOpen ||
            lhs.rightDoorOpen != rhs.rightDoorOpen ||
            lhs.leftDoorOpen != rhs.leftDoorOpen;
    }

    static bool HasChanged(
        const PrintReadyCapability& lhs,
        const PrintReadyCapability& rhs)
    {
        return lhs.ready != rhs.ready;
    }
};

} // namespace rim