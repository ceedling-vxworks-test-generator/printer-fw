#include "PrinterAAdapter.hpp"

#include "AdapterDispatcher.hpp"
#include "DeviceEvent.hpp"

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
    DeviceEvent event
    {
        RI_DATA_TEMPERATURE_SENSOR_A,
        30
    };

    return dispatcher_.Dispatch(
        event);
}

void PrinterAAdapter::Shutdown()
{
}

}