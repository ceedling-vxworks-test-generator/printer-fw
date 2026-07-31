#pragma once

#include <span>
#include <string_view>

namespace rim
{

struct PipelineDefinition
{
    std::string_view name;

    const std::string_view* triggerDomains;
    std::size_t triggerDomainCount;

    std::string_view outputCapability;
};

}