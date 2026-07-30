#include "datastore/CentralInputPort.hpp"

namespace rim
{

RIMResult CentralInputPort::Post(const RIMDataItem& item)
{
    // 入力性質の判別とレーン振り分けは L2(本クラス)の責務。Adapter は分類しない。
    auto kind = classifier_.Classify(item.id);
    if (!kind) return RIMResult::kErrClassify;

    switch (*kind) {
        case InputKind::kFault:           return PostFaultInput(item);
        case InputKind::kOperationReport: return PostOperationReport(item);
        case InputKind::kCurrentValue:    return PostCurrentValueInput(item);
        default:                          return RIMResult::kErrClassify;
    }
}

RIMResult CentralInputPort::PostFaultInput(const RIMDataItem& item)
{
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
    if (item.value.type != ValueType::kJobProgress) return RIMResult::kErrKindMismatch;

    if (!operationQueue_.Enqueue(item)) return RIMResult::kErrPost;
    return RIMResult::kOk;
}

RIMResult CentralInputPort::PostCurrentValueInput(const RIMDataItem& item)
{
    if (item.value.type == ValueType::kNone) return RIMResult::kErrKindMismatch;

    if (!currentBuffer_.Post(item)) return RIMResult::kErrInvalidArg;
    return RIMResult::kOk;
}

} // namespace rim
