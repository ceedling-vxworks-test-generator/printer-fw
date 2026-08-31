#pragma once

#include <string_view>
#include <cstdint>

#include "QueueFactory.hpp"
#include "CompressionPolicy.hpp"

namespace rim
{

struct RouteDefinition
{
    std::uint32_t id;
    std::string_view name;
    QueuePolicy storePolicy;
    QueuePolicy capabilityPolicy;
    QueuePolicy publishPolicy;
};

}