#pragma once

#include "NotificationTarget.hpp"

namespace rim
{

struct ProductDefinition;
struct RouteDefinition;

class RouteResolver
{
public:

    static const RouteDefinition*
    Resolve(
        const ProductDefinition& product,
        const NotificationTarget& target);
};

} // namespace rim