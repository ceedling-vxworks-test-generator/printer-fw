#pragma once

#include "RouteDefinition.hpp"
#include "RouteId.hpp"

namespace rim
{

inline constexpr RouteDefinition kValueRoute    { ROUTE_VALUE, "ValueRoute",  QueuePolicy::Buffered,   QueuePolicy::Buffered,   QueuePolicy::PriorityCompression   };
inline constexpr RouteDefinition kDataRoute     { ROUTE_DATA,  "DataRoute",   QueuePolicy::Buffered,   QueuePolicy::Buffered,   QueuePolicy::PriorityCompression   };
inline constexpr RouteDefinition kErrorRoute    { ROUTE_ERROR, "ErrorRoute",  QueuePolicy::Coalescing, QueuePolicy::Buffered,   QueuePolicy::PriorityCompression   };

}