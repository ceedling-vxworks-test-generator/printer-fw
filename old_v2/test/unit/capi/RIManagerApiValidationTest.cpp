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
// 検証すべきは「未初期化」「バッファ不正」「バッファ不足」の3種になる。
//
// RIManager はプロセス内に常に1つのシングルトンのため、各試験は
// Create/Destroy を対で呼び、グローバル状態を次の試験へ持ち越さない。
//

TEST(
    RIManagerApiValidationTest,
    GetCapabilityWhenNotInitialized)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_NOT_INITIALIZED,
        RIManager_GetCapability(
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));
}

TEST(
    RIManagerApiValidationTest,
    GetCurrentCapabilityWhenNotInitialized)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_NOT_INITIALIZED,
        RIManager_GetCurrentCapability(
            rim::kCapEnvironment,
            &cap,
            sizeof cap,
            nullptr));
}

TEST(
    RIManagerApiValidationTest,
    GetCapabilityWithNullBuffer)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetCapability(
            rim::kCapEnvironment,
            nullptr,
            16,
            nullptr));

    RIManager_Destroy();
}

TEST(
    RIManagerApiValidationTest,
    SubscribeWithNullCallback)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    std::uint64_t subscriptionId = 0;

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_SubscribeCapability(
            rim::kCapEnvironment,
            nullptr,
            nullptr,
            &subscriptionId));

    RIManager_Destroy();
}

TEST(
    RIManagerApiValidationTest,
    UnsubscribeWhenNotInitialized)
{
    EXPECT_EQ(
        RI_NOT_INITIALIZED,
        RIManager_Unsubscribe(
            1));
}

TEST(
    RIManagerApiValidationTest,
    CreateTwiceIsIdempotent)
{
    ASSERT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    // 二重生成しても古いインスタンスを孤立させず、成功を返す。
    EXPECT_EQ(
        RI_SUCCESS,
        RIManager_Create());

    RIManager_Destroy();
}

TEST(
    RIManagerApiValidationTest,
    DestroyWhenNotInitialized)
{
    EXPECT_EQ(
        RI_NOT_INITIALIZED,
        RIManager_Destroy());
}
