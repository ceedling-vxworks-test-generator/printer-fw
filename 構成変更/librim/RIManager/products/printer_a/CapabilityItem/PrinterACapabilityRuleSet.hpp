#pragma once

#include <cstddef>

#include "CapabilityEvaluator.hpp"
#include "ErrorRepository.hpp"
#include "ICapabilityRuleProvider.hpp"
#include "PrinterACapabilityIds.hpp"
#include "PrinterACapabilityRules.hpp"

namespace rim
{

//
// PrinterACapabilityRuleSet - PrinterA の Capability 規則一式を所有し、
// Core の CapabilityEvaluator へ登録する。
//
// 「どの Capability があるか」を知っているのはこのクラス(= Product)だけである。
// Core はここから渡された ICapabilityRule* の配列を回すだけで、中身を知らない。
//
class PrinterACapabilityRuleSet final : public ICapabilityRuleProvider
{
public:

    explicit PrinterACapabilityRuleSet(const ErrorRepository& repository)
        : error_(repository)
        , printReady_(repository)
    {
    }

    // Core の評価器へ自分の規則を登録する。
    void RegisterTo(CapabilityEvaluator& evaluator) const override
    {
        evaluator.Register(&environment_);
        evaluator.Register(&error_);
        evaluator.Register(&printReady_);
        evaluator.Register(&consumable_);
        evaluator.Register(&job_);
    }

    std::size_t CapabilityCount() const override
    {
        return kPrinterACapabilityCount;
    }

private:

    EnvironmentCapabilityRule environment_;
    ErrorCapabilityRule       error_;
    PrintReadyCapabilityRule  printReady_;
    ConsumableCapabilityRule  consumable_;
    JobCapabilityRule         job_;
};

} // namespace rim
