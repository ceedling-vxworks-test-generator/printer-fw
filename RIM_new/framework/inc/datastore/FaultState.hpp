#pragma once

//
// FaultState - 異常系Contextで用いる状態値(仕様 §7.1)。
// FaultRegistry.apply() が値ごとに挙動分岐する:
//   kRaised=登録 / kCleared=クリア / kAllCleared=全クリア /
//   kUpdatedHeal=回復状態へ更新 / kUpdatedActive=発生状態へ更新
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
