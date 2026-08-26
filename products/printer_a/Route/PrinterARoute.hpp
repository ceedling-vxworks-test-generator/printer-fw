#pragma once

#include "RouteDefinition.hpp"
#include "PrinterARouteId.hpp"

namespace rim
{

inline constexpr RouteDefinition kValueRoute    { ROUTE_VALUE, "ValueRoute",  QueuePolicy::Buffered,   QueuePolicy::Buffered,  CompressionPolicy::KeepLatest,  100 };
inline constexpr RouteDefinition kDataRoute     { ROUTE_DATA,  "DataRoute",   QueuePolicy::Buffered,   QueuePolicy::Buffered,  CompressionPolicy::KeepOldest,  100 };
inline constexpr RouteDefinition kErrorRoute    { ROUTE_ERROR, "ErrorRoute",  QueuePolicy::Coalescing, QueuePolicy::Buffered,  CompressionPolicy::None,        0   };
inline constexpr RouteDefinition kFacadeRoute   { ROUTE_FACADE,"FacadeRoute", QueuePolicy::Coalescing, QueuePolicy::Buffered,  CompressionPolicy::KeepOldest,  100 };

}