#pragma once

#include <cstdint>

#include "NotificationTarget.hpp"

namespace rim
{

inline std::uint64_t
MakeNotificationTargetKey(
    const NotificationTarget& target)
{
    return
        (static_cast<std::uint64_t>(
            target.type) << 32)
        | target.id;

        // TODO: ログ追加候補
        // Target関連ログでは type と id を
        // 必ずセットで出力する。
        // idのみではCapability/Dataの区別ができない。
        
}

}