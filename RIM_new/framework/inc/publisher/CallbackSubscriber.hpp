#pragma once

#include <functional>

#include "capability/EnvironmentCapability.hpp"

namespace rim
{

using EnvironmentCallback =
    std::function<void(
        const EnvironmentCapability&)>;

}