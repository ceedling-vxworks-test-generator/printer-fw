#include "Adapter.hpp"

#include "AdapterDispatcher.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "data_id.h"

namespace rim
{

Adapter::Adapter(
    AdapterDispatcher& dispatcher)
    : dispatcher_(dispatcher)
{
}

bool Adapter::Initialize()
{
    return true;
}

bool Adapter::Poll()
{
    RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    // item.valueType =
    //     ValueType::kDouble;

    item.value =
        RIMValueFactory::CreateDouble(
            30);

    return dispatcher_.Dispatch(
        item);
}

void Adapter::Shutdown()
{
}

}