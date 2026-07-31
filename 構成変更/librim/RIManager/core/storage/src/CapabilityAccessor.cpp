#include "CapabilityAccessor.hpp"

namespace rim
{

EnvironmentCapability
CapabilityAccessor::GetEnvironment() const
{
    return GetImpl<
        EnvironmentCapability>();
}

ErrorCapability
CapabilityAccessor::GetError() const
{
    return GetImpl<
        ErrorCapability>();
}

PrintReadyCapability
CapabilityAccessor::GetPrintReady() const
{
    return GetImpl<
        PrintReadyCapability>();
}

ConsumableCapability
CapabilityAccessor::GetConsumable() const
{
    return GetImpl<
        ConsumableCapability>();
}

JobCapability
CapabilityAccessor::GetJob() const
{
    return GetImpl<
        JobCapability>();
}

} // namespace rim