#include <iostream>

#include "DataStoreWorker.hpp"
#include "DataItemDefinition.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

DataStoreWorker::DataStoreWorker(
    const ProductDefinition& product,
    IQueue<RIMDataItem>& queue,
    DomainStorageRegistry& domainStore,
    IQueue<CapabilityInput>& capabilityQueue)
    :
    product_(product),
    queue_(queue),
    domainStore_(domainStore),
    capabilityQueue_(capabilityQueue),
    dataDomainMap_(product)
{
}

DataStoreWorker::~DataStoreWorker()
{
    stop();
}

void
DataStoreWorker::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            &DataStoreWorker::run,
            this);
}

void
DataStoreWorker::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

void
DataStoreWorker::run()
{
    while (running_)
    {
        RIMDataItem item{};

        if (!queue_.WaitAndPop(item))
        {
            break;
        }

        if (ProcessItem(item))
        {
            CapabilityInput input{};

            input.changedDataId =
                item.id;

            capabilityQueue_.Push(
                input);
        }
    }
}

bool
DataStoreWorker::ExecuteOnce()
{
    RIMDataItem item{};

    if (!queue_.TryPop(item))
    {
        return false;
    }

    if (ProcessItem(item))
    {
        CapabilityInput input{};

        input.changedDataId =
            item.id;

        capabilityQueue_.Push(
            input);
    }

    return true;
}

bool
DataStoreWorker::ProcessItem(
    RIMDataItem& item)
{
    bool result = false;

    const auto domainId =
        dataDomainMap_.Find(
            item.id);

    const auto* definition =
        FindDataItem(
            product_,
            item.id);

    if (definition != nullptr)
    {
        RIMDataItem currentItem{};

        RIMValue currentValue{};

        bool exists = false;

        if (domainId !=
            kInvalidDomainId)
        {
            const auto* storage =
                domainStore_.Find(
                    domainId);

            if (storage != nullptr)
            {
                exists =
                    storage->Find(
                        item.id,
                        currentItem);

                if (exists)
                {
                    currentValue =
                        currentItem.value;
                }
            }
        }

        item.value =
            definition->store(
                currentValue,
                item.value,
                nullptr);

        item.value.type =
            definition->storeValueType;

        if (exists &&
            definition->diff != nullptr)
        {
            result =
                definition->diff(
                    currentValue,
                    item.value);
        }
        else
        {
            result = true;
        }
    }

    if (domainId !=
        kInvalidDomainId)
    {
        domainStore_
            .GetOrCreate(
                domainId)
            .Store(
                item);
    }

    return result;
}

} // namespace rim