#include "PrinterAdapter.hpp"

#include "DeviceEvent.hpp"

namespace rim
{

PrinterAdapter::PrinterAdapter(
    AdapterDispatcher& d)
    : dispatcher_(d)
{
}

bool PrinterAdapter::Initialize()
{
    return true;
}

bool PrinterAdapter::Poll()
{
    DeviceEvent ev
    {
        RI_DATA_TEMPERATURE_SENSOR_A,
        30
    };

    return dispatcher_.Dispatch(
        ev);
}

void PrinterAdapter::Shutdown()
{
}

} // namespace rim