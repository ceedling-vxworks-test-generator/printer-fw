#pragma once

#include <vector>
#include <algorithm>

#include "CallbackSubscriber.hpp"

namespace rim
{

struct EnvironmentSubscription
{
    SubscriptionId id;

    EnvironmentCallback callback;
};

struct ErrorSubscription
{
    SubscriptionId id;

    ErrorCallback callback;
};

struct PrintReadySubscription
{
    SubscriptionId id;

    PrintReadyCallback callback;
};

struct ConsumableSubscription
{
    SubscriptionId id;

    ConsumableCallback callback;
};

struct JobSubscription
{
    SubscriptionId id;

    JobCallback callback;
};

class CallbackSubscriptionRegistry
{
public:

    SubscriptionId
    SubscribeEnvironment(
        EnvironmentCallback callback)
    {
        const auto id =
            nextId_++;

        environments_.push_back(
            {
                id,
                std::move(callback)
            });

        return id;
    }
 
    void NotifyEnvironment(
        const EnvironmentCapability& capability)
    {
        for (const auto& entry
            : environments_)
        {
            entry.callback(
                entry.id,
                capability);
        }
    }

    SubscriptionId
    SubscribeError(
        ErrorCallback callback)
    {
        const auto id =
            nextId_++;

        errors_.push_back(
            {
                id,
                std::move(callback)
            });

        return id;
    }

    void NotifyError(
        const ErrorCapability& capability)
    {
        for (const auto& entry
            : errors_)
        {
            entry.callback(
                entry.id,
                capability);
        }
    }

    SubscriptionId
    SubscribePrintReady(
        PrintReadyCallback callback)
    {
        const auto id =
            nextId_++;

        printReadies_.push_back(
            {
                id,
                std::move(callback)
            });

        return id;
    }

    void NotifyPrintReady(
        const PrintReadyCapability& capability)
    {
        for (const auto& entry
            : printReadies_)
        {
            entry.callback(
                entry.id,
                capability);
        }
    }

    SubscriptionId
    SubscribeConsumable(
        ConsumableCallback callback)
    {
        const auto id =
            nextId_++;

        consumables_.push_back(
            {
                id,
                std::move(callback)
            });

        return id;
    }

    void NotifyConsumable(
        const ConsumableCapability& capability)
    {
        for (const auto& entry
            : consumables_)
        {
            entry.callback(
                entry.id,
                capability);
        }
    }

    SubscriptionId
    SubscribeJob(
        JobCallback callback)
    {
        const auto id =
            nextId_++;

        jobs_.push_back(
            {
                id,
                std::move(callback)
            });

        return id;
    }

    void NotifyJob(
        const JobCapability& capability)
    {
        for (const auto& entry
            : jobs_)
        {
            entry.callback(
                entry.id,
                capability);
        }
    }   



    bool Unsubscribe(
        SubscriptionId id)
    {
        const auto environmentOldSize =
            environments_.size();

        environments_.erase(
            std::remove_if(
                environments_.begin(),
                environments_.end(),
                [&](const auto& entry)
                {
                    return entry.id == id;
                }),
            environments_.end());

        const auto errorOldSize =
            errors_.size();

        errors_.erase(
            std::remove_if(
                errors_.begin(),
                errors_.end(),
                [&](const auto& entry)
                {
                    return entry.id == id;
                }),
            errors_.end());

        const auto printReadyOldSize =
            printReadies_.size();

        printReadies_.erase(
            std::remove_if(
                printReadies_.begin(),
                printReadies_.end(),
                [&](const auto& entry)
                {
                    return entry.id == id;
                }),
            printReadies_.end());

        const auto consumableOldSize =
            consumables_.size();

        consumables_.erase(
            std::remove_if(
                consumables_.begin(),
                consumables_.end(),
                [&](const auto& entry)
                {
                    return entry.id == id;
                }),
            consumables_.end());

        const auto jobOldSize =
            jobs_.size();

        jobs_.erase(
            std::remove_if(
                jobs_.begin(),
                jobs_.end(),
                [&](const auto& entry)
                {
                    return entry.id == id;
                }),
            jobs_.end());


        return
            environmentOldSize != environments_.size()
            || errorOldSize != errors_.size()
            || printReadyOldSize != printReadies_.size()
            || consumableOldSize != consumables_.size()
            || jobOldSize != jobs_.size();
    }

private:

    SubscriptionId
        nextId_{1};

    std::vector<
        EnvironmentSubscription>
        environments_;

    std::vector<
        ErrorSubscription>
        errors_;

    std::vector<
        PrintReadySubscription>
        printReadies_;

    std::vector<
        ConsumableSubscription>
        consumables_;    
        
    std::vector<
        JobSubscription>
        jobs_;       

};

}