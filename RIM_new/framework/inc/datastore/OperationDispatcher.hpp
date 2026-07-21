#pragma once

//
// OperationDispatcher - OperationReportQueue を取り込み OperationRegistry へ反映(仕様 §6.6)。
//

#include "datastore/InputQueues.hpp"
#include "datastore/OperationRegistry.hpp"
#include "datastore/IRegistryUpdateNotifier.hpp"
#include "datastore/RegistryDomain.hpp"

namespace rim
{

class OperationDispatcher
{
public:

    OperationDispatcher(OperationReportQueue& queue, OperationRegistry& registry)
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

    OperationReportQueue&    queue_;
    OperationRegistry&       registry_;
    IRegistryUpdateNotifier* notifier_{nullptr};
};

} // namespace rim
