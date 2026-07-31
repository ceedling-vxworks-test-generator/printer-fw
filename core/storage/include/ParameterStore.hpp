#pragma once

#include <cstddef>
#include <mutex>

#include "RIMConfig.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

//
// ParameterStore - パラメータ種別のデータ項目を id ごとに保持する。
//
// 旧実装は std::unordered_map<int, RIMDataItem>(動的確保あり)。
// ValueStore と同様に id 添字の固定長配列へ置き換えてある。
//
class ParameterStore
{
public:

    bool Store(
        const RIMDataItem& item)
    {
        const std::size_t index =
            static_cast<std::size_t>(
                item.id);

        if (index >= kMaxDataItems)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_);

        slots_[index].item    = item;
        slots_[index].present = true;

        return true;
    }

    bool Find(
        RIMDataId id,
        RIMDataItem& out) const
    {
        const std::size_t index =
            static_cast<std::size_t>(id);

        if (index >= kMaxDataItems)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_);

        if (!slots_[index].present)
        {
            return false;
        }

        out = slots_[index].item;

        return true;
    }

private:

    struct Slot
    {
        RIMDataItem item{};
        bool        present{false};
    };

    mutable std::mutex m_;

    Slot slots_[kMaxDataItems]{};
};

} // namespace rim
