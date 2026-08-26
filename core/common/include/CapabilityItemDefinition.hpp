#pragma once

#include <cstddef>
#include <string_view>

#include "RIId.hpp"
#include "ValueType.hpp"
#include "RIMValue.hpp"

namespace rim
{

class RIMSnapshot;

using CapabilityBuildCallback =
    RIMValue (*)(
        const RIMSnapshot& snapshot);

using CapabilityDiffCallback =
    bool (*)(
        const RIMValue& oldValue,
        const RIMValue& newValue);

struct CapabilityItemDefinition
{
    RICapabilityId id;
    std::string_view name;
    ValueType valueType;
    RIMValue initialValue;
    CapabilityBuildCallback build;
    CapabilityDiffCallback diff;
    const RIDataId* requiredDataIds;
    std::size_t requiredDataCount;
    // std::string_view route;
    std::uint32_t routeId;
};

} // namespace rim