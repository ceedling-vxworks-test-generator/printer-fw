#include "capability/CapabilityManager.hpp"

#include "capability/CapabilityKind.hpp"
#include "datastore/RegistryDomain.hpp"
#include "datastore/Snapshot.hpp"

namespace rim
{

void CapabilityManager::Init()
{
    prev_ = CapabilitySet{};
    current_ = CapabilitySet{};
    hasPrev_ = false;
    hasCurrent_ = false;
}

void CapabilityManager::Shutdown()
{
    hasPrev_ = false;
    hasCurrent_ = false;
}

void CapabilityManager::NotifyUpdated(RegistryDomainSet /*domains*/)
{
    // 取得範囲決定: PrintCap 等は横断依存のため全ドメインを取得する(設計判断)。
    SnapshotRequest req;
    req.domains = kDomainFault | kDomainOperation | kDomainCurrentAll;

    const MachineSnapshot snap = reader_.Capture(req);

    const CapabilitySet curr = builder_.Build(snap);

    const DiffResult diff = diffChecker_.HasDifference(prev_, curr);

    current_    = curr;
    hasCurrent_ = true;

    // 既に前回があり、かつ差分が無ければ配信しない。
    if (hasPrev_ && !diff.changed) {
        prev_ = curr;
        return;
    }

    MachineCapability mc;
    mc.capabilities = curr;
    mc.changedKinds = diff.changedKinds;
    mc.priority     = priorityChecker_.Check(curr);

    // ErrorCap / JobCap の変化は明示イベントとして通知する。
    const bool explicitEvent =
        (diff.changedKinds & KindBit(CapabilityKind::kError)) ||
        (diff.changedKinds & KindBit(CapabilityKind::kJob));
    mc.trigger = explicitEvent ? PublishTrigger::kEvent
                               : (hasPrev_ ? PublishTrigger::kOnChange : PublishTrigger::kInitial);

    publisher_.Notify(mc);

    prev_    = curr;
    hasPrev_ = true;
}

bool CapabilityManager::Current(CapabilitySet& out) const
{
    if (!hasCurrent_) return false;
    out = current_;
    return true;
}

} // namespace rim
