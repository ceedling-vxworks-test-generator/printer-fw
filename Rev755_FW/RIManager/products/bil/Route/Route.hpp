#pragma once

#include "RouteDefinition.hpp"
#include "RouteId.hpp"

namespace rim
{

inline constexpr RouteDefinition
kDataRoute
{
    ROUTE_DATA,
    "DataRoute",
    QueuePolicy::Buffered,
    QueuePolicy::Buffered,
    QueuePolicy::PriorityCompression
};

}
