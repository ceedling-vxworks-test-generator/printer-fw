#pragma once

//
// RIMDataId - データ種別ID(連番・固定長配列前提)。rim_types.h の rim_data_id_t に対応。
// kCount は番兵(配列サイズに使用)。
//

namespace rim
{

enum class RIMDataId
{
    kTemperature = 0,
    kHumidity,
    kPressure,
    kJobProgress,
    kFaultCode,
    kCount
};

// 配列添字用ヘルパ。
inline constexpr int ToIndex(RIMDataId id) { return static_cast<int>(id); }
inline constexpr int kRIMDataIdCount = static_cast<int>(RIMDataId::kCount);

} // namespace rim
