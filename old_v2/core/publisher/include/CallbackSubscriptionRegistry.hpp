#pragma once

#include <cstddef>

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
// コールバックは std::function ではなく関数ポインタ + userData で保持するため、
// 購読時も含めて動的確保は一切起きない。
//
class CallbackSubscriptionRegistry
{
public:

    // 指定 Capability の通知を購読する。満杯なら kInvalidSubscriptionId。
    // userData はコールバックへそのまま渡される(Core は中身を見ない)。
    SubscriptionId Subscribe(
        CapabilityId id,
        CapabilityCallbackFn callback,
        void* userData = nullptr)
    {
        if (id >= kCapabilityMaxCount)                 return kInvalidSubscriptionId;
        if (callback == nullptr)                       return kInvalidSubscriptionId;
        if (count_ >= kCapabilitySubscriptionMaxCount) return kInvalidSubscriptionId;

        const SubscriptionId subscriptionId = nextId_++;

        entries_[count_].subscriptionId = subscriptionId;
        entries_[count_].capabilityId   = id;
        entries_[count_].callback       = callback;
        entries_[count_].userData       = userData;
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
            entries_[i].callback(
                entries_[i].subscriptionId, id, payload, entries_[i].userData);
            ++notified;
        }

        return notified;
    }

    bool Unsubscribe(SubscriptionId id)
    {
        for (std::size_t i = 0; i < count_; ++i) {
            if (entries_[i].subscriptionId != id) continue;

            // 末尾を詰める(順序は保証しない)
            entries_[i] = entries_[count_ - 1];
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
        SubscriptionId       subscriptionId{kInvalidSubscriptionId};
        CapabilityId         capabilityId{kInvalidCapabilityId};
        CapabilityCallbackFn callback{nullptr};
        void*                userData{nullptr};
    };

    SubscriptionId nextId_{1};
    Entry          entries_[kCapabilitySubscriptionMaxCount]{};
    std::size_t    count_{0};
};

} // namespace rim
