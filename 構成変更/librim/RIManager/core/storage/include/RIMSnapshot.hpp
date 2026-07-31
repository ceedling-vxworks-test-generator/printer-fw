#pragma once

#include <vector>

#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"

namespace rim
{

struct RIMSnapshot
{
    std::vector<RIMDataItem> items;

    bool Find(
        RIMDataId id,
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
        RIMDataId id,
        double& value) const
    {
        RIMDataItem item{};

        if (!Find(id, item))
        {
            return false;
        }

        return RIMValueAccessor::GetDouble(
            item.value,
            value);
    }

    bool TryGetBool(
        RIMDataId id,
        bool& value) const
    {
        RIMDataItem item{};

        if (!Find(id, item))
        {
            return false;
        }

        return RIMValueAccessor::GetBool(
            item.value,
            value);
    }


    bool TryGetInt32(
        RIMDataId id,
        std::int32_t& value) const
    {
        RIMDataItem item{};

        if (!Find(id, item))
        {
            return false;
        }

        return RIMValueAccessor::GetInt32(
            item.value,
            value);
    }



};

} // namespace rim