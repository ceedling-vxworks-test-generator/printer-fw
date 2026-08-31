#include "CapabilityAccessor.hpp"

#include "RIMId.hpp"

namespace rim
{

bool
CapabilityAccessor::TryGet(
    const RIMId& target,
    RIMValue& value) const
{
    if (target.type !=
        RIMIdType::Capability)
    {
        return false;
    }

    return TryGet(
        static_cast<RICapabilityId>(
            target.id),
        value);
}

}
