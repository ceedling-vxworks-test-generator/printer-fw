#pragma once

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"

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

    void Store(
        const ErrorCapability& capability)
    {
        error_ = capability;
    }

    void Store(
        const PrintReadyCapability& capability)
    {
        printReady_ = capability;
    }

    const EnvironmentCapability&
    GetEnvironment() const
    {
        return environment_;
    }

    const ErrorCapability&
    GetError() const
    {
        return error_;
    }

    const PrintReadyCapability&
    GetPrintReady() const
    {
        return printReady_;
    }

private:

    EnvironmentCapability environment_{};

    ErrorCapability error_{};

    PrintReadyCapability printReady_{};
};

} // namespace rim