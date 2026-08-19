#include "PrinterAAdapter.hpp"

#include "AdapterDispatcher.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "printer_a_data_id.h"

namespace rim
{

PrinterAAdapter::PrinterAAdapter(
    AdapterDispatcher& dispatcher)
    : dispatcher_(dispatcher)
{
}

bool PrinterAAdapter::Initialize()
{
    return true;
}

bool PrinterAAdapter::Poll()
{
    RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.valueType =
        ValueType::kDouble;

    item.value =
        RIMValueFactory::CreateDouble(
            30);

    return dispatcher_.Dispatch(
        item);
}

void PrinterAAdapter::Shutdown()
{
}

}