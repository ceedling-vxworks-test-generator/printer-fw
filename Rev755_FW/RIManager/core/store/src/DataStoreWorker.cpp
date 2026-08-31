#include "RIMLog.hpp"

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
    // RIM_LOG_ERROR(
    //     "ExecuteOnce ENTER");

    RIMDataItem item{};

    if (!queue_.TryPop(item))
    {
        // RIM_LOG_ERROR(
        //     "queue empty");

        return false;
    }

    // RIM_LOG_ERROR(
    //     "popped id=%u",
    //     item.id);

    Process(item);

    return true;
}

void DataStoreWorker::Process(
    RIMDataItem& item)
{
    const auto* definition =
        context_
            .FindDataItem(
                {
                    RIMIdType::Data,
                    static_cast<std::uint32_t>(
                        item.id)
                });
                
    if (definition == nullptr)
    {
        return;
    }

    const auto domainId =
        context_
            .FindDomainId(
                {
                    RIMIdType::Data,
                    static_cast<std::uint32_t>(
                        item.id)
                });
                
    // RIM_LOG_INFO(
    //     "dataId=%u domainId=%u",
    //     item.id,
    //     domainId);

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

    // RIM_LOG_INFO(
    //     "Find domain=%u result=%p",
    //     domainId,
    //     static_cast<const void*>(storage));
        
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

    auto* mutableStorage =
        domainStore_.FindMutable(
            domainId);

    // RIM_LOG_INFO(
    //     "FindMutable domain=%u result=%p",
    //     domainId,
    //     static_cast<void*>(mutableStorage));
        
    if (mutableStorage == nullptr)
    {
        return;
    }

    mutableStorage->Store(
        item);

    CapabilityInput input{};

    input.changedId =
    {
        RIMIdType::Data,
        static_cast<std::uint32_t>(
            item.id)
    };

    capabilityQueue_.Push(
        input);
}

} // namespace rim