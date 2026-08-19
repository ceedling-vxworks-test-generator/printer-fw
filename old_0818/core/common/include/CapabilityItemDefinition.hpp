#pragma once

#include <cstddef>
#include <string_view>
#include <any>

#include "RIId.hpp"
#include "ValueType.hpp"

namespace rim
{

class RIMSnapshot;

using CapabilityBuildCallback =
    std::any (*)(
        const RIMSnapshot& snapshot);

using CapabilityCompareCallback =
    bool (*)(
        const std::any& oldValue,
        const std::any& newValue);

struct CapabilityItemDefinition
{
    RICapabilityId id;

    std::string_view name;

    ValueType valueType;

    CapabilityBuildCallback build;

    CapabilityCompareCallback compare;

    const RIDataId* requiredDataIds;

    std::size_t requiredDataCount;

    std::string_view route;
};

} // namespace rim