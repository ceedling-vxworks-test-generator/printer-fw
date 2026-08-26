#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "DomainId.hpp"
#include "PartitionStorage.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class PartitionStorageRegistry
{
public:

    PartitionStorage&
    GetOrCreate(
        DomainId id)
    {
        std::lock_guard<std::mutex>
            lock(m_);

        auto& ptr =
            storages_[id];

        if (!ptr)
        {
            ptr =
                std::make_unique<
                    PartitionStorage>();
        }

        return *ptr;
    }

    const PartitionStorage*
    Find(
        DomainId id) const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        const auto it =
            storages_.find(id);

        if (it ==
            storages_.end())
        {
            return nullptr;
        }

        return it->second.get();
    }

    std::vector<DomainId>
    GetDomainIds() const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        std::vector<DomainId>
            result;

        result.reserve(
            storages_.size());

        for (const auto& [id, storage]
             : storages_)
        {
            result.push_back(id);
        }

        return result;
    }

    bool FindData(
        DomainId domainId,
        RIDataId dataId,
        RIMDataItem& item) const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        const auto it =
            storages_.find(domainId);

        if (it == storages_.end())
        {
            return false;
        }

        return it->second->Find(
            dataId,
            item);
    }

    std::vector<RIMDataItem>
    GetAll() const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        std::vector<RIMDataItem>
            result;

        for (const auto& [domainId, storage]
             : storages_)
        {
            auto items =
                storage->GetAll();

            result.insert(
                result.end(),
                items.begin(),
                items.end());
        }

        return result;
    }


void Dump() const
{
    std::lock_guard<std::mutex>
        lock(m_);

    std::cout
        << "\n=== Storage Dump ==="
        << std::endl;

    for (const auto& [domainId, storage]
         : storages_)
    {
        std::cout
            << "Domain="
            << domainId
            << std::endl;

        const auto items =
            storage->GetAll();

        for (const auto& item : items)
        {
            std::cout
                << "  id="
                << item.id
                << " type="
                << static_cast<int>(
                       item.value.type);

            switch (item.value.type)
            {
            case ValueType::kBool:
                std::cout
                    << " value="
                    << item.value.value.b;
                break;

            case ValueType::kInt32:
                std::cout
                    << " value="
                    << item.value.value.i32;
                break;

            case ValueType::kUInt32:
                std::cout
                    << " value="
                    << item.value.value.u32;
                break;

            case ValueType::kInt64:
                std::cout
                    << " value="
                    << item.value.value.i64;
                break;

            case ValueType::kUInt64:
                std::cout
                    << " value="
                    << item.value.value.u64;
                break;

            case ValueType::kDouble:
                std::cout
                    << " value="
                    << item.value.value.d;
                break;

            case ValueType::kString:
                std::cout
                    << " value="
                    << (item.value.value.str ?
                        item.value.value.str :
                        "(null)");
                break;

            case ValueType::kBinary:
                std::cout
                    << " size="
                    << item.value.size;
                break;

            default:
                break;
            }

            std::cout
                << std::endl;
        }
    }

    std::cout
        << "===================="
        << std::endl;
}



private:

    mutable std::mutex
        m_;

    std::unordered_map<
        DomainId,
        std::unique_ptr<
            PartitionStorage>>
        storages_;
};

} // namespace rim
