#pragma once

//
// CapabilityKind - 生成される8種のCapability(仕様 §4.2)。ビットマスクで変更集合を表す。
//

#include <cstdint>

namespace rim
{

enum class CapabilityKind
{
    kError = 0,
    kJob,
    kEnv,
    kMaint,
    kHealth,
    kSafety,
    kConsumable,
    kPrint,
    kCount
};

using CapabilityKindSet = std::uint32_t;

inline constexpr CapabilityKindSet KindBit(CapabilityKind k)
{
    return 1u << static_cast<std::uint32_t>(k);
}

} // namespace rim
