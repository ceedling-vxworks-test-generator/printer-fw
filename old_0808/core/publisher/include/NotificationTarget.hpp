#pragma once

#include <cstdint>

#include "NotificationTargetType.hpp"

namespace rim
{

struct NotificationTarget
{
    NotificationTargetType type;
    std::uint32_t id{};
};

}