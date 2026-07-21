#pragma once

//
// StateRepository - 購読者ごとの「最後に配信した値」を保持し配信的差分を判定(仕様 §7.2)。
// 意味的差分(Capが変わったか)は保持しない(それは Capability層)。
//

#include <array>
#include <cstddef>

#include "capability/CapabilitySet.hpp"
#include "capability/CapabilityDiffChecker.hpp"
#include "publisher/ISubscriber.hpp"

namespace rim
{

class StateRepository
{
public:

    static constexpr std::size_t kMaxRecords = 16;

    // 記録なし(初回)or 最終配信値と差があれば true。
    bool HasChangedFor(ISubscriber* sub, const CapabilitySet& curr) const
    {
        const Record* r = Find(sub);
        if (!r) return true;
        return diff_.HasDifference(r->last, curr).changed;
    }

    void Update(ISubscriber* sub, const CapabilitySet& curr)
    {
        Record* r = Find(sub);
        if (r) { r->last = curr; return; }
        if (count_ < kMaxRecords) {
            records_[count_].sub     = sub;
            records_[count_].last    = curr;
            ++count_;
        }
    }

    void Forget(ISubscriber* sub)
    {
        for (std::size_t i = 0; i < count_; ++i) {
            if (records_[i].sub == sub) {
                --count_;
                records_[i] = records_[count_];
                return;
            }
        }
    }

    void Clear() { count_ = 0; }

private:

    struct Record
    {
        ISubscriber*  sub{nullptr};
        CapabilitySet last{};
    };

    Record* Find(ISubscriber* sub)
    {
        for (std::size_t i = 0; i < count_; ++i)
            if (records_[i].sub == sub) return &records_[i];
        return nullptr;
    }
    const Record* Find(ISubscriber* sub) const
    {
        for (std::size_t i = 0; i < count_; ++i)
            if (records_[i].sub == sub) return &records_[i];
        return nullptr;
    }

    CapabilityDiffChecker           diff_;
    std::array<Record, kMaxRecords> records_{};
    std::size_t                     count_{0};
};

} // namespace rim
