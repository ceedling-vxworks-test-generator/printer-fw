#pragma once

//
// RIMSystem - RIM 一括初期化・結線。
// 結線順(仕様): Publisher → Capability → DataStore(通知先=Capability) → Adapter。
// 機種可変(IInputClassifier / IRuleResolver / ICapabilityBuilder)は外部から注入する。
//

#include "datastore/DataStore.hpp"
#include "datastore/IInputClassifier.hpp"
#include "datastore/RIMResult.hpp"

#include "adapter/IRuleResolver.hpp"
#include "adapter/RawDataInput.hpp"

#include "capability/ICapabilityBuilder.hpp"
#include "capability/CapabilityManager.hpp"

#include "publisher/PublishEngine.hpp"

#include "accessor/PrinterStatusReader.hpp"

namespace rim
{

class RIMSystem
{
public:

    RIMSystem(
        const IInputClassifier& classifier,
        const IRuleResolver& resolver,
        const ICapabilityBuilder& builder)
        : store_(classifier)
        , capability_(store_.Reader(), builder, publisher_)
        , adapter_(store_, resolver, classifier)
        , accessor_(store_.Reader(), capability_)
    {
    }

    RIMResult Init()
    {
        publisher_.Init();
        capability_.Init();
        store_.Init(&capability_);   // 更新通知先 = Capability
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

    RawDataInput&        Adapter()       { return adapter_; }
    DataStore&           DataStoreRef()  { return store_; }
    PublishEngine&       Publisher()     { return publisher_; }
    CapabilityManager&   Capability()    { return capability_; }
    PrinterStatusReader& Accessor()      { return accessor_; }

private:

    DataStore           store_;
    PublishEngine       publisher_;
    CapabilityManager   capability_;
    RawDataInput        adapter_;
    PrinterStatusReader accessor_;
};

} // namespace rim
