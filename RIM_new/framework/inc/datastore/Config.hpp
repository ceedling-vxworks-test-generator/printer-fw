#pragma once

//
// 容量パラメータ。
//   kFaultQueueDepth / kOpQueueDepth : 各レーンのFIFO段数(dispatch未実行間の投入可能数)
//   kFaultCap                        : 同時activeなfaultの最大保持数
//

#include <cstddef>

namespace rim
{

constexpr std::size_t kFaultQueueDepth = 32;
constexpr std::size_t kOpQueueDepth    = 32;
constexpr std::size_t kFaultCap        = 64;

} // namespace rim
