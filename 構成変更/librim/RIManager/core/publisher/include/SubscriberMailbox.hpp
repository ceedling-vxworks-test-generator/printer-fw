#pragma once

#include <cstddef>

#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"

namespace rim
{

//
// SubscriberMailbox - ポーリング購読者向けに Capability の通知を溜める。
//
// 旧実装は std::queue<EnvironmentCapability> / <ErrorCapability> / … という
// 具象型ごとのキューを持っていたため、Capability を1つ増やすたびに Core の改修が
// 必要だった。本実装は CapabilityId で添字を引く固定長リングだけを持つので、
// 何が流れているかを Core は知らない。
//
// 制約と方針:
//   - 全静的確保。std::queue(動的確保)は使わない。
//   - Capability 1種あたり kCapabilityMailboxDepth 件まで。溢れた場合は
//     **最も古い通知を捨てて最新を残す**(状態通知としては最新が真であるため)。
//     捨てが起きたことは DroppedCount() で観測できる。
//   - 種別ごとに独立しているので、片方を読まなくても他方が詰まることはない。
//
class SubscriberMailbox
{
public:

    // 通知を積む。id が範囲外なら false。
    bool Push(CapabilityId id, const CapabilityPayload& payload)
    {
        if (id >= kCapabilityMaxCount) return false;

        Ring& ring = rings_[id];

        if (ring.count == kCapabilityMailboxDepth) {
            // 満杯: 最も古いものを捨てる
            ring.head = Next(ring.head);
            --ring.count;
            ++ring.dropped;
        }

        ring.slots[ring.tail] = payload;
        ring.tail             = Next(ring.tail);
        ++ring.count;

        return true;
    }

    // 指定 Capability の最も古い通知を取り出す。無ければ false。
    bool Pop(CapabilityId id, CapabilityPayload& payload)
    {
        if (id >= kCapabilityMaxCount) return false;

        Ring& ring = rings_[id];
        if (ring.count == 0) return false;

        payload   = ring.slots[ring.head];
        ring.head = Next(ring.head);
        --ring.count;

        return true;
    }

    std::size_t Count(CapabilityId id) const
    {
        if (id >= kCapabilityMaxCount) return 0;
        return rings_[id].count;
    }

    bool Empty(CapabilityId id) const { return Count(id) == 0; }

    // 溢れて捨てた通知の累計(消費が追いついていないことの検知に使う)。
    std::size_t DroppedCount(CapabilityId id) const
    {
        if (id >= kCapabilityMaxCount) return 0;
        return rings_[id].dropped;
    }

    void Clear()
    {
        for (std::size_t i = 0; i < kCapabilityMaxCount; ++i) {
            rings_[i].head  = 0;
            rings_[i].tail  = 0;
            rings_[i].count = 0;
        }
    }

private:

    static std::size_t Next(std::size_t index)
    {
        return (index + 1) % kCapabilityMailboxDepth;
    }

    struct Ring
    {
        CapabilityPayload slots[kCapabilityMailboxDepth]{};
        std::size_t       head{0};
        std::size_t       tail{0};
        std::size_t       count{0};
        std::size_t       dropped{0};
    };

    Ring rings_[kCapabilityMaxCount]{};
};

} // namespace rim
