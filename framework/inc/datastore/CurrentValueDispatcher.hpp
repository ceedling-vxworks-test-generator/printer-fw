#pragma once

//
// CurrentValueDispatcher - 周期起動し CurrentValueBuffer の更新値を取り出して
// CurrentValueRegistry へ反映(仕様 §6.7)。変更があれば notifyUpdated。
//

#include "datastore/CurrentValueBuffer.hpp"
#include "datastore/CurrentValueRegistry.hpp"
#include "datastore/IRegistryUpdateNotifier.hpp"
#include "datastore/RegistryDomain.hpp"

namespace rim
{

class CurrentValueDispatcher
{
public:

    CurrentValueDispatcher(CurrentValueBuffer& buffer, CurrentValueRegistry& registry)
        : buffer_(buffer), registry_(registry)
    {
    }

    void SetNotifier(IRegistryUpdateNotifier* notifier) { notifier_ = notifier; }

    // 周期実行を1回分駆動する。
    void Dispatch()
    {
        RegistryDomainSet changed = kDomainNone;
        for (const RIMDataItem& item : buffer_.TakeUpdatedValues()) {
            if (auto d = registry_.Apply(item)) changed |= *d;
        }
        if (changed != kDomainNone && notifier_) notifier_->NotifyUpdated(changed);
    }

private:

    CurrentValueBuffer&      buffer_;
    CurrentValueRegistry&    registry_;
    IRegistryUpdateNotifier* notifier_{nullptr};
};

} // namespace rim
