#pragma once

//
// IPublisher - RIM_CapabilityLayer が生成済Capabilityを RIM_PublisherLayer へ渡すIF(仕様 §8.8)。
//

#include "capability/MachineCapability.hpp"

namespace rim
{

class IPublisher
{
public:

    virtual ~IPublisher() = default;

    virtual void Notify(const MachineCapability& capability) = 0;
};

} // namespace rim
