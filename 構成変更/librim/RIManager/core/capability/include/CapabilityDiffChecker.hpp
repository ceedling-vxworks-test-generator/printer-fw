#pragma once

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"

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
        return lhs.errors
            != rhs.errors;
    }

    static bool HasChanged(
        const PrintReadyCapability& lhs,
        const PrintReadyCapability& rhs)
    {
        return lhs.ready != rhs.ready;
    }

    static bool HasChanged(
        const ConsumableCapability& lhs,
        const ConsumableCapability& rhs)
    {
        return
            lhs.stapleLevel
                != rhs.stapleLevel
            ||
            lhs.tonerLevel
                != rhs.tonerLevel;
    }

    static bool HasChanged(
        const JobCapability& lhs,
        const JobCapability& rhs)
    {
        return
            lhs.jobActive
                != rhs.jobActive
            ||
            lhs.jobId
                != rhs.jobId;
    }

};

} // namespace rim