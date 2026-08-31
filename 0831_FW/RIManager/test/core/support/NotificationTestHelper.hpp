#pragma once

#include "NotificationMessage.hpp"
#include "RIMId.hpp"
#include "NotificationTrigger.hpp"

namespace test
{

inline rim::RIMId
CapabilityTarget(
    std::uint32_t capabilityId)
{
    return
    {
        rim::RIMIdType::Capability,
        capabilityId
    };
}

inline rim::RIMId
DataTarget(
    std::uint32_t dataId)
{
    return
    {
        rim::RIMIdType::Data,
        dataId
    };
}

inline rim::RIMId
FacadeTarget(
    std::uint32_t FacadeId)
{
    return
    {
        rim::RIMIdType::Facade,
        FacadeId
    };
}

inline rim::NotificationMessage
OnChangeMessage(
    std::uint32_t capabilityId)
{
    return
    {
        CapabilityTarget(capabilityId),
        rim::NotificationTrigger::OnChange
    };
}

inline rim::NotificationMessage
PeriodicMessage(
    std::uint32_t capabilityId)
{
    return
    {
        CapabilityTarget(capabilityId),
        rim::NotificationTrigger::Periodic
    };
}

}