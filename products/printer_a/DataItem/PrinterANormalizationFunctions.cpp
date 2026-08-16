#include "PrinterANormalizationFunctions.hpp"

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "SourceUnit.hpp"

namespace rim
{

RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context)
{
    return value;
}

RIMValue IdentityNormalizeBool(
    const RIMValue& value,
    const void* context)
{
    double raw{};

    if (!RIMValueAccessor::GetDouble(
            value,
            raw))
    {
        return {};
    }

    return
        RIMValueFactory::CreateBool(
            raw != 0.0);
}

RIMValue IdentityNormalizeInt32(
    const RIMValue& value,
    const void* context)
{
    double raw{};

    if (!RIMValueAccessor::GetDouble(
            value,
            raw))
    {
        return {};
    }

    return
        RIMValueFactory::CreateInt32(
            static_cast<std::int32_t>(
                raw));
}

RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const void* context)
{
    double raw{};

    if (!RIMValueAccessor::GetDouble(
            value,
            raw))
    {
        return {};
    }

    // 未指定(nullptr)は SourceUnit::kNormalized と同義(換算しない)。
    // 単位不明を推測して勝手に換算はしない。
    const SourceUnit unit =
        context != nullptr
            ? *static_cast<const SourceUnit*>(context)
            : SourceUnit::kNormalized;

    switch (unit)
    {
    case SourceUnit::kCelsius:
        return RIMValueFactory::CreateDouble(
            raw + 273.15);

    case SourceUnit::kFahrenheit:
        return RIMValueFactory::CreateDouble(
            (raw - 32.0) * 5.0 / 9.0 + 273.15);

    case SourceUnit::kNormalized:
    default:
        return RIMValueFactory::CreateDouble(
            raw);
    }
}

RIMValue NormalizeBinary(
    const RIMValue& value,
    const void*)
{
    return value;
}

} // namespace rim