#pragma once

//
// PrinterADataProfile - PrinterA の id 定義(機種可変)。
// IRuleResolver(SelectRule)と IInputClassifier(Classify)を実装する。
// id→(Rule, 性質) の対応はテーブル駆動(PrinterADataProfile.cpp の kTable)で管理する。
// Rule実体は状態を持たないため、id横断で共有する単一インスタンスとして保持する
// (このクラス自体はテーブルの参照窓口のみで、Rule実体を所有しない)。
// 結線では本オブジェクトを resolver 兼 classifier として注入する。
//

#include "adapter/IRuleResolver.hpp"
#include "adapter/rule/Rules.hpp"

#include "datastore/IInputClassifier.hpp"

namespace rim
{

class PrinterADataProfile final
    : public IRuleResolver
    , public IInputClassifier
{
public:

    const IRule* SelectRule(RIMDataId id) const override;

    std::optional<InputKind> Classify(RIMDataId id) const override;
};

} // namespace rim
