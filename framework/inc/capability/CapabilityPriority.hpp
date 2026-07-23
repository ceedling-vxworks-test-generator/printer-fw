#pragma once

//
// CapabilityPriority - Publish優先度(仕様 §8.7.3)。
//

namespace rim
{

enum class CapabilityPriority
{
    kLow = 0,
    kNormal,
    kHigh,
    kCritical
};

} // namespace rim
