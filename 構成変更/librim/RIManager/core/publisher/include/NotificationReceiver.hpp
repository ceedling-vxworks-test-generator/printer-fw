#pragma once

#include <cstddef>

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "SubscriberMailbox.hpp"

namespace rim
{

//
// NotificationReceiver - ポーリング購読者が Mailbox から通知を取り出す窓口。
//
// 旧実装は TryGetEnvironment / TryGetError / TryGetPrintReady と Capability ごとに
// 関数を持っていた(= Core が製品の Capability 一覧を知っている状態)。
// 本実装は CapabilityId を引数で受ける1本の TryGet に統一している。
//
// 取り出した CapabilityPayload をどの型として解釈するかは購読者の責務
// (payload.As<T>())。Core はその型を知らない。
//
class NotificationReceiver
{
public:

    explicit NotificationReceiver(SubscriberMailbox& mailbox)
        : mailbox_(mailbox)
    {
    }

    // 指定 Capability の通知を1件取り出す。無ければ false。
    bool TryGet(CapabilityId id, CapabilityPayload& payload)
    {
        return mailbox_.Pop(id, payload);
    }

    // 取り出さずに覗くだけ(先頭の通知は残る)。
    bool Peek(CapabilityId id, CapabilityPayload& payload) const
    {
        return mailbox_.Peek(id, payload);
    }

    // 溜まっている通知の件数。
    std::size_t Pending(CapabilityId id) const { return mailbox_.Count(id); }

    bool HasPending(CapabilityId id) const { return mailbox_.Count(id) != 0; }

private:

    SubscriberMailbox& mailbox_;
};

} // namespace rim
