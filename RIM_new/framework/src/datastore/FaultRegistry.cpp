#include "datastore/FaultRegistry.hpp"

namespace rim
{

std::uint32_t FaultRegistry::ResolveKey(const RIMDataItem& item) const
{
    return item.context.key ? *item.context.key : item.value.u.faultCode;
}

std::optional<RegistryDomain> FaultRegistry::Apply(const RIMDataItem& item)
{
    const FaultState state = item.context.faultState.value_or(FaultState::kRaised);

    std::lock_guard<std::mutex> lk(mutex_);

    bool changed = false;

    switch (state) {
        case FaultState::kRaised: {
            changed = map_.Add(ResolveKey(item), Entry{true});
            break;
        }
        case FaultState::kCleared: {
            changed = map_.Remove(ResolveKey(item));
            break;
        }
        case FaultState::kAllCleared: {
            changed = (map_.Size() > 0);
            map_.Clear();
            break;
        }
        case FaultState::kUpdatedHeal: {
            Entry* e = map_.Find(ResolveKey(item));
            if (e && e->active) { e->active = false; changed = true; }
            break;
        }
        case FaultState::kUpdatedActive: {
            Entry* e = map_.Find(ResolveKey(item));
            if (e && !e->active) { e->active = true; changed = true; }
            break;
        }
        case FaultState::kNone:
        default:
            break;
    }

    if (changed) return kDomainFault;
    return std::nullopt;
}

FaultSnapshot FaultRegistry::MakeSnapshot() const
{
    std::lock_guard<std::mutex> lk(mutex_);

    FaultSnapshot snap;
    for (std::size_t i = 0; i < map_.Size(); ++i) {
        if (map_.ValueAt(i).active) {
            if (snap.activeCount < kFaultCap)
                snap.activeCodes[snap.activeCount] = map_.KeyAt(i);
            ++snap.activeCount;
        } else {
            ++snap.healedCount;
        }
    }
    return snap;
}

void FaultRegistry::Clear()
{
    std::lock_guard<std::mutex> lk(mutex_);
    map_.Clear();
}

} // namespace rim
