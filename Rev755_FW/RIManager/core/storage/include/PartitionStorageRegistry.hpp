#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <RIMLog.hpp>

#include "DomainId.hpp"
#include "PartitionStorage.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class PartitionStorageRegistry
{
public:

    // PartitionStorage&
    // GetOrCreate(
    //     DomainId id)
    // {
    //     std::lock_guard<std::mutex>
    //         lock(m_);

    //     auto& ptr =
    //         storages_[id];

    //     if (!ptr)
    //     {
    //         ptr =
    //             std::make_unique<
    //                 PartitionStorage>();
    //     }

    //     return *ptr;
    // }

    const PartitionStorage*
    Find(
        DomainId id) const
    {
        // RIM_LOG_INFO(
        //     "Registry=%p Find domain=%u",
        //     this,
        //     static_cast<unsigned>(id));

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

    void RegisterDomains(
        const std::vector<DomainId>& domains)
    {
        // RIM_LOG_INFO(
        //     "RegisterDomains count=%zu",
        //     domains.size());

        for (const auto domainId : domains)
        {
            // RIM_LOG_INFO(
            //     "Register domain=%u",
            //     static_cast<unsigned>(domainId));

            RegisterDomain(
                domainId);
        }
    }

    bool RegisterDomain(
        DomainId id)
    {
        std::lock_guard<std::mutex>
            lock(m_);

        // RIM_LOG_INFO(
        //     "Registry=%p RegisterDomain id=%u",
        //     this,
        //     static_cast<unsigned>(id));

        if (storages_.find(id) != storages_.end())
        {
            // RIM_LOG_WARNING(
            //     "RegisterDomain duplicate id=%u",
            //     static_cast<unsigned>(id));

            return false;
        }

        storages_.emplace(
            id,
            std::make_unique<PartitionStorage>());

        // RIM_LOG_INFO(
        //     "RegisterDomain success id=%u size=%zu",
        //     static_cast<unsigned>(id),
        //     storages_.size());

        return true;
    }

    PartitionStorage*
    FindMutable(
        DomainId id)
    {
        // RIM_LOG_INFO(
        //     "Registry=%p FindMutable domain=%u",
        //     this,
        //     static_cast<unsigned>(id));

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

    void Dump() const
    {
        std::lock_guard<std::mutex> lock(
            m_);

        RIM_LOG_INFO(
            "=== Storage Dump Begin ===");

        for (const auto& [domainId, storage]
            : storages_)
        {
            RIM_LOG_INFO(
                "Domain=%u",
                static_cast<unsigned>(domainId));

            const auto items =
                storage->GetAll();

            for (const auto& item : items)
            {
                switch (item.value.type)
                {
                case ValueType::kBool:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%s",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.value.b ? "true" : "false");
                    break;

                case ValueType::kInt32:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%d",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.value.i32);
                    break;

                case ValueType::kUInt32:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%u",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.value.u32);
                    break;

                case ValueType::kInt64:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%lld",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        static_cast<long long>(
                            item.value.value.i64));
                    break;

                case ValueType::kUInt64:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%llu",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        static_cast<unsigned long long>(
                            item.value.value.u64));
                    break;

                case ValueType::kDouble:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%f",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.value.d);
                    break;

                case ValueType::kString:
                    RIM_LOG_INFO(
                        "  id=%u type=%d value=%s",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.value.str != nullptr
                            ? item.value.value.str
                            : "(null)");
                    break;

                case ValueType::kBinary:
                    RIM_LOG_INFO(
                        "  id=%u type=%d size=%zu",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type),
                        item.value.size);
                    break;

                default:
                    RIM_LOG_INFO(
                        "  id=%u type=%d",
                        static_cast<unsigned>(item.id),
                        static_cast<int>(item.value.type));
                    break;
                }
            }
        }

        RIM_LOG_INFO(
            "=== Storage Dump End ===");
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
