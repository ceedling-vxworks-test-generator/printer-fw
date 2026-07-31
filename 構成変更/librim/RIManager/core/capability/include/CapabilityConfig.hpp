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

// SubscriberMailbox が Capability 1種あたり保持できる通知の数。
// 消費が追いつかず溢れた場合は **最も古い通知を捨てる**(最新の状態を残す)。
// 消費メモリは kCapabilityMaxCount * kCapabilityMailboxDepth * sizeof(CapabilityPayload)
// になるため、増やす際は RAM を確認すること。
inline constexpr std::size_t kCapabilityMailboxDepth = 4;

// コールバック購読の最大登録数(全 Capability 合計)。
inline constexpr std::size_t kCapabilitySubscriptionMaxCount = 64;

} // namespace rim
