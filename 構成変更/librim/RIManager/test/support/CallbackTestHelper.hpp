#pragma once

#include <atomic>

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "CapabilityStore.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriptionId.hpp"

namespace rim
{

//
// 購読コールバックの試験用ヘルパ。
//
// Core のコールバックは **関数ポインタ + userData** である(std::function は
// 暗黙の動的確保と例外送出があるため組込みでは使わない)。
// そのため捕捉付きラムダを直接渡せない。状態はこの構造体に置き、
// そのアドレスを userData として渡す。
//

struct CapabilityRecorder
{
    // ワーカスレッドから書かれる可能性があるので、判定に使う値は atomic にする。
    std::atomic<int>           calls{0};
    std::atomic<SubscriptionId> lastSubscription{kInvalidSubscriptionId};
    std::atomic<CapabilityId>   lastCapability{kInvalidCapabilityId};

    // 中身は最後の1件だけ保持する(複数スレッドから同時に書かれる試験では使わない)。
    CapabilityPayload lastPayload{};

    bool Called() const { return calls.load() != 0; }

    void Reset()
    {
        calls            = 0;
        lastSubscription = kInvalidSubscriptionId;
        lastCapability   = kInvalidCapabilityId;
        lastPayload      = CapabilityPayload{};
    }

    template <typename T>
    const T* As() const { return lastPayload.As<T>(); }

    // CallbackSubscriptionRegistry::Subscribe に渡す関数。
    // userData には CapabilityRecorder* を渡すこと。
    static void Callback(
        SubscriptionId subscriptionId,
        CapabilityId capabilityId,
        const CapabilityPayload& payload,
        void* userData)
    {
        auto* self =
            static_cast<CapabilityRecorder*>(
                userData);

        if (self == nullptr)
        {
            return;
        }

        self->lastSubscription = subscriptionId;
        self->lastCapability   = capabilityId;
        self->lastPayload      = payload;
        self->calls.fetch_add(1);
    }
};

//
// 「Store の現在値を通知経路へ流す」配信器の中身。
// 実運用(rim_api.cpp の StorePublisher)と同じ振る舞いを試験でも使うためのもの。
//
// GenericCapabilityPublisher に Publish 関数と userData として渡す。
//
struct StorePublishBinding
{
    const CapabilityStore* store{nullptr};
    ChangeNotifyManager*   notifyManager{nullptr};
    CapabilityId           capabilityId{kInvalidCapabilityId};

    static void Publish(
        void* userData)
    {
        auto* self =
            static_cast<StorePublishBinding*>(
                userData);

        if (self == nullptr ||
            self->store == nullptr ||
            self->notifyManager == nullptr)
        {
            return;
        }

        CapabilityPayload payload;

        if (!self->store->TryGet(
                self->capabilityId,
                payload))
        {
            return;
        }

        self->notifyManager->Notify(
            self->capabilityId,
            payload);
    }
};

// 呼ばれた回数を数えるだけの配信器。userData には int* を渡すこと。
inline void CountingPublish(
    void* userData)
{
    auto* counter =
        static_cast<int*>(
            userData);

    if (counter != nullptr)
    {
        ++(*counter);
    }
}

} // namespace rim
