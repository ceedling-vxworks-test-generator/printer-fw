#include "datastore/CurrentValueRegistry.hpp"

#include <cstddef>

namespace rim
{
namespace
{

struct DomainEntry
{
    RIMDataId      id;     // 自己文書化・テスト用(検索には使わない。添字は id そのもの)
    RegistryDomain domain; // CurrentValueRegistry が扱わない id(Fault/Operation)は kDomainNone
};

// id → 副ドメイン の対応表。RIMDataId の宣言順に並べ、id をそのまま添字にする
// (O(1)、稠密ID規約。PrinterADataProfile の kTable と同じ流儀)。
const DomainEntry kDomainTable[] = {
    { RIMDataId::kTemperature,      kDomainEnvironment },
    { RIMDataId::kHumidity,         kDomainEnvironment },
    { RIMDataId::kPressure,         kDomainEnvironment },
    { RIMDataId::kInkLevel,         kDomainConsumable  },
    { RIMDataId::kWiperLevel,       kDomainConsumable  },
    { RIMDataId::kCoverOpen,        kDomainSafety      },
    { RIMDataId::kEStop,            kDomainSafety      },
    { RIMDataId::kJobProgress,      kDomainNone        }, // OperationRegistry が扱う
    { RIMDataId::kFaultCode,        kDomainNone        }, // FaultRegistry が扱う
    { RIMDataId::kMaintenanceCount, kDomainMaintenance },
    { RIMDataId::kUnitAlive,        kDomainHealth      },
};

static_assert(sizeof(kDomainTable) / sizeof(kDomainTable[0]) == static_cast<std::size_t>(kRIMDataIdCount),
              "kDomainTable must have exactly one row per RIMDataId (add/remove a row when RIMDataId changes)");

} // namespace

RegistryDomain CurrentValueRegistry::DomainForId(RIMDataId id)
{
    const int idx = ToIndex(id);
    if (idx < 0 || idx >= kRIMDataIdCount) return kDomainNone;
    return kDomainTable[idx].domain;
}

bool CurrentValueRegistry::ValueEquals(const RIMValue& a, const RIMValue& b)
{
    if (a.type != b.type) return false;
    switch (a.type) {
        case ValueType::kKelvinX100:  return a.u.kelvinX100  == b.u.kelvinX100;
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
        if (slot(RIMDataId::kTemperature).present) e.temperatureKelvinX100 = slot(RIMDataId::kTemperature).value.u.kelvinX100;
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
