#pragma once

#include <cstddef>
#include <mutex>

#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"

namespace rim
{

//
// CapabilityStore - 生成済み Capability の保持。
//
// 旧 MachineCapabilityStore は EnvironmentCapability / JobCapability … を
// メンバとして直接持っていたため、Core が製品の Capability 語彙に固定されていた。
// 本クラスは CapabilityId を添字とする型消去スロットのみを持ち、中身を解釈しない。
// Capability を1つ増やしても Core は無改修。
//
// 全静的確保(kCapabilityMaxCount 個の固定配列)。動的確保しない。
//
class CapabilityStore
{
public:

    // id の Capability を格納する。範囲外の id は無視して false。
    bool Store(CapabilityId id, const CapabilityPayload& payload)
    {
        if (id >= kCapabilityMaxCount) return false;

        std::lock_guard<std::mutex> lock(mutex_);
        slots_[id].payload = payload;
        slots_[id].present = true;
        return true;
    }

    // id の Capability を取り出す。未格納/範囲外なら false。
    bool TryGet(CapabilityId id, CapabilityPayload& out) const
    {
        if (id >= kCapabilityMaxCount) return false;

        std::lock_guard<std::mutex> lock(mutex_);
        if (!slots_[id].present) return false;
        out = slots_[id].payload;
        return true;
    }

    bool Has(CapabilityId id) const
    {
        if (id >= kCapabilityMaxCount) return false;
        std::lock_guard<std::mutex> lock(mutex_);
        return slots_[id].present;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& s : slots_) { s.present = false; s.payload = CapabilityPayload{}; }
    }

    static constexpr std::size_t Capacity() { return kCapabilityMaxCount; }

private:

    struct Slot
    {
        CapabilityPayload payload{};
        bool              present{false};
    };

    mutable std::mutex mutex_;
    Slot               slots_[kCapabilityMaxCount]{};
};

} // namespace rim
