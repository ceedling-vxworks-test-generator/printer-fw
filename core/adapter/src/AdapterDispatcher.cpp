#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"

#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    const DeviceEvent& event)
{
    const auto* definition =
        FindDataItem(
            product_,
            event.id);

    if (definition == nullptr)
    {
        return false;
    }

    const RIMValue rawValue =
        RIMValueFactory::CreateDouble(
            event.value);

    const RIMValue setValue =
        definition->normalize(
            rawValue,
            event.context);

    RIMDataItem item{};

    item.id = event.id;

    item.valueType =
        definition->setValueType;

    item.value =
        setValue;

    queue_.Push(
        item);

    return true;
}

} // namespace rim