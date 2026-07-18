#pragma once

//
// DataStore 容量パラメータ。rim_datastore.h の -D 上書き可能マクロに対応。
//   kFaultQueueDepth : FAULTレーンのFIFO段数
//   kOpQueueDepth    : OPERATIONレーンのFIFO段数
//   kFaultCap        : Faultコレクション(同時activeなfault)の最大保持数
//

#include <cstddef>

namespace rim
{

constexpr std::size_t kFaultQueueDepth = 16;
constexpr std::size_t kOpQueueDepth    = 16;
constexpr std::size_t kFaultCap        = 32;

} // namespace rim
