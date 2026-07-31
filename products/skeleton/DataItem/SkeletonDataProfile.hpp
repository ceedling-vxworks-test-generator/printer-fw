#pragma once

//
// SkeletonDataProfile - 新機種を起こすときの雛形。
//
// products/<機種>/DataItem/ へコピーし、その機種の id -> Rule 対応へ差し替える。
// テーブル駆動にしてあるので、規則が増えても分岐は伸びない。
//

#include "IRuleResolver.hpp"

namespace rim
{

class SkeletonDataProfile final
    : public IRuleResolver
{
public:

    const IRule* SelectRule(
        RIMDataId id) const override;
};

} // namespace rim
