#pragma once

#include "MachineCapabilityStore.hpp"
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"

namespace rim
{

class CapabilityAccessor
{
public:

    explicit CapabilityAccessor(
        const MachineCapabilityStore& store)
        : store_(store)
    {
    }

    EnvironmentCapability
    GetEnvironment() const;

    ErrorCapability
    GetError() const;

    PrintReadyCapability
    GetPrintReady() const;

    ConsumableCapability
    GetConsumable() const;

    JobCapability
    GetJob() const;

private:

    template<typename T>
    T GetImpl() const;

    const MachineCapabilityStore&
        store_;
};

template<>
inline EnvironmentCapability
CapabilityAccessor::GetImpl<
    EnvironmentCapability>() const
{
    return store_.GetEnvironment();
}

template<>
inline ErrorCapability
CapabilityAccessor::GetImpl<
    ErrorCapability>() const
{
    return store_.GetError();
}

template<>
inline PrintReadyCapability
CapabilityAccessor::GetImpl<
    PrintReadyCapability>() const
{
    return store_.GetPrintReady();
}

template<>
inline ConsumableCapability
CapabilityAccessor::GetImpl<
    ConsumableCapability>() const
{
    return store_.GetConsumable();
}

template<>
inline JobCapability
CapabilityAccessor::GetImpl<
    JobCapability>() const
{
    return store_.GetJob();
}

} // namespace rim