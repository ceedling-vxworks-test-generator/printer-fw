#pragma once

#include "RouteDefinition.hpp"

namespace rim
{

inline constexpr RouteDefinition kValueRoute    {   "ValueRoute",  QueuePolicy::Buffered,   QueuePolicy::Buffered,  CompressionPolicy::KeepLatest,  100 };
inline constexpr RouteDefinition kDataRoute     {   "DataRoute",   QueuePolicy::Buffered,   QueuePolicy::Buffered,  CompressionPolicy::KeepOldest,  100 };
inline constexpr RouteDefinition kErrorRoute    {   "ErrorRoute",  QueuePolicy::Coalescing, QueuePolicy::Buffered,  CompressionPolicy::None,        0   };

}