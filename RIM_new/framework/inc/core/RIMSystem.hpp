#pragma once

//
// RIMSystem - RIM 一括初期化・結線(rim_core.cpp 相当)。
// 結線順: Publisher → Capability → DataStore(通知先=Capability) → Adapter。
// 機種可変(IDataProfile / ICapabilityEvaluatorSet)は外部から注入する。
// framework の各レイヤ実体を所有し、Adapter/Accessor 経由で利用する。
//

#include "datastore/DataStore.hpp"
#include "datastore/RIMResult.hpp"

#include "adapter/IDataProfile.hpp"
#include "adapter/Adapter.hpp"

#include "capability/ICapabilityEvaluatorSet.hpp"
#include "capability/CapabilityManager.hpp"

#include "publisher/Publisher.hpp"

#include "accessor/Accessor.hpp"

namespace rim
{

class RIMSystem
{
public:

    RIMSystem(
        const IDataProfile& profile,
        const ICapabilityEvaluatorSet& evaluators)
        : store_(profile)
        , capability_(store_, evaluators, publisher_)
        , adapter_(store_, profile)
        , accessor_(store_, capability_)
    {
    }

    // Publisher→Capability→DataStore(notifier=Capability)→Adapter の順で結線。
    RIMResult Init()
    {
        publisher_.Init();
        capability_.Init();
        RIMResult r = store_.Init(&capability_);
        if (r != RIMResult::kOk) return r;
        return RIMResult::kOk;
    }

    void Shutdown()
    {
        store_.Shutdown();
        capability_.Shutdown();
        publisher_.Shutdown();
    }

    // Dispatcher駆動(周期/イベント。スケルトンでは明示呼び出し)。
    void Dispatch() { store_.Dispatch(); }

    Adapter&           AdapterRef()    { return adapter_; }
    DataStore&         DataStoreRef()  { return store_; }
    Publisher&         PublisherRef()  { return publisher_; }
    CapabilityManager& CapabilityRef() { return capability_; }
    Accessor&          AccessorRef()   { return accessor_; }

private:

    Publisher         publisher_;
    DataStore         store_;
    CapabilityManager capability_;
    Adapter           adapter_;
    Accessor          accessor_;
};

} // namespace rim
