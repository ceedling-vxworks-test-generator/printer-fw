#include <iostream>

#include "DataStoreDispatcher.hpp"
#include "DataItemDefinition.hpp"
#include "RIMDataItem.hpp"
#include "DataStoreQueue.hpp"
#include "ValueStore.hpp"
#include "IChangeChecker.hpp"

namespace rim
{

DataStoreDispatcher::DataStoreDispatcher(
    const ProductDefinition& product,
    DataStoreQueue& queue,
    ValueStore& store,
    const IChangeChecker& changeChecker,
    IRIMSnapshotReader& reader,
    CapabilityInputQueue& capabilityQueue,
    uint32_t priority)
    : product_(product)
    , queue_(queue)
    , store_(store)
    , changeChecker_(changeChecker)
    , reader_(reader)
    , capabilityQueue_(capabilityQueue)
    , priority_(priority)
{
}

DataStoreDispatcher::~DataStoreDispatcher()
{
    stop();
}

void DataStoreDispatcher::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;
    workerThread_ = std::thread(&DataStoreDispatcher::run, this);
}

void DataStoreDispatcher::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;
    queue_.shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

void DataStoreDispatcher::run()
{
    while (running_)
    {
        RIMDataItem oldItem{};
        RIMDataItem newItem{};

        if (!queue_.Pop(newItem))
        {
            break;
        }
        
        const RIMValue* oldValue = nullptr;

        if (store_.Find(newItem.id, oldItem))
        {
            oldValue = &oldItem.value;
        }

        if( !changeChecker_.isChanged(oldValue, newItem))
        {
            continue;
        }

        ProcessItem(newItem);

        const auto* definition = FindDataItem(product_, newItem.id);

        if (definition == nullptr)
        {
            continue;
        }

        CapabilityInput input{};

        input.snapshot = reader_.Read();
        input.changedDataId = newItem.id;
        capabilityQueue_.Push(input);
    }
}

bool DataStoreDispatcher::ExecuteOnce()
{
    RIMDataItem oldItem{};
    RIMDataItem newItem{};

    if (!queue_.TryPop(newItem))
    {
        return false;
    }

    const RIMValue* oldValue = nullptr;

    if (store_.Find(newItem.id, oldItem))
    {
        oldValue = &oldItem.value;
    }

    if (!changeChecker_.isChanged(oldValue, newItem))
    {
        return true;
    }

    ProcessItem(newItem);

    const auto* definition = FindDataItem(product_, newItem.id);

    if (definition == nullptr)
    {
        return false;
    }

    CapabilityInput input{};

    input.snapshot = reader_.Read();
    input.changedDataId = newItem.id;
    capabilityQueue_.Push(input);

    return true;
}

void DataStoreDispatcher::ProcessItem(
    RIMDataItem& item)
{
    const auto* definition =
        FindDataItem(
            product_,
            item.id);

    if (definition != nullptr)
    {
        RIMDataItem currentItem{};

        RIMValue currentValue{};

        if (store_.Find(
                item.id,
                currentItem))
        {
            currentValue =
                currentItem.value;
        }

        item.value =
            definition->store(
                currentValue,
                item.value,
                nullptr);

        item.valueType =
            definition->storeValueType;
    }

    store_.Store(
        item);
}

} // namespace rim