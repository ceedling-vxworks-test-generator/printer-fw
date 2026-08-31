#pragma once

#include <cstddef>
#include <string_view>

#include "rim_capability_id.h"
#include "RIMDataId.hpp"

namespace rim
{

class MachineCapabilityStore;
class RIMSnapshot;

using CapabilityBuildCallback =
    void (*)(
        const RIMSnapshot& snapshot,
        MachineCapabilityStore& store);

struct CapabilityItemDefinition
{
    RICapabilityId id;

    std::string_view name;

    CapabilityBuildCallback build;

    const RIMDataId* requiredDataItems;

    std::size_t requiredDataItemCount;
};

}