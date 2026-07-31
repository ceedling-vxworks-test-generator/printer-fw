#pragma once

#include <cstddef>

namespace rim
{

//
// Capability 層の容量パラメータ。すべて静的確保の上限を決めるもので、
// 実行時に伸びることはない(組込み向け: 定常運転中の動的確保をしない)。
//

// 1つの Capability の中身(型消去済みバイト列)の最大サイズ。
// Product の Capability 構造体のうち最大のものが収まる値にすること。
// 超える場合は CapabilityPayload::From() の static_assert がビルド時に検出する。
inline constexpr std::size_t kCapabilityPayloadMaxBytes = 256;

// 1つの Product が定義できる Capability の最大数(CapabilityId の上限)。
inline constexpr std::size_t kCapabilityMaxCount = 32;

} // namespace rim
