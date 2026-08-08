#include <cmath>
#include "ChangeChecker.hpp"

#include "ProductDefinition.hpp"
#include "RIMDataItem.hpp"
#include "RIMValue.hpp"
#include "ValueType.hpp"

namespace rim
{

bool ChangeChecker::isChanged(const RIMValue* oldValue, const RIMDataItem& item) const
{
    if (oldValue == nullptr)
    {
        return true;
    }

    switch (item.valueType)
    {
    case ValueType::kBool:
        return oldValue->value.b != item.value.value.b;

    case ValueType::kInt32:
        return oldValue->value.i32 != item.value.value.i32;

    case ValueType::kUInt32:
        return oldValue->value.u32 != item.value.value.u32;

    case ValueType::kInt64:
        return oldValue->value.i64!= item.value.value.i64;

    case ValueType::kUInt64:
        return oldValue->value.u64 != item.value.value.u64;

    case ValueType::kDouble:
        return std::fabs(oldValue->value.d - item.value.value.d) > 0.0001;

    case ValueType::kString:
        return oldValue->value.str != item.value.value.str;
    }

    return true;
}

}