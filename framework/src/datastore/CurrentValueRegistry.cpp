#include "datastore/CurrentValueRegistry.hpp"

namespace rim
{

RegistryDomain CurrentValueRegistry::DomainForId(RIMDataId id)
{
    switch (id) {
        case RIMDataId::kTemperature:
        case RIMDataId::kHumidity:
        case RIMDataId::kPressure:          return kDomainEnvironment;
        case RIMDataId::kInkLevel:
        case RIMDataId::kWiperLevel:        return kDomainConsumable;
        case RIMDataId::kCoverOpen:
        case RIMDataId::kEStop:             return kDomainSafety;
        case RIMDataId::kMaintenanceCount:  return kDomainMaintenance;
        case RIMDataId::kUnitAlive:         return kDomainHealth;
        default:                            return kDomainNone;
    }
}

bool CurrentValueRegistry::ValueEquals(const RIMValue& a, const RIMValue& b)
{
    if (a.type != b.type) return false;
    switch (a.type) {
        case ValueType::kCelsiusX100: return a.u.celsiusX100 == b.u.celsiusX100;
        case ValueType::kPercent:     return a.u.percent     == b.u.percent;
        case ValueType::kJobProgress: return a.u.jobProgress == b.u.jobProgress;
        case ValueType::kFaultCode:   return a.u.faultCode   == b.u.faultCode;
        case ValueType::kBool:        return a.u.boolean     == b.u.boolean;
        case ValueType::kUInt32:      return a.u.count       == b.u.count;
        case ValueType::kNone:        return true;
        default:                      return false;
    }
}

std::optional<RegistryDomain> CurrentValueRegistry::Apply(const RIMDataItem& item)
{
    const RegistryDomain domain = DomainForId(item.id);
    if (domain == kDomainNone) return std::nullopt;

    const int idx = ToIndex(item.id);
    if (idx < 0 || idx >= kRIMDataIdCount) return std::nullopt;

    std::lock_guard<std::mutex> lk(mutex_);
    Slot& s = slots_[idx];
    if (s.present && ValueEquals(s.value, item.value)) return std::nullopt;  // 変化なし

    s.value   = item.value;
    s.present = true;
    return domain;
}

CurrentValueSnapshot CurrentValueRegistry::MakeSnapshot(RegistryDomainSet domains) const
{
    std::lock_guard<std::mutex> lk(mutex_);
    CurrentValueSnapshot snap;

    auto slot = [&](RIMDataId id) -> const Slot& { return slots_[ToIndex(id)]; };

    if (domains & kDomainEnvironment) {
        EnvironmentSnapshot e;
        if (slot(RIMDataId::kTemperature).present) e.temperatureX100 = slot(RIMDataId::kTemperature).value.u.celsiusX100;
        if (slot(RIMDataId::kHumidity).present)    e.humidity        = slot(RIMDataId::kHumidity).value.u.percent;
        if (slot(RIMDataId::kPressure).present)    e.pressure        = slot(RIMDataId::kPressure).value.u.percent;
        snap.environment = e;
    }
    if (domains & kDomainConsumable) {
        ConsumableSnapshot c;
        if (slot(RIMDataId::kInkLevel).present)   c.inkLevel   = slot(RIMDataId::kInkLevel).value.u.percent;
        if (slot(RIMDataId::kWiperLevel).present) c.wiperLevel = slot(RIMDataId::kWiperLevel).value.u.percent;
        snap.consumable = c;
    }
    if (domains & kDomainSafety) {
        SafetySnapshot sf;
        if (slot(RIMDataId::kCoverOpen).present) sf.coverOpen = slot(RIMDataId::kCoverOpen).value.u.boolean;
        if (slot(RIMDataId::kEStop).present)     sf.eStop     = slot(RIMDataId::kEStop).value.u.boolean;
        snap.safety = sf;
    }
    if (domains & kDomainMaintenance) {
        MaintenanceSnapshot m;
        if (slot(RIMDataId::kMaintenanceCount).present) m.counter = slot(RIMDataId::kMaintenanceCount).value.u.count;
        snap.maintenance = m;
    }
    if (domains & kDomainHealth) {
        HealthSnapshot h;
        if (slot(RIMDataId::kUnitAlive).present) h.unitAlive = slot(RIMDataId::kUnitAlive).value.u.boolean;
        snap.health = h;
    }

    return snap;
}

void CurrentValueRegistry::Clear()
{
    std::lock_guard<std::mutex> lk(mutex_);
    for (auto& s : slots_) { s.present = false; s.value = RIMValue{}; }
}

} // namespace rim
