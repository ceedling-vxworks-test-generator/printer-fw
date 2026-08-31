#include "NormalizationFunctions.hpp"

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context)
{
    return value;
}

RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const void* context)
{
    double celsius{};

    if (!RIMValueAccessor::GetDouble(
            value,
            celsius))
    {
        return {};
    }

    return
        RIMValueFactory::CreateDouble(
            celsius + 273.15);
}

RIMValue NormalizeBinary(
    const RIMValue& value,
    const void*)
{
    return value;
}

} // namespace rim