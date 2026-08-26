#pragma once

#include "NotificationMessage.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"

namespace test
{

inline rim::NotificationTarget
CapabilityTarget(
    std::uint32_t capabilityId)
{
    return
    {
        rim::NotificationTargetType::Capability,
        capabilityId
    };
}

inline rim::NotificationTarget
DataTarget(
    std::uint32_t dataId)
{
    return
    {
        rim::NotificationTargetType::Data,
        dataId
    };
}

inline rim::NotificationTarget
FacadeTarget(
    std::uint32_t FacadeId)
{
    return
    {
        rim::NotificationTargetType::Facade,
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