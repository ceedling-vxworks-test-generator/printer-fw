#pragma once

#include <cstddef>
#include <utility>

#include "CallbackSubscriber.hpp"
#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"

namespace rim
{

//
// CallbackSubscriptionRegistry - Capability 通知のコールバック購読を管理する。
//
// 旧実装は SubscribeEnvironment / SubscribeError / … と Capability ごとに
// 購読関数と std::vector を持っていた(= Core が製品の Capability 一覧を知って
// いる状態)。本実装は「購読したい CapabilityId」を引数で受けるだけなので、
// Capability の増減で Core は無改修になる。
//
// 全静的確保(固定長配列)。上限は kCapabilitySubscriptionMaxCount。
// ※ std::function は実装によっては動的確保しうる。組込み向けの完全な静的化は
//    別途 IF ポインタ方式への置換で対応する(非機能要件の項目)。
//
class CallbackSubscriptionRegistry
{
public:

    // 指定 Capability の通知を購読する。満杯なら kInvalidSubscriptionId。
    SubscriptionId Subscribe(CapabilityId id, CapabilityCallback callback)
    {
        if (id >= kCapabilityMaxCount)              return kInvalidSubscriptionId;
        if (!callback)                              return kInvalidSubscriptionId;
        if (count_ >= kCapabilitySubscriptionMaxCount) return kInvalidSubscriptionId;

        const SubscriptionId subscriptionId = nextId_++;

        entries_[count_].subscriptionId = subscriptionId;
        entries_[count_].capabilityId   = id;
        entries_[count_].callback       = std::move(callback);
        ++count_;

        return subscriptionId;
    }

    // 指定 Capability を購読している全コールバックを呼ぶ。
    // 戻り値は呼び出した件数。
    std::size_t Notify(CapabilityId id, const CapabilityPayload& payload) const
    {
        std::size_t notified = 0;

        for (std::size_t i = 0; i < count_; ++i) {
            if (entries_[i].capabilityId != id) continue;
            entries_[i].callback(entries_[i].subscriptionId, id, payload);
            ++notified;
        }

        return notified;
    }

    bool Unsubscribe(SubscriptionId id)
    {
        for (std::size_t i = 0; i < count_; ++i) {
            if (entries_[i].subscriptionId != id) continue;

            // 末尾を詰める(順序は保証しない)
            entries_[i] = std::move(entries_[count_ - 1]);
            entries_[count_ - 1] = Entry{};
            --count_;
            return true;
        }

        return false;
    }

    std::size_t Count() const { return count_; }

    // 指定 Capability の購読数。誰も購読していない Capability の配信を
    // 省くといった最適化の判断に使える。
    std::size_t CountFor(CapabilityId id) const
    {
        std::size_t n = 0;
        for (std::size_t i = 0; i < count_; ++i) {
            if (entries_[i].capabilityId == id) ++n;
        }
        return n;
    }

    void Clear()
    {
        for (std::size_t i = 0; i < count_; ++i) entries_[i] = Entry{};
        count_ = 0;
    }

private:

    struct Entry
    {
        SubscriptionId     subscriptionId{kInvalidSubscriptionId};
        CapabilityId       capabilityId{kInvalidCapabilityId};
        CapabilityCallback callback{};
    };

    SubscriptionId nextId_{1};
    Entry          entries_[kCapabilitySubscriptionMaxCount]{};
    std::size_t    count_{0};
};

} // namespace rim
