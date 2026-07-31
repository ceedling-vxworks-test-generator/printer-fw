#pragma once

#include <cstdint>

#include "ConsumableCapability.hpp"
#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class ConsumableRule
{
public:

    ConsumableCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        ConsumableCapability capability{};

        std::int32_t stapleLevel{};
        std::int32_t tonerLevel{};

        snapshot.TryGetInt32(
            RIMDataId::kStapleLevel,
            stapleLevel);

        snapshot.TryGetInt32(
            RIMDataId::kTonerLevel,
            tonerLevel);

        capability.stapleLevel =
            stapleLevel;

        capability.tonerLevel =
            NormalizeToner(
                tonerLevel);

        return capability;
    }

private:

    int NormalizeToner(
        int value) const
    {
        return value;
    }
};

} // namespace rim