#include "datastore/DataStore.hpp"

namespace rim
{

namespace
{

// FaultState → CollectionOp 写像(後方互換)。
CollectionOp MapFaultState(FaultState s)
{
    switch (s) {
        case FaultState::kRaised:        return CollectionOp::kAdd;
        case FaultState::kCleared:       return CollectionOp::kRemove;
        case FaultState::kAllCleared:    return CollectionOp::kClearAll;
        case FaultState::kUpdatedHeal:   return CollectionOp::kUpdate;
        case FaultState::kUpdatedActive: return CollectionOp::kUpdate;
        case FaultState::kNone:          return CollectionOp::kAdd;
        default:                         return CollectionOp::kAdd;
    }
}

} // namespace

RIMResult DataStore::Init(IUpdateNotifier* notifier)
{
    std::lock_guard<std::mutex> lk(mutex_);
    for (int i = 0; i < kRIMDataIdCount; ++i) {
        current_[i].present = false;
        current_[i].dirty   = false;
    }
    faultRing_.Clear();
    faultMap_.Clear();
    opRing_.Clear();
    opPresent_     = false;
    opJobProgress_ = 0;
    notifier_      = notifier;
    return RIMResult::kOk;
}

void DataStore::Shutdown()
{
    std::lock_guard<std::mutex> lk(mutex_);
    notifier_ = nullptr;
}

void DataStore::Fire(DomainSet domains)
{
    if (domains != kDomainNone && notifier_ != nullptr)
        notifier_->OnUpdated(domains);
}

RIMResult DataStore::PostFault(const RIMDataItem& item)
{
    auto kind = profile_.Classify(item.id);
    if (!kind || *kind != InputKind::kFault) return RIMResult::kErrKindMismatch;

    // op: ctx.op 優先。無ければ faultState から写像(未指定は kRaised=kAdd 既定)。
    CollectionOp op = item.context.op
        ? *item.context.op
        : MapFaultState(item.context.faultState ? *item.context.faultState : FaultState::kRaised);

    // 値型検証: CLEAR_ALL は値・キー不要。それ以外で key 未指定のときのみ、
    // キー源として value が FaultCode であることを要求(従来pushパス互換)。
    if (op != CollectionOp::kClearAll && !item.context.key &&
        item.value.type != ValueType::kFaultCode) {
        return RIMResult::kErrKindMismatch;
    }

    std::uint32_t key = item.context.key ? *item.context.key : item.value.u.faultCode;

    FaultEntry payload;
    payload.active = (item.context.faultState &&
                      *item.context.faultState == FaultState::kUpdatedHeal) ? 0u : 1u;

    FaultEvent ev;
    ev.op = op; ev.key = key; ev.payload = payload;

    std::lock_guard<std::mutex> lk(mutex_);
    if (!faultRing_.Push(ev)) return RIMResult::kErrPost;
    return RIMResult::kOk;
}

RIMResult DataStore::PostOperation(const RIMDataItem& item)
{
    auto kind = profile_.Classify(item.id);
    if (!kind || *kind != InputKind::kOperationReport) return RIMResult::kErrKindMismatch;
    if (item.value.type != ValueType::kJobProgress) return RIMResult::kErrKindMismatch;

    OpEvent ev;
    ev.jobProgress = item.value.u.jobProgress;

    std::lock_guard<std::mutex> lk(mutex_);
    if (!opRing_.Push(ev)) return RIMResult::kErrPost;
    return RIMResult::kOk;
}

RIMResult DataStore::PostCurrent(const RIMDataItem& item)
{
    auto kind = profile_.Classify(item.id);
    if (!kind || *kind != InputKind::kCurrentValue) return RIMResult::kErrKindMismatch;

    const int idx = ToIndex(item.id);
    if (idx < 0 || idx >= kRIMDataIdCount) return RIMResult::kErrInvalidArg;

    std::lock_guard<std::mutex> lk(mutex_);
    CurrentSlot& s = current_[idx];   // 同一IDは最新で上書き
    s.value   = item.value;
    s.present = true;
    s.dirty   = true;
    return RIMResult::kOk;
}

void DataStore::Dispatch()
{
    DomainSet changed = kDomainNone;

    {
        std::lock_guard<std::mutex> lk(mutex_);

        // FAULT: FIFO順に add/remove/update/clear_all を管理配列へ反映。
        FaultEvent fev;
        while (faultRing_.Pop(fev)) {
            bool ch = false;
            switch (fev.op) {
                case CollectionOp::kAdd:      ch = faultMap_.Add(fev.key, fev.payload);    break;
                case CollectionOp::kRemove:   ch = faultMap_.Remove(fev.key);              break;
                case CollectionOp::kUpdate:   ch = faultMap_.Update(fev.key, fev.payload); break;
                case CollectionOp::kClearAll: ch = (faultMap_.Size() > 0); faultMap_.Clear(); break;
                default: break;
            }
            if (ch) changed |= kDomainFault;
        }

        // OPERATION: FIFO順に最新JobProgressへ反映。
        OpEvent oev;
        while (opRing_.Pop(oev)) {
            opJobProgress_ = oev.jobProgress;
            opPresent_     = true;
            changed |= kDomainOperation;
        }

        // CURRENT: dirty を取り込み(latest-wins)。
        for (int i = 0; i < kRIMDataIdCount; ++i) {
            if (current_[i].dirty) {
                current_[i].dirty = false;
                changed |= kDomainCurrent;
            }
        }
    }   // ★ 通知発火の前に必ず解放(Capture再入によるデッドロック回避)

    Fire(changed);
}

RIMResult DataStore::Capture(const SnapshotRequest& req, MachineSnapshot& out) const
{
    out.fault.reset();
    out.current.reset();

    std::lock_guard<std::mutex> lk(mutex_);

    if (req.domains & kDomainFault) {
        FaultSnapshot fs;
        const std::uint32_t n = static_cast<std::uint32_t>(faultMap_.Size());
        fs.activeCount = n;
        for (std::uint32_t i = 0; i < n; ++i)
            fs.activeCodes[i] = faultMap_.KeyAt(i);
        out.fault = fs;
    }
    if (req.domains & kDomainCurrent) {
        CurrentValueSnapshot cs;
        for (int i = 0; i < kRIMDataIdCount; ++i) {
            cs.present[i] = current_[i].present;
            cs.values[i]  = current_[i].value;
        }
        out.current = cs;
    }

    return RIMResult::kOk;
}

} // namespace rim
