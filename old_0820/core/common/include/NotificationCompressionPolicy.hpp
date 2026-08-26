#pragma once

#include <cstdint>

#include "CompressionPolicy.hpp"
#include "NotificationTarget.hpp"

namespace rim
{

struct NotificationCompressionPolicy
{

    NotificationTarget target;
    CompressionPolicy policy;
    
};

}