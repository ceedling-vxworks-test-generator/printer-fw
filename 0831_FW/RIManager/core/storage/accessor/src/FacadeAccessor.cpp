#include "FacadeAccessor.hpp"

#include "RIMId.hpp"

namespace rim
{

bool
FacadeAccessor::TryGet(
    const RIMId& target,
    RIMValue& value) const
{
    if (target.type !=
        RIMIdType::Facade)
    {
        return false;
    }

    return TryGet(
        static_cast<RIFacadeId>(
            target.id),
        value);
}

}