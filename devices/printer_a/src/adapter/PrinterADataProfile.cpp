#include "adapter/PrinterADataProfile.hpp"

namespace rim
{

const IRule* PrinterADataProfile::SelectRule(RIMDataId id) const
{
    switch (id) {
        case RIMDataId::kTemperature:      return &celsius_;
        case RIMDataId::kHumidity:
        case RIMDataId::kPressure:
        case RIMDataId::kInkLevel:
        case RIMDataId::kWiperLevel:       return &percent_;
        case RIMDataId::kJobProgress:      return &progress_;
        case RIMDataId::kFaultCode:        return &faultCode_;
        case RIMDataId::kCoverOpen:
        case RIMDataId::kEStop:
        case RIMDataId::kUnitAlive:        return &boolean_;
        case RIMDataId::kMaintenanceCount: return &count_;
        default:                           return nullptr;
    }
}

std::optional<InputKind> PrinterADataProfile::Classify(RIMDataId id) const
{
    switch (id) {
        case RIMDataId::kFaultCode:   return InputKind::kFault;
        case RIMDataId::kJobProgress: return InputKind::kOperationReport;
        case RIMDataId::kTemperature:
        case RIMDataId::kHumidity:
        case RIMDataId::kPressure:
        case RIMDataId::kInkLevel:
        case RIMDataId::kWiperLevel:
        case RIMDataId::kCoverOpen:
        case RIMDataId::kEStop:
        case RIMDataId::kMaintenanceCount:
        case RIMDataId::kUnitAlive:   return InputKind::kCurrentValue;
        default:                      return std::nullopt;
    }
}

} // namespace rim
