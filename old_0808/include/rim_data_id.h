#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RIDataId
{
    RI_DATA_UNKNOWN = 0,

    RI_DATA_TEMPERATURE_SENSOR_A,
    RI_DATA_TEMPERATURE_SENSOR_B,

    RI_DATA_HUMIDITY_SENSOR,

    RI_DATA_UPPER_DOOR_OPEN,
    RI_DATA_RIGHT_DOOR_OPEN,
    RI_DATA_LEFT_DOOR_OPEN,

    RI_DATA_STAPLE_LEVEL,
    RI_DATA_TONER_LEVEL,

    RI_DATA_JOB_ACTIVE,
    RI_DATA_JOB_ID,

    RI_DATA_ERROR_LIST

} RIDataId;

#ifdef __cplusplus
}
#endif