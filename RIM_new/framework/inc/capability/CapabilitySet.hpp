#pragma once

//
// CapabilitySet - 生成されるCapability(意味づけ結果)。rim_capability_set_t 相当。
// 公開契約型として framework 側に置く。has_xxx は std::optional で表現。
// スケルトンでは代表2種(printable / temp_alert)を持つ。
//

#include <optional>

namespace rim
{

struct CapabilitySet
{
    std::optional<bool> printable;   // 印刷可否
    std::optional<bool> tempAlert;   // 温度警報
};

} // namespace rim
