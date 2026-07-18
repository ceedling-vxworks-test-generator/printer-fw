#include "adapter/PrinterADataProfile.hpp"

namespace rim
{

std::optional<InputKind> PrinterADataProfile::Classify(RIMDataId id) const
{
    switch (id) {
        case RIMDataId::kFaultCode:   return InputKind::kFault;
        case RIMDataId::kJobProgress: return InputKind::kOperationReport;
        case RIMDataId::kTemperature:
        case RIMDataId::kHumidity:
        case RIMDataId::kPressure:    return InputKind::kCurrentValue;
        default:                      return std::nullopt;
    }
}

std::optional<RIMValue> PrinterADataProfile::Convert(
    RIMDataId id,
    double raw,
    const DataContext& ctx) const
{
    const std::int32_t scale = ctx.scaleX1000 ? *ctx.scaleX1000 : 1000;

    switch (id) {
        case RIMDataId::kTemperature: {
            const std::int32_t cx100 =
                static_cast<std::int32_t>((raw * scale / 1000.0) * 100.0);
            return RIMValue::CelsiusX100(cx100);
        }
        case RIMDataId::kHumidity:
        case RIMDataId::kPressure: {
            long p = static_cast<long>(raw);
            if (p < 0)   p = 0;
            if (p > 100) p = 100;
            return RIMValue::Percent(static_cast<std::uint8_t>(p));
        }
        case RIMDataId::kJobProgress: {
            long p = static_cast<long>(raw);
            if (p < 0)   p = 0;
            if (p > 100) p = 100;
            return RIMValue::JobProgress(static_cast<std::uint8_t>(p));
        }
        case RIMDataId::kFaultCode:
            return RIMValue::FaultCode(static_cast<std::uint32_t>(raw));
        default:
            return std::nullopt;
    }
}

} // namespace rim
