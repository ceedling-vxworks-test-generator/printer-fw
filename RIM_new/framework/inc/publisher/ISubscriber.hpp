#pragma once

//
// ISubscriber - 購読者の抽象IF。rim の rim_subscriber_fn(関数ポインタ＋ctx)を置換。
//

#include "capability/CapabilitySet.hpp"

namespace rim
{

class ISubscriber
{
public:

    virtual ~ISubscriber() = default;

    virtual void OnPublish(const CapabilitySet& cap) = 0;
};

} // namespace rim
