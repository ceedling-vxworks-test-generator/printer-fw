#pragma once

#include "AccessId.hpp"
#include "printer_a.h"

namespace rim::test
{

constexpr RIDataId kUnknownDataId =
    static_cast<RIDataId>(
        99999);

constexpr rim::AccessId kUnknownAccessId =
    static_cast<rim::AccessId>(
        99999u);

constexpr double kTemperatureA =
    25.0;

constexpr double kTemperatureB =
    26.0;

constexpr double kHumidity =
    50.0;

} // namespace rim::test