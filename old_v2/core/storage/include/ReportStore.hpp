#pragma once

#include <cstddef>
#include <mutex>

#include "FixedVector.hpp"
#include "RIMConfig.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

//
// ReportStore - 通知履歴として積むデータ項目。
//
// 旧実装は std::vector<RIMDataItem> に push_back し続けるだけだったため、
//   - 動的確保が起きる
//   - **上限が無く、動かし続けるとメモリを食い尽くす**
// という2つの問題があった。固定容量にし、満杯時は最も古いものを捨てる
// (履歴なので最新側を残す)。捨てた件数は DroppedCount() で観測できる。
//
class ReportStore
{
public:

    using Container =
        FixedVector<RIMDataItem, kMaxDataItems>;

    void Store(
        const RIMDataItem& item)
    {
        std::lock_guard<std::mutex> lock(m_);

        if (items_.Full())
        {
            items_.Erase(0);
            ++dropped_;
        }

        items_.PushBack(
            item);
    }

    std::size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return items_.Size();
    }

    // 溢れて捨てた件数の累計。
    std::size_t DroppedCount() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return dropped_;
    }

    void CopyAllTo(
        Container& out) const
    {
        out.Clear();

        std::lock_guard<std::mutex> lock(m_);

        for (std::size_t i = 0; i < items_.Size(); ++i)
        {
            out.PushBack(
                items_[i]);
        }
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_);
        items_.Clear();
    }

private:

    mutable std::mutex m_;

    Container   items_;
    std::size_t dropped_{0};
};

} // namespace rim
