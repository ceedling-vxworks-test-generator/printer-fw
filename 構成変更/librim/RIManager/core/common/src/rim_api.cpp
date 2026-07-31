#include "rim_api.h"

#include <cstddef>
#include <cstring>

#include "RIManager.hpp"
#include "ProductBinding.hpp"
#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "ValueStore.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "DataStoreWorker.hpp"
#include "RIMValueFactory.hpp"

#include "CapabilityInputQueue.hpp"
#include "CapabilityEvaluator.hpp"
#include "CapabilityStore.hpp"
#include "CapabilityWorker.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"
#include "PublishManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"

#include "IErrorDefinitionRegistry.hpp"

namespace
{

struct RIManagerContext;

//
// CallbackSlot - C の {コールバック, userData} の組を保持する枠。
//
// Core の購読 IF は「関数ポインタ + userData(1個)」なので、C 側の
// コールバックと userData の**2つ**をそのまま渡せない。
// この枠を1つ確保して、そのアドレスを userData として渡す(トランポリン方式)。
// 枠は固定長配列から払い出すため動的確保は起きない。
//
struct CallbackSlot
{
    RICapabilityCallback callback {nullptr};
    void*                userData {nullptr};
    rim::SubscriptionId  subscriptionId {rim::kInvalidSubscriptionId};
    bool                 used {false};
};

// Core から呼ばれ、C のコールバックへ橋渡しする。
void CapabilityTrampoline(
    rim::SubscriptionId subscriptionId,
    rim::CapabilityId capabilityId,
    const rim::CapabilityPayload& payload,
    void* userData)
{
    auto* slot =
        static_cast<CallbackSlot*>(
            userData);

    if (slot == nullptr ||
        slot->callback == nullptr)
    {
        return;
    }

    slot->callback(
        subscriptionId,
        capabilityId,
        payload.Data(),
        payload.Size(),
        slot->userData);
}

//
// StorePublisher - 「現在値を Store から読んで通知経路へ流す」配信器。
//
// 旧実装は Capability ごとに別々のラムダを std::function で包み、
// unique_ptr で確保して map に入れていた。中身は全 Capability で同じなので、
// id を持つだけの同型オブジェクトを id の数だけ**値で**並べれば足りる
// (動的確保なし、std::function なし)。
//
class StorePublisher final : public rim::ICapabilityPublisher
{
public:

    void Bind(
        RIManagerContext* context,
        rim::CapabilityId id)
    {
        context_ = context;
        id_      = id;
    }

    bool IsBound() const
    {
        return context_ != nullptr;
    }

    // 定義は RIManagerContext の後(完全型が要るため)。
    void Publish() override;

private:

    RIManagerContext* context_ {nullptr};

    rim::CapabilityId id_ {rim::kInvalidCapabilityId};
};

//
// RIManagerContext - ランタイム一式。
//
// 旧実装はここで "Environment" / "Error" / … と機種固有の Capability を
// 名指しで5つ登録していた。本実装は Product が提供する規則一覧
// (ICapabilityRuleProvider)を回して登録するだけなので、Capability が
// いくつあるかを Core は知らない。
//
struct RIManagerContext
{
    //
    // Queue
    //

    rim::StoreInputQueue
        storeQueue;

    rim::CapabilityInputQueue
        capabilityQueue;

    rim::PublisherInputQueue
        publisherQueue;

    //
    // Error
    //

    rim::ErrorRepository
        errorRepository;

    //
    // Datastore
    //

    rim::ValueStore
        valueStore;

    rim::AggregateRIMSnapshotReader
        snapshotReader;

    //
    // Capability
    //

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityEvaluator
        capabilityEvaluator;

    // Product が持ち込む規則一式(定義は products/ 側。core は宣言しか知らない)
    rim::ICapabilityRuleProvider*
        ruleProvider {nullptr};

    //
    // Publisher
    //

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager
        notifyManager;

    // publishManager は publisherRegistry を参照するため、**宣言順を逆にしない**こと
    // (メンバの初期化はここでの宣言順に行われる)。
    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::PublishManager
        publishManager;

    // Capability ごとの配信器(値で保持。動的確保しない)
    StorePublisher
        publishers[rim::kCapabilityMaxCount] {};

    // C コールバックの保持枠(同上)
    CallbackSlot
        callbackSlots[rim::kCapabilitySubscriptionMaxCount] {};

    //
    // Worker
    //

    rim::DataStoreWorker
        dataStoreWorker;

    rim::CapabilityWorker
        capabilityWorker;

    rim::PublisherWorker
        publisherWorker;

    //
    // Accessor
    //

    rim::NotificationReceiver
        receiver;

