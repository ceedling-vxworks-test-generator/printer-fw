#pragma once

#include <cstddef>
#include <string_view>
#include <any>

#include "RIId.hpp"
#include "ValueType.hpp"
#include "RIMValue.hpp"

namespace rim
{

class RIMSnapshot;

using FacadeBuildCallback =
    RIMValue (*)(
        const RIMSnapshot& snapshot);

using FacadeDiffCallback =
    bool (*)(
        const RIMValue& oldValue,
        const RIMValue& newValue);

struct FacadeItemDefinition
{
    RIFacadeId id;
    std::string_view name;
    ValueType valueType;
    RIMValue initialValue;
    FacadeBuildCallback build;
    FacadeDiffCallback diff;
    const RICapabilityId* requiredCapabilityIds;
    std::size_t requiredCapabilityCount;
    std::uint32_t routeId;
};

} // namespace rim