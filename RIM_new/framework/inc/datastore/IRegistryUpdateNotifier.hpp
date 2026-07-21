#pragma once

//
// IRegistryUpdateNotifier - L2→L3 更新通知IF(仕様 §6.5〜)。
// Dispatcher が変更を検出したとき notifyUpdated(RegistryDomainSet) を呼ぶ。
// RIM_CapabilityLayer(CapabilityManager) が実装する。
//

#include "datastore/RegistryDomain.hpp"

namespace rim
{

class IRegistryUpdateNotifier
{
public:

    virtual ~IRegistryUpdateNotifier() = default;

    virtual void NotifyUpdated(RegistryDomainSet domains) = 0;
};

} // namespace rim
