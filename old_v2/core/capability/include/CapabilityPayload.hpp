#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "CapabilityConfig.hpp"

namespace rim
{

//
// CapabilityPayload - 型消去された Capability の中身。
//
// Core は「不透明なバイト列」としてしか扱わない。中身の型を知っているのは
// 生成した Product と、それを受け取る購読者だけである。
// これにより Core は EnvironmentCapability / JobCapability といった製品固有の型を
// 一切知らずに、保持・差分判定・配信ができる。
//
// 制約:
//   - 固定長バッファ(kCapabilityPayloadMaxBytes)。動的確保しない。
//   - 格納できるのは trivially copyable な型のみ(std::vector 等は入らない。
//     可変長が要るなら Product 側で固定容量コンテナに詰めてから渡すこと)。
//
class CapabilityPayload
{
public:

    // Product が自分の Capability 型から生成する。
    template <typename T>
    static CapabilityPayload From(const T& value)
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "CapabilityPayload に入れる型は trivially copyable であること"
                      "(可変長を持つ型は Product 側で固定容量コンテナへ詰めること)");
        static_assert(sizeof(T) <= kCapabilityPayloadMaxBytes,
                      "CapabilityPayload の容量不足。kCapabilityPayloadMaxBytes を見直すこと");

        CapabilityPayload p;
        // パディングまで含めて 0 で埋めてからコピーする。
        // これにより「論理的に同じ値がパディングの差で不一致になる」事故を防ぎ、
        // Core 既定のバイト比較(Equals)を信頼できるものにする。
        std::memset(p.bytes_, 0, sizeof p.bytes_);
        std::memcpy(p.bytes_, &value, sizeof(T));
        p.size_ = sizeof(T);
        return p;
    }

    // 受け取り側(Product / 購読者)が元の型として解釈する。
    //
    // 注意: 検知できるのは **サイズ違い** だけである。同サイズの別型を渡した場合は
    // 検知できない(型消去の代償)。取り違えを本当に防ぐには、CapabilityId と型の
    // 対応を Product 側で1箇所に集約し、そこ以外で As<T>() を直接呼ばないこと。
    template <typename T>
    const T* As() const
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "CapabilityPayload から取り出す型は trivially copyable であること");
        if (size_ != sizeof(T)) return nullptr;
        return reinterpret_cast<const T*>(bytes_);
    }

    // --- ここから下は Core が使う操作。中身の意味は解釈しない ---

    const std::uint8_t* Data() const { return bytes_; }
    std::size_t         Size() const { return size_; }
    bool                Empty() const { return size_ == 0; }

    // 既定の等価判定(バイト比較)。From() が 0 埋めしているためパディングは影響しない。
    // 誤差許容比較などが要る Capability は Product が ICapabilityDiffRule を実装する。
    bool Equals(const CapabilityPayload& rhs) const
    {
        if (size_ != rhs.size_) return false;
        return std::memcmp(bytes_, rhs.bytes_, size_) == 0;
    }

    bool operator==(const CapabilityPayload& rhs) const { return Equals(rhs); }
    bool operator!=(const CapabilityPayload& rhs) const { return !Equals(rhs); }

private:

    std::uint8_t bytes_[kCapabilityPayloadMaxBytes]{};
    std::size_t  size_{0};
};

} // namespace rim
