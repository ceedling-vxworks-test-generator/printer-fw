#include "adapter/AdapterDispatcher.hpp"

#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMValueFactory.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    const DeviceEvent& event)
{
    RIMDataItem item{};

    item.id = event.id;

    item.valueType =
        ValueType::kDouble;

    item.value =
        RIMValueFactory::CreateDouble(
            static_cast<double>(
                event.value));

    queue_.Push(
        item);

    return true;
}

} // namespace rim