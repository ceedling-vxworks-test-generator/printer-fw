#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "rim_api.h"

TEST(
    RIManagerValueApiTest,
    SetAndGetBool)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            1,
            nullptr),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    int value{};

    ASSERT_EQ(
        RIM_GetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            &value),
        RI_SUCCESS);

    EXPECT_EQ(
        value,
        1);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    SetAndGetInt32)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetInt32(
            RI_DATA_JOB_ID,
            12345,
            nullptr),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    int32_t value{};

    ASSERT_EQ(
        RIM_GetInt32(
            RI_DATA_JOB_ID,
            &value),
        RI_SUCCESS);

    EXPECT_EQ(
        value,
        12345);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    SetAndGetDouble)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            36.5,
            nullptr),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    double value{};

    ASSERT_EQ(
        RIM_GetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            &value),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        value,
        36.5);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    SetDoubleWithCelsiusCtxIsNormalized)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    // ctx に摂氏を指定すると、TemperatureSensorA の normalize
    // (NormalizeTemperatureSensorA)が絶対温度へ換算する。
    const rim_source_unit_t celsius = RIM_UNIT_CELSIUS;

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            25.0,
            &celsius),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    double value{};

    ASSERT_EQ(
        RIM_GetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            &value),
        RI_SUCCESS);

    // 25degC -> 298.15K
    EXPECT_DOUBLE_EQ(
        value,
        298.15);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    GetBoolNullValue)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_GetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            nullptr),
        RI_INVALID_PARAMETER);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    GetInt32NullValue)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_GetInt32(
            RI_DATA_JOB_ID,
            nullptr),
        RI_INVALID_PARAMETER);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    RIManagerValueApiTest,
    GetDoubleNullValue)
{
    ASSERT_EQ(
         RIM_Create(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_GetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            nullptr),
        RI_INVALID_PARAMETER);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}
