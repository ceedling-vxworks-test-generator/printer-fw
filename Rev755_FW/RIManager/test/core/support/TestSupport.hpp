#pragma once

#include "rim_api.h"

#include "PartitionStorage.hpp"
#include "RIMValueFactory.hpp"

#include "RIMDataTestHelper.hpp"

namespace rim::test
{

class TestSupport
{
public:

    static void SetAllDoorsClosed(
        PartitionStorage& storage)
    {
        StoreBool(
            storage,
            RI_DATA_UPPER_DOOR_OPEN,
            false);

        StoreBool(
            storage,
            RI_DATA_RIGHT_DOOR_OPEN,
            false);

        StoreBool(
            storage,
            RI_DATA_LEFT_DOOR_OPEN,
            false);
    }

    static void SetAllDoorsOpen(
        PartitionStorage& storage)
    {
        StoreBool(
            storage,
            RI_DATA_UPPER_DOOR_OPEN,
            true);

        StoreBool(
            storage,
            RI_DATA_RIGHT_DOOR_OPEN,
            true);

        StoreBool(
            storage,
            RI_DATA_LEFT_DOOR_OPEN,
            true);
    }

    static void SetEnvironment(
        PartitionStorage& storage,
        double temperature,
        double humidity)
    {
        StoreDouble(
            storage,
            RI_DATA_TEMPERATURE_SENSOR_A,
            temperature);

        StoreDouble(
            storage,
            RI_DATA_HUMIDITY_SENSOR,
            humidity);
    }
};

} // namespace rim::test