#pragma once

#include <atomic>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <mutex>

#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"
#include "BinaryEntry.hpp"
#include "BinaryHash.hpp"
#include "BinaryInfo.hpp"

namespace rim
{

using StorageItemId = std::uint32_t;

class PartitionStorage
{
public:

    void Store(
        const RIMDataItem& item)
    {
        std::unique_lock<std::shared_mutex>
            lock(m_);

        RIMDataItem storedItem =
            item;

        if (storedItem.value.type ==
            ValueType::kBinary)
        {
            const std::uint8_t* bytes{};
            std::size_t size{};

            if (RIMValueAccessor::GetBinary(
                    storedItem.value,
                    bytes,
                    size))
            {
                auto& binary =
                    binaries_[storedItem.id];

                binary.Set(
                    bytes,
                    size);

                storedItem.value.value.bytes =
                    binary.buffer.data();

                storedItem.value.size =
                    binary.buffer.size();
            }
        }
        else
        {
            binaries_.erase(
                storedItem.id);
        }

        items_[storedItem.id] =
            std::move(storedItem);
    }

    bool Find(
        StorageItemId id,
        RIMDataItem& out) const
    {
        std::shared_lock<
            std::shared_mutex>
            lock(m_);

        const auto it =
            items_.find(id);

        if (it == items_.end())
        {
            return false;
        }

        out =
            it->second;

        return true;
    }

    std::vector<RIMDataItem>
    GetAll() const
    {
        std::shared_lock<
            std::shared_mutex>
            lock(m_);

        std::vector<RIMDataItem>
            result;

        result.reserve(
            items_.size());

        for (const auto& [id, item]
             : items_)
        {
            result.push_back(
                item);
        }

        return result;
    }

    bool GetBinaryHash(
        RIDataId id,
        std::uint64_t& hash) const
    {
        std::shared_lock<
            std::shared_mutex>
            lock(m_);

        const auto it =
            binaries_.find(id);

        if (it == binaries_.end())
        {
            return false;
        }

        hash =
            it->second.hash;

        return true;
    }

    bool GetBinaryInfo(
        RIDataId id,
        BinaryInfo& info) const
    {
        std::shared_lock<
            std::shared_mutex>
            lock(m_);

        const auto it =
            binaries_.find(id);

        if (it == binaries_.end())
        {
            return false;
        }

        info.bytes =
            it->second.buffer.data();

        info.size =
            it->second.buffer.size();

        info.hash =
            it->second.hash;

        return true;
    }

    std::shared_mutex&
    Mutex() const
    {
        return m_;
    }

    std::vector<RIMDataItem>
    GetAllUnlocked() const
    {
        std::vector<RIMDataItem>
            result;

        result.reserve(
            items_.size());

        for (const auto& [id, item]
            : items_)
        {
            result.push_back(
                item);
        }

        return result;
    }

private:

    mutable std::shared_mutex
        m_;

    std::unordered_map<
        StorageItemId,
        RIMDataItem>
        items_;

    std::unordered_map<
        StorageItemId,
        BinaryEntry
    > binaries_;
};

} // namespace rim