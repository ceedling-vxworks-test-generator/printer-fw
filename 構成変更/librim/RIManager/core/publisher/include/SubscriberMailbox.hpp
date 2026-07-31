#pragma once

#include <queue>

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"

namespace rim
{

class SubscriberMailbox
{
public:

    void Push(
        const EnvironmentCapability& capability)
    {
        environmentQueue_.push(
            capability);
    }

    void Push(
        const ErrorCapability& capability)
    {
        errorQueue_.push(
            capability);
    }

    void Push(
        const PrintReadyCapability& capability)
    {
        printReadyQueue_.push(
            capability);
    }

    bool Pop(
        EnvironmentCapability& capability)
    {
        if (environmentQueue_.empty())
        {
            return false;
        }

        capability =
            environmentQueue_.front();

        environmentQueue_.pop();

        return true;
    }

    bool Pop(
        ErrorCapability& capability)
    {
        if (errorQueue_.empty())
        {
            return false;
        }

        capability =
            errorQueue_.front();

        errorQueue_.pop();

        return true;
    }

    bool Pop(
        PrintReadyCapability& capability)
    {
        if (printReadyQueue_.empty())
        {
            return false;
        }

        capability =
            printReadyQueue_.front();

        printReadyQueue_.pop();

        return true;
    }

private:

    std::queue<EnvironmentCapability>
        environmentQueue_;

    std::queue<ErrorCapability>
        errorQueue_;

    std::queue<PrintReadyCapability>
        printReadyQueue_;
};

} // namespace rim