#pragma once

//
// CurrentValueBuffer - 最新値系の受理バッファ(仕様 §6.4)。
// Idごとに最新値を上書き保持し dirty を立てる。takeUpdatedValues で dirty分を取り出しクリア。
// dirtyの確認とクリアは同一ロック内で行う。
//

#include <array>
#include <mutex>
#include <vector>

#include "datastore/RIMDataId.hpp"
#include "datastore/RIMDataItem.hpp"

namespace rim
{

class CurrentValueBuffer
{
public:

    // 最新値を上書き保持し dirty を立てる。Idが範囲外なら false。
    bool Post(const RIMDataItem& item);

    // dirty の立っている最新値を取り出し、dirty を落とす。周期内で同一IDは最新1件のみ。
    std::vector<RIMDataItem> TakeUpdatedValues();

    void Clear();

private:

    struct Slot
    {
        RIMDataItem data{};
        bool        dirty{false};
    };

    std::mutex mutex_;
    std::array<Slot, kRIMDataIdCount> slots_{};
};

} // namespace rim
