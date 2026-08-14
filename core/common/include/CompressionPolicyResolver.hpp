#pragma once

#include "NotificationTarget.hpp"
#include "CompressionPolicy.hpp"

namespace rim
{

class CompressionPolicyResolver
{
public:

    static CompressionPolicy
    Resolve(
        const NotificationTarget& target);
};

}