#pragma once

#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "SubscriberMailbox.hpp"

namespace rim
{

//
// ChangeNotifyManager - 変化した Capability を購読者へ届ける。
//
// 旧実装は Notify(const EnvironmentCapability&) … と Capability ごとの
// オーバーロードを持ち、しかも「Mailbox にも積む種別」と「コールバックだけの
// 種別」が実装内でバラバラに決め打ちされていた(Consumable / Job は Mailbox に
// 積まれない、という非対称が仕様ではなく実装都合で生まれていた)。
//
// 本実装は CapabilityId を引数で受ける1本の Notify に統一し、
// **全 Capability を同じ経路(Mailbox + コールバック)で扱う**。
// Capability の増減で Core は無改修になる。
//
class ChangeNotifyManager
{
public:

    ChangeNotifyManager(
        SubscriberMailbox&            mailbox,
        CallbackSubscriptionRegistry& callbackRegistry)
        : mailbox_(mailbox)
        , callbackRegistry_(callbackRegistry)
    {
    }

    // 変化した Capability を配信する。
    // ポーリング購読者向けに Mailbox へ積み、コールバック購読者を即時に呼ぶ。
    void Notify(CapabilityId id, const CapabilityPayload& payload)
    {
        mailbox_.Push(id, payload);
        callbackRegistry_.Notify(id, payload);
    }

private:

    SubscriberMailbox&            mailbox_;
    CallbackSubscriptionRegistry& callbackRegistry_;
};

} // namespace rim
