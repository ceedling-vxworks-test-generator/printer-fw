#pragma once

//
// RIMDataId - データ種別識別子(連番・固定長配列前提)。
// 8 Capability を支えるデータ集合。kCount は番兵。
//   Environment : Temperature / Humidity / Pressure
//   Consumable  : InkLevel / WiperLevel
//   Safety      : CoverOpen / EStop
//   Operation   : JobProgress
//   Fault       : FaultCode
//   Maintenance : MaintenanceCount
//   Health      : UnitAlive
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
