#pragma once

//
// PrinterADataProfile - PrinterA の id 定義(機種可変)。
// IRuleResolver(SelectRule)と IInputClassifier(Classify)を実装し、Rule実体を所有する。
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

private:

    CelsiusRule   celsius_;
    PercentRule   percent_;
    ProgressRule  progress_;
    FaultCodeRule faultCode_;
    BoolRule      boolean_;
    CountRule     count_;
};

} // namespace rim
