#include "datastore/OperationRegistry.hpp"

namespace rim
{

std::optional<RegistryDomain> OperationRegistry::Apply(const RIMDataItem& item)
{
    if (item.id != RIMDataId::kJobProgress) return std::nullopt;

    const std::uint8_t p = item.value.u.jobProgress;

    std::lock_guard<std::mutex> lk(mutex_);
    if (present_ && jobProgress_ == p) return std::nullopt;  // 変化なし

    present_     = true;
    jobProgress_ = p;
    return kDomainOperation;
}

OperationSnapshot OperationRegistry::MakeSnapshot() const
{
    std::lock_guard<std::mutex> lk(mutex_);
    OperationSnapshot snap;
    snap.jobPresent  = present_;
    snap.jobProgress = jobProgress_;
    snap.jobActive   = present_ && jobProgress_ > 0 && jobProgress_ < 100;
    return snap;
}

void OperationRegistry::Clear()
{
    std::lock_guard<std::mutex> lk(mutex_);
    present_     = false;
    jobProgress_ = 0;
}

} // namespace rim
