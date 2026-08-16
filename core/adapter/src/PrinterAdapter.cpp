#include "PrinterAdapter.hpp"

#include "DeviceEvent.hpp"

#include "AdapterLog.hpp"

namespace rim
{

PrinterAdapter::PrinterAdapter(
    AdapterDispatcher& d)
    : dispatcher_(d)
{
}

bool PrinterAdapter::Initialize()
{
    RIM_ADAPTER_LOG_TRACE(
        "enter");

    RIM_ADAPTER_LOG_TRACE(
        "exit result=true");

    return true;
}

bool PrinterAdapter::Poll()
{
    RIM_ADAPTER_LOG_TRACE(
        "enter");

    DeviceEvent ev
    {
        RI_DATA_TEMPERATURE_SENSOR_A,
        30
    };

    const bool result =
        dispatcher_.Dispatch(
            ev);

    if (!result)
    {
        RIM_ADAPTER_LOG_ERROR(
            "Dispatch failed: id=%d",
            static_cast<int>(ev.id));
    }

    RIM_ADAPTER_LOG_TRACE(
        "exit result=%s",
        result ? "true" : "false");

    return result;
}

void PrinterAdapter::Shutdown()
{
    RIM_ADAPTER_LOG_TRACE(
        "enter/exit");
}

} // namespace rim