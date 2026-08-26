#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <cstring>

#include "printer_a.h"

#include "RIMValueFactory.hpp"

TEST(
    EndToEndCApiErrorListTest,
    RoundTrip)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    RI_FAULT_INFO_LIST input{};

    input.count = 1;

    input.entries[0].code = 1234;
    input.entries[0].state = 5;

    ASSERT_EQ(
        PrinterA_SetErrorList(
            &input),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));

    RI_FAULT_INFO_LIST output{};

    ASSERT_EQ(
        PrinterA_GetErrorList(
            &output),
        RI_SUCCESS);

    EXPECT_EQ(
        output.count,
        1U);

    EXPECT_EQ(
        output.entries[0].code,
        1234U);

    EXPECT_EQ(
        output.entries[0].state,
        5U);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}