#pragma once

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"

namespace rim
{

class MachineCapabilityStore
{
public:

    void Store(
        const EnvironmentCapability& capability)
    {
        environment_ = capability;
    }

    const EnvironmentCapability&
    GetEnvironment() const
    {
        return environment_;
    }

    void Store(
        const ErrorCapability& capability)
    {
        error_ = capability;
    }

    const ErrorCapability&
    GetError() const
    {
        return error_;
    }

    void Store(
        const PrintReadyCapability& capability)
    {
        printReady_ = capability;
    }

    const PrintReadyCapability&
    GetPrintReady() const
    {
        return printReady_;
    }

    void Store(
        const ConsumableCapability& capability)
    {
        consumable_ = capability;
    }

    const ConsumableCapability&
    GetConsumable() const
    {
        return consumable_;
    }

    void Store(
        const JobCapability& capability)
    {
        job_ = capability;
    }

    const JobCapability&
    GetJob() const
    {
        return job_;
    }

private:

    EnvironmentCapability environment_{};

    ErrorCapability error_{};

    PrintReadyCapability printReady_{};

    ConsumableCapability consumable_{};

    JobCapability job_{};
};

} // namespace rim