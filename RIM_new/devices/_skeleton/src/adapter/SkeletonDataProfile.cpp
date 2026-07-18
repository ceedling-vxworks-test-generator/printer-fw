#include "adapter/SkeletonDataProfile.hpp"

namespace rim
{

std::optional<InputKind> SkeletonDataProfile::Classify(RIMDataId id) const
{
    // TODO: この機種の id → 性質(InputKind)対応を定義する。
    //   FaultCode 系   → InputKind::kFault
    //   Report 系      → InputKind::kOperationReport
    //   最新値系       → InputKind::kCurrentValue
    (void)id;
    return std::nullopt;
}

std::optional<RIMValue> SkeletonDataProfile::Convert(
    RIMDataId id,
    double raw,
    const DataContext& ctx) const
{
    // TODO: 生値 raw を id 定義に沿った正規化 RIMValue へ変換する。
    //   ctx.scaleX1000 があればスケール適用。
    //   例: return RIMValue::CelsiusX100(static_cast<std::int32_t>(raw * 100.0));
    (void)id;
    (void)raw;
    (void)ctx;
    return std::nullopt;
}

} // namespace rim
