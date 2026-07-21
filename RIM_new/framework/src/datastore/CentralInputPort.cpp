#include "datastore/CentralInputPort.hpp"

namespace rim
{

RIMResult CentralInputPort::PostFaultInput(const RIMDataItem& item)
{
    auto kind = classifier_.Classify(item.id);
    if (!kind || *kind != InputKind::kFault) return RIMResult::kErrKindMismatch;

    const FaultState state = item.context.faultState.value_or(FaultState::kRaised);
    // AllCleared 以外は、キー源(context.key or value=FaultCode)が必要。
    if (state != FaultState::kAllCleared &&
        !item.context.key && item.value.type != ValueType::kFaultCode) {
        return RIMResult::kErrKindMismatch;
    }

    if (!faultQueue_.Enqueue(item)) return RIMResult::kErrPost;  // 満杯=喪失
    return RIMResult::kOk;
}

RIMResult CentralInputPort::PostOperationReport(const RIMDataItem& item)
{
    auto kind = classifier_.Classify(item.id);
    if (!kind || *kind != InputKind::kOperationReport) return RIMResult::kErrKindMismatch;
    if (item.value.type != ValueType::kJobProgress) return RIMResult::kErrKindMismatch;

    if (!operationQueue_.Enqueue(item)) return RIMResult::kErrPost;
    return RIMResult::kOk;
}

RIMResult CentralInputPort::PostCurrentValueInput(const RIMDataItem& item)
{
    auto kind = classifier_.Classify(item.id);
    if (!kind || *kind != InputKind::kCurrentValue) return RIMResult::kErrKindMismatch;
    if (item.value.type == ValueType::kNone) return RIMResult::kErrKindMismatch;

    if (!currentBuffer_.Post(item)) return RIMResult::kErrInvalidArg;
    return RIMResult::kOk;
}

} // namespace rim
