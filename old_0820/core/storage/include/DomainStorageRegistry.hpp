#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "DomainId.hpp"
#include "DomainStorage.hpp"
#include "BinaryStoreValue.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class DomainStorageRegistry
{
public:

    DomainStorage&
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
                    DomainStorage>();
        }

        return *ptr;
    }

    const DomainStorage*
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
        RIDataId id,
        RIMDataItem& item) const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        for (const auto& [domainId, storage]
             : storages_)
        {
            if (storage->Find(
                    id,
                    item))
            {
                return true;
            }
        }

        return false;
    }

    bool FindBinary(
        RIDataId id,
        BinaryStoreValue*& out) const
    {
        std::lock_guard<std::mutex>
            lock(m_);

        for (const auto& [domainId, storage]
             : storages_)
        {
            if (storage->FindBinary(
                    id,
                    out))
            {
                return true;
            }
        }

        return false;
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

private:

    mutable std::mutex
        m_;

    std::unordered_map<
        DomainId,
        std::unique_ptr<
            DomainStorage>>
        storages_;
};

} // namespace rim