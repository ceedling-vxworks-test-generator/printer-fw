#include "RIDataIdConverter.hpp"

rim::RIMDataId
ToInternalDataId(
    RIDataId dataId)
{
    switch (dataId)
    {
    case RI_DATA_TEMPERATURE_SENSOR_A:
        return rim::RIMDataId::kTemperatureSensorA;

    case RI_DATA_TEMPERATURE_SENSOR_B:
        return rim::RIMDataId::kTemperatureSensorB;

    case RI_DATA_HUMIDITY_SENSOR:
        return rim::RIMDataId::kHumiditySensor;

    case RI_DATA_UPPER_DOOR_OPEN:
        return rim::RIMDataId::kUpperDoorOpen;

    case RI_DATA_RIGHT_DOOR_OPEN:
        return rim::RIMDataId::kRightDoorOpen;

    case RI_DATA_LEFT_DOOR_OPEN:
        return rim::RIMDataId::kLeftDoorOpen;

    case RI_DATA_STAPLE_LEVEL:
        return rim::RIMDataId::kStapleLevel;

    case RI_DATA_JOB_ACTIVE:
        return rim::RIMDataId::kJobActive;

    case RI_DATA_JOB_ID:
        return rim::RIMDataId::kJobId;

    case RI_DATA_ERROR_LIST:
        return rim::RIMDataId::kErrorList;

    default:
        return rim::RIMDataId::kUnknown;
    }
}