#pragma once

//
// SubscriptionBroker - 購読者の登録/解除と関心Cap一致に基づく実配信対象の管理(仕様 §7.3)。
// 購読者は登録順に保持する(配信順の安定性)。
//

#include <array>
#include <cstddef>

#include "datastore/RIMResult.hpp"
#include "publisher/ISubscriber.hpp"
#include "publisher/SubscriptionSet.hpp"

namespace rim
{

class SubscriptionBroker
{
public:

    static constexpr std::size_t kMaxSubscribers = 16;

    RIMResult Subscribe(ISubscriber* sub, const SubscriptionSet& set)
    {
        if (sub == nullptr) return RIMResult::kErrInvalidArg;
        if (count_ >= kMaxSubscribers) return RIMResult::kErrPost;
        entries_[count_].sub = sub;
        entries_[count_].set = set;
        ++count_;
        return RIMResult::kOk;
    }

    void Unsubscribe(ISubscriber* sub)
    {
        for (std::size_t i = 0; i < count_; ++i) {
            if (entries_[i].sub == sub) {
                for (std::size_t j = i + 1; j < count_; ++j) entries_[j - 1] = entries_[j];  // 登録順維持
                --count_;
                return;
            }
        }
    }

    std::size_t            Count() const { return count_; }
    ISubscriber*           SubscriberAt(std::size_t i) const { return entries_[i].sub; }
    const SubscriptionSet& SubscriptionAt(std::size_t i) const { return entries_[i].set; }

    void Clear() { count_ = 0; }

private:

    struct Entry
    {
        ISubscriber*    sub{nullptr};
        SubscriptionSet set{};
    };

    std::array<Entry, kMaxSubscribers> entries_{};
    std::size_t                        count_{0};
};

} // namespace rim