    rim::RIManager
        facade;

    rim::CapabilityAccessor
        capabilityAccessor;

    //
    // Error
    //

    const rim::IErrorDefinitionRegistry*
        errorRegistry {};

    RIManagerContext()
        : snapshotReader(
            valueStore)
        , notifyManager(
            mailbox,
            callbackRegistry)
        , publisherRegistry()
        , publishManager(
            publisherRegistry)
        , dataStoreWorker(
            storeQueue,
            valueStore,
            snapshotReader,
            capabilityQueue)
        , capabilityWorker(
            capabilityQueue,
            capabilityEvaluator,
            capabilityStore,
            publisherQueue)
        , publisherWorker(
            publisherQueue,
            publishManager)
        , receiver(
            mailbox)
        , facade(
            receiver)
        , capabilityAccessor(
            capabilityStore)
        , errorRegistry(
            nullptr)
    {
        const rim::ProductServices services
        {
            errorRepository
        };

        ruleProvider =
            rim::CreateProductCapabilityRuleProvider(
                services);

        if (ruleProvider != nullptr)
        {
            ruleProvider->RegisterTo(
                capabilityEvaluator);
        }

        RegisterPublishers();
    }

    ~RIManagerContext()
    {
        rim::DestroyProductCapabilityRuleProvider(
            ruleProvider);

        ruleProvider = nullptr;
    }

    RIManagerContext(
        const RIManagerContext&) = delete;

    RIManagerContext& operator=(
        const RIManagerContext&) = delete;

private:

    // 登録された規則の数だけ、同じ形の配信器を用意する。
    void RegisterPublishers()
    {
        const std::size_t count =
            capabilityEvaluator.RuleCount();

        for (std::size_t i = 0; i < count; ++i)
        {
            const rim::ICapabilityRule* rule =
                capabilityEvaluator.RuleAt(i);

            if (rule == nullptr)
            {
                continue;
            }

            const rim::CapabilityId id =
                rule->Id();

            if (id >= rim::kCapabilityMaxCount)
            {
                continue;
            }

            if (publishers[id].IsBound())
            {
                continue;
            }

            publishers[id].Bind(
                this,
                id);

            publisherRegistry.Register(
                id,
                &publishers[id]);
        }
    }
};

void StorePublisher::Publish()
{
    if (context_ == nullptr)
    {
        return;
    }

    rim::CapabilityPayload payload;

    if (!context_->capabilityStore.TryGet(
            id_,
            payload))
    {
        return;
    }

    context_->notifyManager.Notify(
        id_,
        payload);
}

void ReevaluateCapabilities(
    RIManagerContext& context)
{
    const auto snapshot =
        context.snapshotReader.Read();

    context.capabilityQueue.Push(
        snapshot);
}

rim::ErrorSeverity
ResolveErrorSeverity(
    const RIManagerContext& context,
    std::uint32_t errorCode)
{
    rim::ErrorSeverity severity =
        rim::ErrorSeverity::kError;

    if (context.errorRegistry != nullptr)
    {
        context.errorRegistry
            ->TryGetSeverity(
                errorCode,
                severity);
    }

    return severity;
}

RIManagerContext* ToContext(
    RIM_HANDLE handle)
{
    return static_cast<
        RIManagerContext*>(
            handle);
}

// Capability の中身を利用者バッファへ写す。
// 足りない場合は書かずに RI_BUFFER_TOO_SMALL(呼出側が通知を消費しないで済む)。
int CopyOut(
    const rim::CapabilityPayload& payload,
    void* buffer,
    std::size_t bufferSize,
    std::size_t* written)
{
    if (payload.Size() > bufferSize)
    {
        return RI_BUFFER_TOO_SMALL;
    }

    std::memcpy(
        buffer,
        payload.Data(),
        payload.Size());

    if (written != nullptr)
    {
        *written = payload.Size();
    }

    return RI_SUCCESS;
}

// 試験用投入の共通処理。
int InjectValue(
    RIM_HANDLE handle,
    std::uint16_t dataId,
    rim::ValueType valueType,
    const rim::RIMValue& value)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    rim::RIMDataItem item{};

    item.id =
        static_cast<rim::RIMDataId>(
            dataId);

    item.valueType = valueType;

    item.value = value;

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

}


