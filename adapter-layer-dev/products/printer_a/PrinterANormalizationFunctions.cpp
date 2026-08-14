#include "PrinterANormalizationFunctions.hpp"

#include "PrinterAContextKeys.hpp"
#include "RIMContextAccessor.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const RIMContext* context)
{
    const double raw =
        value.value.d;

    int unit =
        kUnitNormalized;

    TryGetContextValue(
        context,
        kContextKeyUnit,
        unit);

    double kelvin{};

    switch (unit)
    {
    case kUnitCelsius:
        kelvin = raw + 273.15;
        break;

    case kUnitFahrenheit:
        kelvin = (raw - 32.0) * 5.0 / 9.0 + 273.15;
        break;

    case kUnitNormalized:
    default:
        kelvin = raw;
        break;
    }

    return RIMValueFactory::CreateDouble(
        kelvin);
}

RIMValue NormalizeStapleLevel(
    const RIMValue& value,
    const RIMContext*)
{
    double v =
        value.value.d;

    if (v < 0.0)   v = 0.0;
    if (v > 100.0) v = 100.0;

    return RIMValueFactory::CreateInt32(
        static_cast<std::int32_t>(v));
}

} // namespace rim
