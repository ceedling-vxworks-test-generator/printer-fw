#pragma once

//
// SkeletonDataProfile - 新機種テンプレ。IRuleResolver + IInputClassifier。
// TODO: Skeleton を機種名へ置換し、Rule実体の所有・SelectRule・Classify を実装する。
//

#include "adapter/IRuleResolver.hpp"
#include "datastore/IInputClassifier.hpp"

namespace rim
{

class SkeletonDataProfile final
    : public IRuleResolver
    , public IInputClassifier
{
public:

    const IRule* SelectRule(RIMDataId id) const override;
    std::optional<InputKind> Classify(RIMDataId id) const override;

    // TODO: この機種の Rule 実体をメンバとして所有する。
};

} // namespace rim
