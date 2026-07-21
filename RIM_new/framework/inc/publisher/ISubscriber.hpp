#pragma once

//
// ISubscriber - 外部購読者の抽象IF。関数ポインタ＋ctxを抽象クラスへ置換。
//

#include "capability/MachineCapability.hpp"

namespace rim
{

class ISubscriber
{
public:

    virtual ~ISubscriber() = default;

    virtual void OnPublish(const MachineCapability& capability) = 0;
};

} // namespace rim
