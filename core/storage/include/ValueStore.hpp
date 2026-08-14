#pragma once

#include "RIMDataItem.hpp"
#include "BinaryStoreValue.hpp"
#include "RIMValueAccessor.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace rim
{

class ValueStore
{
public:

    void Store(
        const RIMDataItem& item)
    {
        std::lock_guard<std::mutex> lock(
            m_);

        d_[item.id] =
            item;

        if (item.valueType !=
            ValueType::kBinary)
        {
            return;
        }

        BinaryStoreValue* binary{};

        if (!RIMValueAccessor::GetBinaryStore(
                item.value,
                binary))
        {
            return;
        }

        if (binary == nullptr)
        {
            return;
        }

        binaries_[item.id] =
            std::make_unique<
                BinaryStoreValue>(
                    *binary);
    }

    bool Find(
        RIDataId id,
        RIMDataItem& out) const
    {
        std::lock_guard<std::mutex> lock(m_);

        const auto it = d_.find(id);

        if (it == d_.end())
        {
            return false;
        }

        out = it->second;

        return true;
    }

    bool FindBinary(
        RIDataId id,
        BinaryStoreValue*& out) const
    {
        std::lock_guard<std::mutex> lock(m_);

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

    std::vector<RIMDataItem> GetAll() const
    {
        std::lock_guard<std::mutex> lock(m_);

        std::vector<RIMDataItem> result;

        result.reserve(
            d_.size());

        for (const auto& [id, item] : d_)
        {
            result.push_back(
                item);
        }

        return result;
    }

public: // for test

    std::size_t GetDataCount() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return d_.size();
    }

    std::size_t GetDataBucketCount() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return d_.bucket_count();
    }

    std::size_t GetBinaryCount() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return binaries_.size();
    }

    std::size_t GetBinaryBucketCount() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return binaries_.bucket_count();
    }

private:

    mutable std::mutex m_;

    std::unordered_map<
        RIDataId,
        RIMDataItem> d_;

    std::unordered_map<
        RIDataId,
        std::unique_ptr<BinaryStoreValue>>
        binaries_;
};

} // namespace rim