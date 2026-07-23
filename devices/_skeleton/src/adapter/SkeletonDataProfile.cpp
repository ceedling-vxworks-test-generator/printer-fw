#include "adapter/SkeletonDataProfile.hpp"

namespace rim
{

const IRule* SkeletonDataProfile::SelectRule(RIMDataId id) const
{
    // TODO: id → 所有する Rule 実体へのポインタを返す。無ければ nullptr。
    (void)id;
    return nullptr;
}

std::optional<InputKind> SkeletonDataProfile::Classify(RIMDataId id) const
{
    // TODO: id → 性質(kFault / kOperationReport / kCurrentValue)。無ければ nullopt。
    (void)id;
    return std::nullopt;
}

} // namespace rim
