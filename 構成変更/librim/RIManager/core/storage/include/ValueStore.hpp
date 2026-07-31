#pragma once

#include <cstddef>
#include <mutex>

#include "FixedVector.hpp"
#include "RIMConfig.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

//
// ValueStore - データ項目の最新値を id ごとに保持する。
//
// 旧実装は std::unordered_map<RIMDataId, RIMDataItem> で、
//   - Store のたびにノードを動的確保しうる
//   - GetAll() が std::vector を **返り値で** 作るため、Snapshot を作るたびに確保
// が起きていた(定常運転中にヒープを触る)。
//
// RIMDataId は 0 から連番の列挙なので、**添字で引く固定長配列**にすれば
// 動的確保もハッシュ計算も不要になり、参照は O(1) になる。
//
// 容量は kMaxDataItems。機種が定義する RIMDataId の総数以上にしておくこと
// (足りない id は格納されず、Snapshot に載らない)。
//
class ValueStore
{
public:

    // 最新値を格納する。id が範囲外なら false。
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

    //
    // 格納済みの全項目を out へ書き出す。
    //
    // 旧 GetAll() は std::vector を返していたため呼び出しごとに確保していた。
    // 呼出側の入れ物へ詰める形にして確保を無くしてある。
    //
    void CopyAllTo(
        FixedVector<RIMDataItem, kMaxDataItems>& out) const
    {
        out.Clear();

        std::lock_guard<std::mutex> lock(m_);

        for (std::size_t i = 0; i < kMaxDataItems; ++i)
        {
            if (!slots_[i].present)
            {
                continue;
            }

            out.PushBack(
                slots_[i].item);
        }
    }

    std::size_t Count() const
    {
        std::lock_guard<std::mutex> lock(m_);

        std::size_t n = 0;

        for (std::size_t i = 0; i < kMaxDataItems; ++i)
        {
            if (slots_[i].present) ++n;
        }

        return n;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_);

        for (auto& slot : slots_)
        {
            slot.present = false;
            slot.item    = RIMDataItem{};
        }
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
