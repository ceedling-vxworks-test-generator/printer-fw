#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"

#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    RIDataId id,
    int value,
    const RIMContext* context)
{
    // idとcontextの組から変換ルール(normalize)を辿り着く。
    // 現状はidだけでDataItemDefinition(=normalizeそのもの)が決まり、
    // normalize自身がcontextを見て単位等を判別する形(機種依存の
    // normalize関数は products/<機種>/ に定義する)。
    const auto* definition =
        FindDataItem(
            product_,
            id);

    if (definition == nullptr)
    {
        return false;
    }

    const RIMValue rawValue =
        RIMValueFactory::CreateDouble(
            static_cast<double>(
                value));

    const RIMValue setValue =
        definition->normalize(
            rawValue,
            context);

    RIMDataItem item{};

    item.id = id;

    item.valueType =
        definition->setValueType;

    item.value =
        setValue;

    queue_.Push(
        item);

    return true;
}

} // namespace rim
