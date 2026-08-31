#pragma once

#include <cstdint>

#include "RIMId.hpp"

namespace rim
{

inline std::uint64_t
MakeNotificationTargetKey(
    const RIMId& target)
{
    return
        (static_cast<std::uint64_t>(
            target.type)
        << 32)
        | target.id;
}

}
