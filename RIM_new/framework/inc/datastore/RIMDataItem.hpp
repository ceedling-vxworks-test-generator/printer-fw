#pragma once

//
// RIMDataItem - 層間で受け渡す標準データモデル。rim_data_entry_item_t 相当。
//

#include "datastore/RIMDataId.hpp"
#include "datastore/RIMValue.hpp"
#include "datastore/DataContext.hpp"

namespace rim
{

struct RIMDataItem
{
    RIMDataId   id{RIMDataId::kTemperature};
    RIMValue    value{};
    DataContext context{};
};

} // namespace rim
