#pragma once

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "NotificationReceiver.hpp"

namespace rim
{

//
// RIManager - 利用者向けのファサード。変化通知を1件ずつ取り出す。
//
// 旧実装は TryGetEnvironment / TryGetError / TryGetPrintReady と Capability ごとに
// 関数を持っていた(= Core が製品の Capability 一覧を知っている状態)。
// 本実装は CapabilityId を引数で受ける1本に統一している。
//
class RIManager
{
public:

    explicit RIManager(
        NotificationReceiver& receiver)
        : receiver_(receiver)
    {
    }

    // 指定 Capability の通知を1件取り出す。無ければ false。
    bool TryGet(
        CapabilityId id,
        CapabilityPayload& payload)
    {
        return receiver_.TryGet(
            id,
            payload);
    }

    // 指定 Capability の通知を、指定の型として1件取り出す。
    // 通知が無い、またはサイズが T と一致しない場合は false。
    template <typename T>
    bool TryGetAs(
        CapabilityId id,
        T& out)
    {
        CapabilityPayload payload;

        if (!receiver_.TryGet(
                id,
                payload))
        {
            return false;
        }

        const T* typed =
            payload.As<T>();

        if (typed == nullptr)
        {
            return false;
        }

        out = *typed;

        return true;
    }

    bool HasPending(
        CapabilityId id) const
    {
        return receiver_.HasPending(
            id);
    }

private:

    NotificationReceiver& receiver_;
};

} // namespace rim
