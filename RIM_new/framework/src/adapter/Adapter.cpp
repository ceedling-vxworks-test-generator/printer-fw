#include "adapter/Adapter.hpp"

namespace rim
{

RIMResult Adapter::DispatchPost(InputKind kind, const RIMDataItem& item)
{
    switch (kind) {
        case InputKind::kFault:            return store_.PostFault(item);
        case InputKind::kOperationReport:  return store_.PostOperation(item);
        case InputKind::kCurrentValue:     return store_.PostCurrent(item);
        default:                           return RIMResult::kErrClassify;
    }
}

RIMResult Adapter::Intake(RIMDataId id, double raw, const DataContext* ctx)
{
    const int idx = ToIndex(id);
    if (idx < 0 || idx >= kRIMDataIdCount) return RIMResult::kErrInvalidArg;

    RIMDataItem item;
    item.id = id;
    if (ctx) item.context = *ctx;

    auto value = profile_.Convert(id, raw, item.context);
    if (!value) return RIMResult::kErrConvert;
    item.value = *value;

    auto kind = profile_.Classify(id);
    if (!kind) return RIMResult::kErrClassify;

    return DispatchPost(*kind, item);
}

RIMResult Adapter::Submit(RIMDataId id, CollectionOp op, std::uint32_t key,
                          const RIMValue* value, const DataContext* ctx)
{
    const int idx = ToIndex(id);
    if (idx < 0 || idx >= kRIMDataIdCount) return RIMResult::kErrInvalidArg;

    auto kind = profile_.Classify(id);
    if (!kind) return RIMResult::kErrClassify;
    // コレクション操作はキューレーン(FAULT/OPERATION)限定。CURRENTは非対応。
    if (*kind != InputKind::kFault && *kind != InputKind::kOperationReport)
        return RIMResult::kErrKindMismatch;

    RIMDataItem item;
    item.id = id;
    if (ctx) item.context = *ctx;

    // op/key は本引数で明示指定。
    item.context.op  = op;
    item.context.key = key;

    // value: REMOVE/CLEAR_ALL は不要 → NONE。ADD/UPDATE は与えられた値。
    if (op == CollectionOp::kRemove || op == CollectionOp::kClearAll || value == nullptr) {
        item.value = RIMValue{};
    } else {
        item.value = *value;
    }

    return DispatchPost(*kind, item);
}

} // namespace rim
