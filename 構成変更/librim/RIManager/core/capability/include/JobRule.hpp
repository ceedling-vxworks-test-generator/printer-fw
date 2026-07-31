#pragma once

#include <cstdint>

#include "JobCapability.hpp"
#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class JobRule
{
public:

    JobCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        JobCapability capability{};

        bool jobActive{};
        std::int32_t jobId{};

        snapshot.TryGetBool(
            RIMDataId::kJobActive,
            jobActive);

        snapshot.TryGetInt32(
            RIMDataId::kJobId,
            jobId);

        capability.jobActive =
            jobActive;

        capability.jobId =
            jobId;

        return capability;
    }
};

} // namespace rim