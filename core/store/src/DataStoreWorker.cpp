#include "DataStoreWorker.hpp"
#include "DataItemDefinition.hpp"
#include "RIMDataItem.hpp"
#include "BinaryHash.hpp"
#include "RIMValueAccessor.hpp"
#include "BinaryInfo.hpp"

namespace rim
{
DataStoreWorker::~DataStoreWorker()
{
    stop();
}

void DataStoreWorker::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ = std::thread(&DataStoreWorker::WorkerLoop, this);
}

void DataStoreWorker::stop()
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

void DataStoreWorker::WorkerLoop()
{
    while (running_)
    {
        RIMDataItem item{};

        if (!queue_.WaitAndPop(item))
        {
            break;
        }

        Process(item);
    }
}

bool DataStoreWorker::ExecuteOnce()
{
    RIMDataItem item{};

    if (!queue_.TryPop(item))
    {
        return false;
    }

    Process(item);

    return true;
}

void DataStoreWorker::Process(
    RIMDataItem& item)
{
    const auto* definition =
        context_
            .FindDataItem(
                item.id);

    if (definition == nullptr)
    {
        return;
    }

    const auto domainId =
        context_
            .FindDataDomainId(
                item.id);

    if (domainId ==
        kInvalidDomainId)
    {
        return;
    }

    RIMValue currentValue{};
    bool exists = false;

    const auto* storage =
        domainStore_.Find(
            domainId);

    if (storage != nullptr)
    {
        RIMDataItem currentItem{};

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
        bool changed = true;

        if (currentValue.type ==
                ValueType::kBinary &&
            item.value.type ==
                ValueType::kBinary &&
            storage != nullptr)
        {
            BinaryInfo currentInfo{};

            const std::uint8_t* bytes{};
            std::size_t size{};

            if (storage->GetBinaryInfo(
                    item.id,
                    currentInfo) &&
                RIMValueAccessor::GetBinary(
                    item.value,
                    bytes,
                    size))
            {
                if (currentInfo.size !=
                    size)
                {
                    changed = true;
                }
                else
                {
                    const auto newHash =
                        CalculateBinaryHash(
                            bytes,
                            size);

                    if (currentInfo.hash !=
                        newHash)
                    {
                        changed = true;
                    }
                    else
                    {
                        changed =
                            definition->diff(
                                currentValue,
                                item.value);
                    }
                }
            }
            else
            {
                changed =
                    definition->diff(
                        currentValue,
                        item.value);
            }
        }
        else
        {
            changed =
                definition->diff(
                    currentValue,
                    item.value);
        }

        if (!changed)
        {
            return;
        }
    }

    domainStore_
        .GetOrCreate(
            domainId)
        .Store(
            item);

    CapabilityInput input{};

    input.changedDataId =
        item.id;

    capabilityQueue_.Push(
        input);
}

} // namespace rim