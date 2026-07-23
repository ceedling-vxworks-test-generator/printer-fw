#pragma once

//
// RIMDataItem - 層間で受け渡す標準データモデル(仕様: DataEntryItem {id, value, context})。
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
