#pragma once

#include "FixedVector.hpp"
#include "RIMConfig.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"

namespace rim
{

//
// ある瞬間の全データ項目。Capability 生成の入力になる。
//
// items は以前 std::vector だったため、**キューへ積むたびに動的確保**が起きていた
// (定常運転中にヒープを触る)。固定容量にしたことで確保はゼロになる。
// 容量は kMaxDataItems。機種が定義する RIMDataId の総数以上にしておくこと。
//
struct RIMSnapshot
{
    FixedVector<RIMDataItem, kMaxDataItems> items;

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