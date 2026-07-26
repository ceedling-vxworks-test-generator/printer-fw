#pragma once

//
// RIMDataId - PrinterA の データ種別識別子(連番・固定長配列前提)。機種固有(devices側)。
// 8 Capability を支えるデータ集合。kCount は番兵。
//   Environment : Temperature / Humidity / Pressure
//   Consumable  : InkLevel / WiperLevel
//   Safety      : CoverOpen / EStop
//   Operation   : JobProgress
//   Fault       : FaultCode
//   Maintenance : MaintenanceCount
//   Health      : UnitAlive
//
// framework は "datastore/RIMDataId.hpp" というヘッダ名(RIMDataId型・ToIndex()・
// kRIMDataIdCount)にのみ依存し、この列挙子の中身を一切知らない(#include "devices/..."
// は存在しない)。実体は機種ごとに devices/<機種>/inc/datastore/RIMDataId.hpp が提供し、
// ビルド時のインクルードパス解決で framework 側の #include "datastore/RIMDataId.hpp" に
// 届く(CMakeLists.txt の RIM_INCLUDE_DIRS 参照)。新機種を追加する際は
// devices/_skeleton/inc/datastore/RIMDataId.hpp を雛形にする。
//

namespace rim
{

enum class RIMDataId
{
    kTemperature = 0,
    kHumidity,
    kPressure,
    kInkLevel,
    kWiperLevel,
    kCoverOpen,
    kEStop,
    kJobProgress,
    kFaultCode,
    kMaintenanceCount,
    kUnitAlive,
    kCount
};

inline constexpr int ToIndex(RIMDataId id) { return static_cast<int>(id); }
inline constexpr int kRIMDataIdCount = static_cast<int>(RIMDataId::kCount);

} // namespace rim
