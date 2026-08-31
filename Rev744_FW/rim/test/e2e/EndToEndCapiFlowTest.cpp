// C API E2E

#include <gtest/gtest.h>

#include <cstring>
#include <thread>
#include <chrono>

#include "rim_api.h"

TEST(
    EndToEndCapiFlowTest,
    CreateDestroy)
{
    EXPECT_EQ(
        RI_SUCCESS,
        RIM_Create());

    EXPECT_EQ(
        RI_SUCCESS,
        RIM_Destroy());
}

TEST(
    EndToEndCapiFlowTest,
    SetBinaryNullData)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIM_Create());

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIM_SetBinary(
            RI_DATA_ERROR_LIST,
            nullptr,
            10U));

    RIM_Destroy();
}

TEST(
    EndToEndCapiFlowTest,
    SetBinaryZeroSize)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIM_Create());

    std::uint8_t dummy{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIM_SetBinary(
            RI_DATA_ERROR_LIST,
            &dummy,
            0U));

    RIM_Destroy();
}

TEST(
    EndToEndCapiFlowTest,
    SetBinarySuccess)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIM_Create());

    std::uint8_t data[]
    {
        1,
        2,
        3
    };

    EXPECT_EQ(
        RI_SUCCESS,
        RIM_SetBinary(
            RI_DATA_ERROR_LIST,
            data,
            sizeof(data)));

    RIM_Destroy();
}

TEST(
    EndToEndCapiFlowTest,
    GetBinarySuccess)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIM_Create());

    ASSERT_EQ(
        RI_SUCCESS,
        RIM_Start());

    const std::uint8_t input[]
    {
        0x10,
        0x20,
        0x30,
        0x40
    };

    ASSERT_EQ(
        RI_SUCCESS,
        RIM_SetBinary(
            RI_DATA_ERROR_LIST,
            input,
            sizeof(input)));

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    RI_BINARY output{};

    ASSERT_EQ(
        RI_SUCCESS,
        RIM_GetBinary(
            RI_DATA_ERROR_LIST,
            &output));

    ASSERT_EQ(
        sizeof(input),
        output.size);

    ASSERT_NE(
        nullptr,
        output.data);

    EXPECT_EQ(
        0,
        std::memcmp(
            output.data,
            input,
            sizeof(input)));

    RIM_Stop();

    RIM_Destroy();
}