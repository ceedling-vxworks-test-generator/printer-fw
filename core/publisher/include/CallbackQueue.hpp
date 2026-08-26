#pragma once

#include "EventQueue.hpp"
#include "QueuePolicy.hpp"
#include "CallbackNotification.hpp"

namespace rim
{
    using CallbackQueue =
        EventQueue<
            CallbackNotification,
            FifoPolicy>;
}