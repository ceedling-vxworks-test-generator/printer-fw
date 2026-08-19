#pragma once

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "BinaryStoreValue.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"
#include "VersionedStoreState.hpp"

namespace rim
{

class DomainStorage
{
public:

    void Store(
        const RIMDataItem& item)
    {
        std::unique_lock<std::shared_mutex>
            lock(m_);

        items_[item.id] =
            item;

        binaries_.erase(
            item.id);

        if (item.value.type ==
            ValueType::kBinary)
        {
            BinaryStoreValue* binary{};

            if (RIMValueAccessor::GetBinaryStore(
                    item.value,
                    binary) &&
                binary != nullptr)
            {
                binaries_[item.id] =
                    std::make_unique<
                        BinaryStoreValue>(
                            *binary);
            }
        }

        version_.Increment();
    }

    bool Find(
        RIDataId id,
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

    bool FindBinary(
        RIDataId id,
        BinaryStoreValue*& out) const
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

        out =
            it->second.get();

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

    SnapshotVersion
    GetVersion() const
    {
        return version_.Get();
    }

private:

    mutable std::shared_mutex
        m_;

    std::unordered_map<
        RIDataId,
        RIMDataItem>
        items_;

    std::unordered_map<
        RIDataId,
        std::unique_ptr<
            BinaryStoreValue>>
        binaries_;

    VersionedStoreState
        version_;
};

} // namespace rim