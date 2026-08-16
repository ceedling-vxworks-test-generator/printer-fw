#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"

#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "AdapterLog.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    const DeviceEvent& event)
{
    RIM_ADAPTER_LOG_TRACE(
        "enter id=%d rawValue=%.6f",
        static_cast<int>(event.id),
        event.value);

    const auto* definition =
        FindDataItem(
            product_,
            event.id);

    if (definition == nullptr)
    {
        RIM_ADAPTER_LOG_ERROR(
            "DataItemDefinition not found: id=%d (unknown or unregistered RIDataId)",
            static_cast<int>(event.id));

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

    RIM_ADAPTER_LOG_TRACE(
        "exit name=%.*s id=%d pushed to StoreInputQueue",
        static_cast<int>(definition->name.size()),
        definition->name.data(),
        static_cast<int>(event.id));

    return true;
}

} // namespace rim