#pragma once

#include "rim_capability_id.h"

#include "EventPriority.hpp"
#include "CompressionPolicy.hpp"
#include "NotificationTarget.hpp"

namespace rim
{
    struct PublisherInput
    {
        
        NotificationTarget target;
        EventPriority priority;

    };
} // namespace rim