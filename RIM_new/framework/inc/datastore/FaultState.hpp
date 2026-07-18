#pragma once

//
// FaultState - 異常系Contextで用いる状態値。rim_fault_state_t 相当。
// CollectionOp へ写像される(後方互換):
//   kRaised->kAdd / kCleared->kRemove / kAllCleared->kClearAll
//   kUpdatedHeal->kUpdate(healed) / kUpdatedActive->kUpdate(active)
//

namespace rim
{

enum class FaultState
{
    kNone = 0,
    kRaised,
    kCleared,
    kAllCleared,
    kUpdatedHeal,
    kUpdatedActive
};

} // namespace rim
