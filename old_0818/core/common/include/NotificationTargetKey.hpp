#pragma once

#include <cstdint>

#include "NotificationTarget.hpp"

namespace rim
{

inline std::uint64_t
MakeNotificationTargetKey(
    const NotificationTarget& target)
{
    return
        (static_cast<std::uint64_t>(
            target.type) << 32)
        | target.id;
}

}