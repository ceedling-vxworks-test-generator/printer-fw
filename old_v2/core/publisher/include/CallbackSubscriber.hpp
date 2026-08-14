#pragma once

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "SubscriptionId.hpp"

namespace rim
{

//
// Capability 通知コールバック。
//
// 変遷:
//   1. 当初は EnvironmentCallback / ErrorCallback / … と Capability ごとに
//      別名を定義していた(増やすたびに Core の改修が必要)
//   2. std::function<void(SubscriptionId, CapabilityId, const CapabilityPayload&)>
//      の1本に統合
//   3. 現在: **関数ポインタ + userData** に変更
//
// 3 にした理由:
//   - std::function は捕捉が小さいうちは内部バッファで済むが、大きくなると
//     **暗黙に動的確保**する。組込みで「いつ確保が起きるか」が読めないのは困る。
//   - 空の std::function を呼ぶと std::bad_function_call を投げるが、
//     -fno-exceptions では abort になる。呼び出し経路に例外前提の型を置きたくない。
//   - C API 側が既に「関数ポインタ + userData」なので、余計な変換も無くなる。
//
// 受け取り側は自分が購読した CapabilityId に対応する型で payload.As<T>() して
// 解釈する。Core はその型を知らない。
//
using CapabilityCallbackFn =
    void (*)(
        SubscriptionId subscriptionId,
        CapabilityId capabilityId,
        const CapabilityPayload& payload,
        void* userData);

} // namespace rim
