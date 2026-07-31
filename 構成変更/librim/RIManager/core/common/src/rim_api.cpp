#include "rim_api.h"

#include <cstddef>
#include <cstring>

#include "RIManager.hpp"
#include "ProductBinding.hpp"
#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "QueueInputPort.hpp"
#include "RawDataInput.hpp"
#include "ValueStore.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "FaultApplier.hpp"
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

//
// C 側ミラーと C++ 側列挙の値がずれていないことをビルド時に検査する。
// ずれたまま動くと「摂氏として送ったのに華氏として換算される」ような、
// 実行時まで気付けない不具合になるため。
//
static_assert(
    static_cast<int>(rim::SourceUnit::kNormalized) == RIM_UNIT_NORMALIZED,
    "rim_source_unit_t と rim::SourceUnit がずれている");
static_assert(
    static_cast<int>(rim::SourceUnit::kCelsius) == RIM_UNIT_CELSIUS,
    "rim_source_unit_t と rim::SourceUnit がずれている");
static_assert(
    static_cast<int>(rim::SourceUnit::kFahrenheit) == RIM_UNIT_FAHRENHEIT,
    "rim_source_unit_t と rim::SourceUnit がずれている");

static_assert(
    static_cast<int>(rim::FaultState::kNone) == RIM_FAULT_NONE,
    "rim_fault_state_t と rim::FaultState がずれている");
static_assert(
    static_cast<int>(rim::FaultState::kRaised) == RIM_FAULT_RAISED,
    "rim_fault_state_t と rim::FaultState がずれている");
static_assert(
    static_cast<int>(rim::FaultState::kCleared) == RIM_FAULT_CLEARED,
    "rim_fault_state_t と rim::FaultState がずれている");
static_assert(
    static_cast<int>(rim::FaultState::kAllCleared) == RIM_FAULT_ALL_CLEARED,
    "rim_fault_state_t と rim::FaultState がずれている");
static_assert(
    static_cast<int>(rim::FaultState::kUpdatedHeal) == RIM_FAULT_UPDATED_HEAL,
    "rim_fault_state_t と rim::FaultState がずれている");
static_assert(
    static_cast<int>(rim::FaultState::kUpdatedActive) == RIM_FAULT_UPDATED_ACTIVE,
    "rim_fault_state_t と rim::FaultState がずれている");

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

    //
    // Adapter(L1)
    //
    // 受理点は RawDataInput::Push の1本のみ。id -> Rule の対応は Product が
    // 持つ(PrinterADataProfile のテーブル)。
    //
    rim::QueueInputPort
        inputPort;

    // 規則表は Product が持つ(定義は products/ 側。core は宣言しか知らない)
    const rim::IRuleResolver*
        ruleResolver {rim::GetProductRuleResolver()};

    rim::RawDataInput
        adapter;

    rim::CapabilityInputQueue
        capabilityQueue;

    rim::PublisherInputQueue
        publisherQueue;

    //
    // Error
    //

    rim::ErrorRepository
        errorRepository;

    // 異常報告(context.faultState 付き)を ErrorRepository へ反映する
    rim::FaultApplier
        faultApplier;

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
        : inputPort(
            storeQueue)
        , adapter(
            inputPort,
            *ruleResolver)
        , faultApplier(
            errorRepository)
        , snapshotReader(
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
            capabilityQueue,
            &faultApplier)
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

// 異常報告を1件流す(異常系 API の共通処理)。
int PushFault(
    RIM_HANDLE handle,
    rim_fault_state_t state,
    std::uint32_t errorCode);

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

//
// データ投入の共通処理。
//
// **Adapter の受理点(RawDataInput::Push)を通す**。以前はキューへ直接積んで
// いたため、規則による正規化(単位換算・クランプ)を素通りしていた。
//
int InjectRaw(
    RIM_HANDLE handle,
    std::uint16_t dataId,
    double value,
    const rim_context_t* ctx)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        ToContext(handle);

    rim::DataContext dataContext{};

    if (ctx != nullptr)
    {
        if (ctx->has_unit)
        {
            dataContext.unit =
                static_cast<rim::SourceUnit>(
                    ctx->unit);
        }

        if (ctx->has_fault_state)
        {
            dataContext.faultState =
                static_cast<rim::FaultState>(
                    ctx->fault_state);
        }

        if (ctx->has_scale_x1000)
        {
            dataContext.scaleX1000 =
                ctx->scale_x1000;
        }

        if (ctx->has_key)
        {
            dataContext.key =
                ctx->key;
        }
    }

    const rim::RIMResult result =
        context->adapter.Push(
            static_cast<rim::RIMDataId>(
                dataId),
            rim::RawValue::Scalar(
                value),
            ctx != nullptr ? &dataContext : nullptr);

    return result == rim::RIMResult::kOk
           ? RI_SUCCESS
           : RI_INVALID_PARAMETER;
}

int PushFault(
    RIM_HANDLE handle,
    rim_fault_state_t state,
    std::uint32_t errorCode)
{
    rim_context_t ctx{};

    ctx.has_fault_state = true;
    ctx.fault_state     = state;
    ctx.has_key         = true;
    ctx.key             = errorCode;

    return InjectRaw(
        handle,
        0,          // 異常報告なので id は使われない
        0.0,
        &ctx);
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

//
// 以下の異常系 API は、いずれも Push に fault_state を添えた場合の短縮形である。
// 別実装にすると重大度の解決などが二重化するため、同じ経路へ寄せてある。
//

int RIManager_AddError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    return PushFault(
        handle,
        RIM_FAULT_RAISED,
        errorCode);
}

int RIManager_RemoveError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    return PushFault(
        handle,
        RIM_FAULT_CLEARED,
        errorCode);
}

int RIManager_SetErrorState(
    RIM_HANDLE handle,
    uint32_t errorCode,
    int state)
{
    // 0 = 発生中 / それ以外 = 回復済み(rim::ErrorState と同じ並び)
    return PushFault(
        handle,
        state == static_cast<int>(rim::ErrorState::kActive)
            ? RIM_FAULT_UPDATED_ACTIVE
            : RIM_FAULT_UPDATED_HEAL,
        errorCode);
}

int RIManager_Push(
    RIM_HANDLE handle,
    uint16_t dataId,
    double value,
    const rim_context_t* ctx)
{
    return InjectRaw(
        handle,
        dataId,
        value,
        ctx);
}

int RIManager_TestInjectDouble(
    RIM_HANDLE handle,
    uint16_t dataId,
    double value)
{
    return InjectRaw(
        handle,
        dataId,
        value,
        nullptr);
}

int RIManager_TestInjectInt32(
    RIM_HANDLE handle,
    uint16_t dataId,
    int32_t value)
{
    return InjectRaw(
        handle,
        dataId,
        static_cast<double>(
            value),
        nullptr);
}

int RIManager_TestInjectBool(
    RIM_HANDLE handle,
    uint16_t dataId,
    int value)
{
    return InjectRaw(
        handle,
        dataId,
        value != 0 ? 1.0 : 0.0,
        nullptr);
}

}
