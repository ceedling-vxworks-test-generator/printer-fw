#pragma once

#include <cstddef>

#include "CapabilityConfig.hpp"
#include "CapabilityPayload.hpp"
#include "CapabilityStore.hpp"
#include "ICapabilityRule.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

//
// CapabilityEvaluator - 登録された規則を回して Capability を生成し、Store へ格納する。
//
// 旧 CapabilityManager は EnvironmentRule / ErrorRule / … を具象メンバとして持っていた
// ため、Capability を1つ増やすたびに Core の改修が必要だった。
// 本クラスは ICapabilityRule* の配列しか持たないので、何がいくつ登録されるかを知らない。
// 規則の生成と登録は Product の責務(products/printer_a/Pipeline/… で行う)。
//
// 全静的確保(固定長配列)。規則の所有権は持たない(呼出側が寿命を保証すること)。
//
class CapabilityEvaluator
{
public:

    // 規則を登録する。満杯なら false。
    bool Register(const ICapabilityRule* rule)
    {
        if (rule == nullptr) return false;
        if (count_ >= kCapabilityMaxCount) return false;
        rules_[count_++] = rule;
        return true;
    }

    // 登録済みの全規則を評価し、生成できたものを store へ格納する。
    // 戻り値は格納できた Capability の数。
    std::size_t Evaluate(const RIMSnapshot& snapshot, CapabilityStore& store) const
    {
        std::size_t stored = 0;

        for (std::size_t i = 0; i < count_; ++i) {
            CapabilityPayload payload;
            if (!rules_[i]->Evaluate(snapshot, payload)) continue;  // 生成不可はスキップ
            if (store.Store(rules_[i]->Id(), payload)) ++stored;
        }

        return stored;
    }

    std::size_t RuleCount() const { return count_; }

    // 登録済み規則の参照(範囲外は nullptr)。
    // 「登録された Capability の id 一覧」を組み立てる用途に使う。
    const ICapabilityRule* RuleAt(std::size_t index) const
    {
        if (index >= count_) return nullptr;
        return rules_[index];
    }

    void Clear() { count_ = 0; }

private:

    const ICapabilityRule* rules_[kCapabilityMaxCount]{};
    std::size_t            count_{0};
};

} // namespace rim
