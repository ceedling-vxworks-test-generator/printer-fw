#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "SnapshotBinaryEntry.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"

namespace rim
{

struct RIMSnapshot
{
    std::vector<RIMDataItem> items;

    std::unordered_map<
        RIDataId,
        SnapshotBinaryEntry
    >
    binaries_;

    bool Find(
        RIDataId id,
        RIMDataItem& item) const
    {
        for (const auto& current : items)
        {
            if (current.id == id)
            {
                item = current;
                return true;
            }
        }

        return false;
    }

    bool TryGetDouble(
        RIDataId id,
        double& value) const
    {
        RIMDataItem item{};

        if (!Find(
                id,
                item))
        {
            return false;
        }

        return RIMValueAccessor::GetDouble(
            item.value,
            value);
    }

    bool TryGetBool(
        RIDataId id,
        bool& value) const
    {
        RIMDataItem item{};

        if (!Find(
                id,
                item))
        {
            return false;
        }

        return RIMValueAccessor::GetBool(
            item.value,
            value);
    }

    bool TryGetInt32(
        RIDataId id,
        std::int32_t& value) const
    {
        RIMDataItem item{};

        if (!Find(
                id,
                item))
        {
            return false;
        }

        return RIMValueAccessor::GetInt32(
            item.value,
            value);
    }

    bool TryGetBinary(
        RIDataId id,
        const std::uint8_t*& bytes,
        std::size_t& size) const
    {
        RIMDataItem item{};

        if (!Find(
                id,
                item))
        {
            return false;
        }

        return RIMValueAccessor::GetBinary(
            item.value,
            bytes,
            size);
    }

    bool GetBinaryHash(
        RIDataId id,
        std::uint64_t& hash) const;    

};

} // namespace rim