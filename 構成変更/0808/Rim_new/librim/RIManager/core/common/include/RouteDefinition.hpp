#pragma once

#include <string_view>

#include "QueueFactory.hpp"

namespace rim
{

struct RouteDefinition
{
    std::string_view name;
    QueuePolicy policy;
};

}