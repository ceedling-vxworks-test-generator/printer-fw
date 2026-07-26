#pragma once

//
// SkeletonDataProfile - 新機種テンプレ。IRuleResolver + IInputClassifier。
// TODO: Skeleton を機種名へ置換し、SelectRule・Classify を実装する。
// 実装は PrinterADataProfile 方式(推奨): .cpp の匿名namespaceに Rule の単一インスタンスと
// id→(Rule, 性質) のテーブルを置き、SelectRule/Classify はそのテーブルへの参照窓口にする
// (devices/printer_a/src/adapter/PrinterADataProfile.cpp を参照)。
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
};

} // namespace rim
