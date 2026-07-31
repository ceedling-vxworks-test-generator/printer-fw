#include <gtest/gtest.h>

#include <cstddef>

#include "rim_api.h"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

//
// バッファ不足の扱い。
//
// 型消去 API では受け側がバッファの大きさを指定するため、
// 「小さすぎるバッファを渡された」場合の挙動が仕様の一部になる。
// 書き込まず RI_BUFFER_TOO_SMALL を返し、**通知を消費しない**こと。
//

TEST(
    RIManagerEnvironmentApiTest,
    InvalidHandle)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetCurrentCapability(
            nullptr,
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));
}

TEST(
    RIManagerEnvironmentApiTest,
    UngeneratedCapabilityReturnsNoData)
{
    RIM_HANDLE handle = nullptr;

    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create(
            &handle));

    rim::EnvironmentCapability cap{};

    // Start していないので Capability はまだ生成されていない
    EXPECT_EQ(
        RI_NO_DATA,
        RIManager_GetCurrentCapability(
            handle,
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));

    RIManager_Destroy(
        handle);
}

TEST(
    RIManagerEnvironmentApiTest,
    NoPendingNotificationInitially)
{
    RIM_HANDLE handle = nullptr;

    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create(
            &handle));

    EXPECT_EQ(
        0,
        RIManager_HasPendingCapability(
            handle,
            rim::kCapEnvironment));

    RIManager_Destroy(
        handle);
}
