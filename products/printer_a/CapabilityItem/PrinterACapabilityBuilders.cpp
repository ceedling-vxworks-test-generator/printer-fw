#include "../printer_a_data_id.h"

#include "PrinterACapabilityBuilders.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

RIMValue
BuildEnvironmentCapability(
    const RIMSnapshot& snapshot)
{
    double temperature{};
    double humidity{};

    snapshot.TryGetDouble(RI_DATA_TEMPERATURE_SENSOR_A, temperature);
    snapshot.TryGetDouble(RI_DATA_HUMIDITY_SENSOR, humidity);

    if((temperature >= (60.0 + 273.15)) && (humidity >= 60.0))
    {
        return RIMValueFactory::CreateInt32(2);
    }
    else if((temperature >= (25.0 + 273.15)) && (humidity >= 25.0))
    {
        return RIMValueFactory::CreateInt32(1);
    }

    return RIMValueFactory::CreateInt32(0);
}

RIMValue
BuildPrintReadyCapability(
    const RIMSnapshot& snapshot)
{
    bool upperDoorOpen{};
    bool rightDoorOpen{};
    bool leftDoorOpen{};

    snapshot.TryGetBool(RI_DATA_UPPER_DOOR_OPEN, upperDoorOpen);
    snapshot.TryGetBool(RI_DATA_RIGHT_DOOR_OPEN, rightDoorOpen);
    snapshot.TryGetBool(RI_DATA_LEFT_DOOR_OPEN, leftDoorOpen);

    return RIMValueFactory::CreateBool(!upperDoorOpen && !rightDoorOpen && !leftDoorOpen);
}

} // namespace rim