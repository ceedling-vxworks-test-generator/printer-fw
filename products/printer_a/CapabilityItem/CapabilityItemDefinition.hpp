#pragma once

#include <span>
#include <string_view>

namespace rim
{

struct CapabilityItemDefinition
{
    std::string_view name;

    const std::string_view* requiredDataItems;
    std::size_t requiredDataItemCount;
};

}