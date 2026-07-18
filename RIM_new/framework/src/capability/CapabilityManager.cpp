#include "capability/CapabilityManager.hpp"

namespace rim
{

void CapabilityManager::Init()
{
    hasCurrent_ = false;
    current_ = CapabilitySet{};
}

void CapabilityManager::Shutdown()
{
    hasCurrent_ = false;
}

void CapabilityManager::OnUpdated(DomainSet /*domains*/)
{
    SnapshotRequest req;
    req.domains = kDomainFault | kDomainCurrent;

    MachineSnapshot snap;
    if (store_.Capture(req, snap) != RIMResult::kOk) return;

    CapabilitySet cap;
    evaluators_.Evaluate(snap, cap);

    current_    = cap;
    hasCurrent_ = true;

    publisher_.Publish(cap);   // L4 へ Push
}

bool CapabilityManager::Current(CapabilitySet& out) const
{
    if (!hasCurrent_) return false;
    out = current_;
    return true;
}

} // namespace rim
