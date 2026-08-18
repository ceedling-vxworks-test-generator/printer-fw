#include <iostream>

#include "DataStoreDispatcher.hpp"
#include "DataItemDefinition.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

DataStoreDispatcher::DataStoreDispatcher(
    const ProductDefinition& product,
    IQueue<RIMDataItem>& queue,
    DomainStorageRegistry& domainStore,
    IRIMSnapshotReader& reader,
    IQueue<CapabilityInput>& capabilityQueue)
    :
    product_(product),
    queue_(queue),
    domainStore_(domainStore),
    capabilityQueue_(capabilityQueue),
    reader_(reader),
    dataDomainMap_(product)
{
}

DataStoreDispatcher::~DataStoreDispatcher()
{
    stop();
}

void
DataStoreDispatcher::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            &DataStoreDispatcher::run,
            this);
}

void
DataStoreDispatcher::stop()
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
DataStoreDispatcher::run()
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

            input.snapshot =
                reader_.Read();

            input.changedDataId =
                item.id;

            capabilityQueue_.Push(
                input);
        }
    }
}

bool
DataStoreDispatcher::ExecuteOnce()
{
    RIMDataItem item{};

    if (!queue_.TryPop(item))
    {
        return false;
    }

    if (ProcessItem(item))
    {
        CapabilityInput input{};

        input.snapshot =
            reader_.Read();

        input.changedDataId =
            item.id;

        capabilityQueue_.Push(
            input);
    }

    return true;
}

bool
DataStoreDispatcher::ProcessItem(
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

        item.valueType =
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