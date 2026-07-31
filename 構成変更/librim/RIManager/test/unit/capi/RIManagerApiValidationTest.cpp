#include <gtest/gtest.h>

#include <cstddef>

#include "rim_api.h"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

//
// C API の引数検証。
//
// 旧試験は RIManager_GetEnvironment / _GetError / _GetPrintReady と
// Capability ごとの関数を1本ずつ叩いていた。関数が id 引数方式に統合されたので、
// 検証すべきは「ハンドル不正」「バッファ不正」「バッファ不足」の3種になる。
//

TEST(
    RIManagerApiValidationTest,
    GetCapabilityWithNullHandle)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetCapability(
            nullptr,
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));
}

TEST(
    RIManagerApiValidationTest,
    GetCurrentCapabilityWithNullHandle)
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
    RIManagerApiValidationTest,
    GetCapabilityWithNullBuffer)
{
    RIM_HANDLE handle = nullptr;

    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create(
            &handle));

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetCapability(
            handle,
            rim::kCapEnvironment,
            nullptr,
            16,
            nullptr));

    RIManager_Destroy(
        handle);
}

TEST(
    RIManagerApiValidationTest,
    SubscribeWithNullCallback)
{
    RIM_HANDLE handle = nullptr;

    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create(
            &handle));

    std::uint64_t subscriptionId = 0;

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_SubscribeCapability(
            handle,
            rim::kCapEnvironment,
            nullptr,
            nullptr,
            &subscriptionId));

    RIManager_Destroy(
        handle);
}

TEST(
    RIManagerApiValidationTest,
    UnsubscribeWithNullHandle)
{
    EXPECT_EQ(
        RI_INVALID_HANDLE,
        RIManager_Unsubscribe(
            nullptr,
            1));
}

TEST(
    RIManagerApiValidationTest,
    CreateWithNullOutParameter)
{
    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_Create(
            nullptr));
}

TEST(
    RIManagerApiValidationTest,
    DestroyWithNullHandle)
{
    EXPECT_EQ(
        RI_INVALID_HANDLE,
        RIManager_Destroy(
            nullptr));
}
