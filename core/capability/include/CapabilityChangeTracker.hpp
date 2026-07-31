#pragma once

#include <cstddef>

#include "CapabilityChangeList.hpp"
#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "CapabilityStore.hpp"
#include "ICapabilityDiffRule.hpp"

namespace rim
{

//
// CapabilityChangeTracker - 前回値と比較して変化した Capability を洗い出す。
//
// 旧 CapabilityChangeDetector は previousEnvironment_ / previousJob_ … と
// 製品の Capability 型ごとにメンバを持っていた。本クラスは id 添字の型消去スロットで
// 前回値を保持するため、Capability が増減しても Core は無改修。
//
// 差分判定は既定でバイト比較。Product が誤差許容比較等を要する場合は
// SetDiffRule() で ICapabilityDiffRule を差し込む(Capability 単位)。
//
class CapabilityChangeTracker
{
public:

    // id ごとの差分規則を差し込む(未設定なら既定のバイト比較)。
    bool SetDiffRule(CapabilityId id, const ICapabilityDiffRule* rule)
    {
        if (id >= kCapabilityMaxCount) return false;
        slots_[id].diffRule = rule;
        return true;
    }

    // store の現在値を前回値と突き合わせ、変化した id を返す。
    CapabilityChangeList Detect(const CapabilityStore& store)
    {
        CapabilityChangeList changed;

        for (std::size_t i = 0; i < kCapabilityMaxCount; ++i) {
            const CapabilityId id = static_cast<CapabilityId>(i);

            CapabilityPayload current;
            if (!store.TryGet(id, current)) continue;   // 未生成の Capability は対象外

            Slot& slot = slots_[i];

            const bool changedNow =
                !slot.hasPrevious
                    ? true                                   // 初回は変化として扱う
                    : (slot.diffRule
                           ? slot.diffRule->HasChanged(slot.previous, current)
                           : !slot.previous.Equals(current));

            if (changedNow) {
                changed.Add(id);
                slot.previous    = current;
                slot.hasPrevious = true;
            }
        }

        return changed;
    }

    void Reset()
    {
        for (auto& s : slots_) { s.hasPrevious = false; s.previous = CapabilityPayload{}; }
    }

private:

    struct Slot
    {
        CapabilityPayload          previous{};
        bool                       hasPrevious{false};
        const ICapabilityDiffRule* diffRule{nullptr};
    };

    Slot slots_[kCapabilityMaxCount]{};
};

} // namespace rim
