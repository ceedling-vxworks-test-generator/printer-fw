#pragma once

#include <functional>

#include "SubscriptionId.hpp"
#include "NotificationMessage.hpp"

namespace rim
{

    using NotificationCallback =
        std::function<
            void(
                SubscriptionId,
                const NotificationMessage&)>;

}