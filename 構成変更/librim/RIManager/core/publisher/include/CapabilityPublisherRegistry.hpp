#pragma once

#include <cstddef>

#include "CapabilityConfig.hpp"
#include "CapabilityId.hpp"
#include "ICapabilityPublisher.hpp"

namespace rim
{

//
// CapabilityPublisherRegistry - CapabilityId ごとの配信器を保持する。
//
// 旧実装は std::unordered_map<std::string, std::unique_ptr<…>> で
// "Environment" / "Error" といった **文字列キー** を引いていた。
//   - 動的確保(map ノード + 文字列 + unique_ptr)が起きる
//   - キーの綴り間違いがビルド時に検出できない
//   - 引くたびにハッシュ計算と文字列比較が走る
// 本実装は CapabilityId を配列添字にする。動的確保なし、参照は O(1)。
//
// 所有権は持たない(配信器の寿命は呼出側が保証すること)。
//
class CapabilityPublisherRegistry
{
public:

    // 指定 Capability の配信器を登録する。id が範囲外なら false。
    // 既に登録済みなら上書きする。
    bool Register(CapabilityId id, ICapabilityPublisher* publisher)
    {
        if (id >= kCapabilityMaxCount) return false;
        publishers_[id] = publisher;
        return true;
    }

    ICapabilityPublisher* Find(CapabilityId id) const
    {
        if (id >= kCapabilityMaxCount) return nullptr;
        return publishers_[id];
    }

    void Unregister(CapabilityId id)
    {
        if (id >= kCapabilityMaxCount) return;
        publishers_[id] = nullptr;
    }

    std::size_t Count() const
    {
        std::size_t n = 0;
        for (std::size_t i = 0; i < kCapabilityMaxCount; ++i) {
            if (publishers_[i] != nullptr) ++n;
        }
        return n;
    }

private:

    ICapabilityPublisher* publishers_[kCapabilityMaxCount]{};
};

} // namespace rim
