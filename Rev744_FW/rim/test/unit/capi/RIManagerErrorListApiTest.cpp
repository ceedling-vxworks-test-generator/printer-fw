#include <gtest/gtest.h>

#include <thread>
#include <chrono>

#include "rim_api.h"

TEST(
    RIManagerErrorListApiTest,
    SetErrorListNullList)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_SetErrorList(
            nullptr),
        RI_INVALID_PARAMETER);

    RIM_Destroy();
}

TEST(
    RIManagerErrorListApiTest,
    GetErrorListNullList)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_GetErrorList(
            nullptr),
        RI_INVALID_PARAMETER);

    RIM_Destroy();
}

TEST(
    RIManagerErrorListApiTest,
    SetAndGetErrorList)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    RI_FAULT_INFO_LIST input{};

    input.count = 2;

    input.entries[0].code = 1001;
    input.entries[0].state = 1;

    input.entries[1].code = 2001;
    input.entries[1].state = 2;

    EXPECT_EQ(
        RIM_SetErrorList(
            &input),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    RI_FAULT_INFO_LIST output{};

    EXPECT_EQ(
        RIM_GetErrorList(
            &output),
        RI_SUCCESS);

    EXPECT_EQ(
        output.count,
        input.count);

    EXPECT_EQ(
        output.entries[0].code,
        input.entries[0].code);

    EXPECT_EQ(
        output.entries[0].state,
        input.entries[0].state);

    EXPECT_EQ(
        output.entries[1].code,
        input.entries[1].code);

    EXPECT_EQ(
        output.entries[1].state,
        input.entries[1].state);

    RIM_Stop();

    RIM_Destroy();
}