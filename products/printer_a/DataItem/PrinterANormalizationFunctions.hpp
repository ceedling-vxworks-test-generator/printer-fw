#pragma once

#include "RIMValue.hpp"

namespace rim
{

RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context);

RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const void* context);

RIMValue NormalizeBinary(
    const RIMValue& value,
    const void* context);


} // namespace rim