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
    NotInitializedReturnsError)
{
    rim::EnvironmentCapability cap{};

    // Create していない(または Destroy 済み)状態で呼ぶと RI_NOT_INITIALIZED。
    EXPECT_EQ(
        RI_NOT_INITIALIZED,
        RIManager_GetCurrentCapability(
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));
}

TEST(
    RIManagerEnvironmentApiTest,
    UngeneratedCapabilityReturnsNoData)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    rim::EnvironmentCapability cap{};

    // Start していないので Capability はまだ生成されていない
    EXPECT_EQ(
        RI_NO_DATA,
        RIManager_GetCurrentCapability(
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));

    RIManager_Destroy();
}

TEST(
    RIManagerEnvironmentApiTest,
    NoPendingNotificationInitially)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    EXPECT_EQ(
        0,
        RIManager_HasPendingCapability(
            rim::kCapEnvironment));

    RIManager_Destroy();
}
