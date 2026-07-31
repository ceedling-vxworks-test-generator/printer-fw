#pragma once

#include <cstddef>

#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"

namespace rim
{

//
// CapabilityChangeList - 変化した Capability の集合。
//
// 旧 CapabilityChangeSet は std::vector<std::string> で Capability 名を持っていた
// (動的確保 + 文字列比較)。本クラスは CapabilityId の固定容量配列とし、
// 動的確保せず、比較も整数で行う。
//
class CapabilityChangeList
{
public:

    bool Add(CapabilityId id)
    {
        if (count_ >= kCapabilityMaxCount) return false;   // 満杯(取りこぼし)
        ids_[count_++] = id;
        return true;
    }

    bool Contains(CapabilityId id) const
    {
        for (std::size_t i = 0; i < count_; ++i) {
            if (ids_[i] == id) return true;
        }
        return false;
    }

    std::size_t  Size() const { return count_; }
    bool         Empty() const { return count_ == 0; }
    CapabilityId At(std::size_t i) const { return ids_[i]; }
    void         Clear() { count_ = 0; }

private:

    CapabilityId ids_[kCapabilityMaxCount]{};
    std::size_t  count_{0};
};

} // namespace rim
