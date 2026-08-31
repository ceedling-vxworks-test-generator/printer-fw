#pragma once

#include "RIMId.hpp"
#include "RIId.hpp"

namespace rim::test
{

inline RIMId ToDataRIMId(
    RIDataId id)
{
    return
    {
        RIMIdType::Data,
        static_cast<std::uint32_t>(id)
    };
}

inline RIMId ToCapabilityRIMId(
    RICapabilityId id)
{
    return
    {
        RIMIdType::Capability,
        static_cast<std::uint32_t>(id)
    };
}

inline RIMId ToFacadeRIMId(
    RIFacadeId id)
{
    return
    {
        RIMIdType::Facade,
        static_cast<std::uint32_t>(id)
    };
}

inline RIDataId
ToDataId(
    const RIMId& id)
{
    return static_cast<RIDataId>(
        id.id);
}

inline RICapabilityId
ToCapabilityId(
    const RIMId& id)
{
    return static_cast<RICapabilityId>(
        id.id);
}

inline RIFacadeId
ToFacadeId(
    const RIMId& id)
{
    return static_cast<RIFacadeId>(
        id.id);
}

inline RIMId DataId(
    RIDataId id)
{
    return ToDataRIMId(id);
}

inline RIMId CapabilityId(
    RIDataId id)
{
    return ToCapabilityRIMId(id);
}

inline RIMId FacadeId(
    RIDataId id)
{
    return ToFacadeRIMId(id);
}

}
