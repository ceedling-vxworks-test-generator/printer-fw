#pragma once

//
// FaultDispatcher - FaultInputQueue を取り込み FaultRegistry へ反映(仕様 §6.5)。
// 変更が発生したら IRegistryUpdateNotifier で変更ドメインを通知する。
//

#include "datastore/InputQueues.hpp"
#include "datastore/FaultRegistry.hpp"
#include "datastore/IRegistryUpdateNotifier.hpp"
#include "datastore/RegistryDomain.hpp"

namespace rim
{

class FaultDispatcher
{
public:

    FaultDispatcher(FaultInputQueue& queue, FaultRegistry& registry)
        : queue_(queue), registry_(registry)
    {
    }

    void SetNotifier(IRegistryUpdateNotifier* notifier) { notifier_ = notifier; }

    void Dispatch()
    {
        RegistryDomainSet changed = kDomainNone;
        RIMDataItem item;
        while (queue_.Dequeue(item)) {
            if (auto d = registry_.Apply(item)) changed |= *d;
        }
        if (changed != kDomainNone && notifier_) notifier_->NotifyUpdated(changed);
    }

private:

    FaultInputQueue&         queue_;
    FaultRegistry&           registry_;
    IRegistryUpdateNotifier* notifier_{nullptr};
};

} // namespace rim