extern "C"
{


int RIManager_Create(
    RIM_HANDLE* handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    *handle =
        new RIManagerContext();

    return RI_SUCCESS;
}

int RIManager_Destroy(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    context->publisherWorker.Stop();

    context->capabilityWorker.Stop();

    context->dataStoreWorker.Stop();

    delete context;

    return RI_SUCCESS;
}

int RIManager_Start(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    context->dataStoreWorker.Run();

    context->capabilityWorker.Run();

    context->publisherWorker.Run();

    return RI_SUCCESS;
}

int RIManager_Stop(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    context->publisherWorker.Stop();

    context->capabilityWorker.Stop();

    context->dataStoreWorker.Stop();

    return RI_SUCCESS;
}

int RIManager_GetCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    void* buffer,
    size_t bufferSize,
    size_t* written)
{
    if (handle == nullptr ||
        buffer == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        ToContext(handle);

    // バッファ不足で捨てないよう、まず覗いてから消費する。
    if (!context->facade.HasPending(
            capabilityId))
    {
        return RI_NO_DATA;
    }

    rim::CapabilityPayload payload;

    if (!context->facade.TryGet(
            capabilityId,
            payload))
    {
        return RI_NO_DATA;
    }

    return CopyOut(
        payload,
        buffer,
        bufferSize,
        written);
}

int RIManager_GetCurrentCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    void* buffer,
    size_t bufferSize,
    size_t* written)
{
    if (handle == nullptr ||
        buffer == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        ToContext(handle);

    rim::CapabilityPayload payload;

    if (!context->capabilityAccessor.TryGet(
            capabilityId,
            payload))
    {
        return RI_NO_DATA;
    }

    return CopyOut(
        payload,
        buffer,
        bufferSize,
        written);
}

int RIManager_HasPendingCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    return context->facade.HasPending(
               capabilityId)
           ? 1
           : 0;
}

int RIManager_SubscribeCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    RICapabilityCallback callback,
    void* userData,
    uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        ToContext(handle);

    // 空き枠を探す
    CallbackSlot* slot = nullptr;

    for (auto& candidate : context->callbackSlots)
    {
        if (!candidate.used)
        {
            slot = &candidate;
            break;
        }
    }

    if (slot == nullptr)
    {
        return RI_INTERNAL_ERROR;
    }

    slot->callback = callback;
    slot->userData = userData;

    const auto id =
        context->callbackRegistry.Subscribe(
            capabilityId,
            CapabilityTrampoline,
            slot);

    if (id == rim::kInvalidSubscriptionId)
    {
        // 枠を戻してから失敗を返す
        *slot = CallbackSlot{};

        return RI_INTERNAL_ERROR;
    }

    slot->subscriptionId = id;
    slot->used           = true;

    *subscriptionId = id;

    return RI_SUCCESS;
}

int RIManager_Unsubscribe(
    RIM_HANDLE handle,
    uint64_t subscriptionId)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    const bool removed =
        context->callbackRegistry.Unsubscribe(
            subscriptionId);

    if (removed)
    {
        // 枠を解放する(解放し忘れると購読数の上限に達してしまう)
        for (auto& slot : context->callbackSlots)
        {
            if (slot.used &&
                slot.subscriptionId == subscriptionId)
            {
                slot = CallbackSlot{};
                break;
            }
        }
    }

    return removed
           ? RI_SUCCESS
           : RI_NO_DATA;
}

int RIManager_AddError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    if (context->errorRepository.Add(
        {
            errorCode,
            rim::ErrorState::kActive,
            ResolveErrorSeverity(
                *context,
                errorCode)
        }))
    {
        ReevaluateCapabilities(
            *context);
    }

    return RI_SUCCESS;
}

int RIManager_RemoveError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    context->errorRepository.Remove(
        errorCode);

    ReevaluateCapabilities(
        *context);

    return RI_SUCCESS;
}

int RIManager_SetErrorState(
    RIM_HANDLE handle,
    uint32_t errorCode,
    int state)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    context->errorRepository.SetState(
        errorCode,
        static_cast<rim::ErrorState>(
            state));

    ReevaluateCapabilities(
        *context);

    return RI_SUCCESS;
}

int RIManager_TestInjectDouble(
    RIM_HANDLE handle,
    uint16_t dataId,
    double value)
{
    return InjectValue(
        handle,
        dataId,
        rim::ValueType::kDouble,
        rim::RIMValueFactory::CreateDouble(
            value));
}

int RIManager_TestInjectInt32(
    RIM_HANDLE handle,
    uint16_t dataId,
    int32_t value)
{
    return InjectValue(
        handle,
        dataId,
        rim::ValueType::kInt32,
        rim::RIMValueFactory::CreateInt32(
            value));
}

int RIManager_TestInjectBool(
    RIM_HANDLE handle,
    uint16_t dataId,
    int value)
{
    return InjectValue(
        handle,
        dataId,
        rim::ValueType::kBool,
        rim::RIMValueFactory::CreateBool(
            value != 0));
}

}
