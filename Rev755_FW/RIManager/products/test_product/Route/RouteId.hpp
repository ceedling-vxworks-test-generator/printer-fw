#pragma once

#include <cstdint>

namespace rim
{

enum RouteId : std::uint32_t
{
    ROUTE_VALUE = 1,
    ROUTE_DATA  = 2,
    ROUTE_ERROR = 3,
    ROUTE_FACADE = 4,
};

}