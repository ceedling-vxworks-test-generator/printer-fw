#pragma once

#include "RouteDefinition.hpp"

namespace rim
{

inline constexpr RouteDefinition
kValue
{
    "Value",
    QueuePolicy::Buffered
    
};

inline constexpr RouteDefinition
kData
{
    "Data",
    QueuePolicy::Buffered
};

inline constexpr RouteDefinition
kError
{
    "Error",
    QueuePolicy::Coalescing
};

}