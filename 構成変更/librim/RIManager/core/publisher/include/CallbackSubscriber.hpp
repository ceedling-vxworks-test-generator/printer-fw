#pragma once

#include <functional>

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "SubscriptionId.hpp"

namespace rim
{

//
// Capability 通知コールバック。
//
// 旧実装は EnvironmentCallback / ErrorCallback / … と Capability ごとに別名を
// 定義していたため、Capability を1つ増やすたびに Core の改修が必要だった。
// 本実装は「どの Capability か(CapabilityId)」と「中身(型消去バイト列)」だけを
// 渡す1本の型にまとめてある。
//
// 受け取り側は自分が購読した CapabilityId に対応する型で payload.As<T>() して
// 解釈する。Core はその型を知らない。
//
using CapabilityCallback =
    std::function<void(
        SubscriptionId,
        CapabilityId,
        const CapabilityPayload&)>;

} // namespace rim
