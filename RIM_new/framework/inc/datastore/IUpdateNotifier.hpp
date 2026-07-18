#pragma once

//
// IUpdateNotifier - L2→L3 更新通知の抽象IF。
// rim の関数ポインタテーブル rim_update_notifier_t を抽象クラスへ置換。
// DataStore が変化ドメインを検出したときに OnUpdated を呼ぶ(通知先=CapabilityManager)。
//

#include "datastore/DomainSet.hpp"

namespace rim
{

class IUpdateNotifier
{
public:

    virtual ~IUpdateNotifier() = default;

    virtual void OnUpdated(DomainSet domains) = 0;
};

} // namespace rim
