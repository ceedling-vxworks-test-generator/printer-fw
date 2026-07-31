#pragma once

#include <cstdint>

namespace rim
{

using SubscriptionId =
    std::uint64_t;

// 購読 ID は 1 から採番するため、0 を「無効/採番失敗」に使う。
// (購読枠が満杯だった場合などに返る)
inline constexpr SubscriptionId kInvalidSubscriptionId = 0;

}
