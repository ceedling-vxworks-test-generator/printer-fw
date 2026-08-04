#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "rim_api.h"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"
#include "DataItem/PrinterADataIds.hpp"

//
// C API 経由で「変化通知」を受け取れること。
//
// 旧試験は RIManager_TestInjectTemperature / _GetEnvironment という
// 機種固有の関数名を叩いていた。現在はどちらも id 引数を取る汎用関数である。
//

TEST(
    EndToEndCApiNotificationTest,
    StartStopAndReceiveCapability)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectDouble(
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    rim::EnvironmentCapability capability{};

    bool received = false;

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

    while (std::chrono::steady_clock::now()
           < timeout)
    {
        if (RIManager_GetCapability(
                rim::kCapEnvironment,
                &capability,
                sizeof capability,
                nullptr)
            == RI_SUCCESS)
        {
            received = true;
            break;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        received);

    if (received)
    {
        EXPECT_DOUBLE_EQ(
            capability.temperature,
            30.0);
    }

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiNotificationTest,
    SmallBufferDoesNotConsumeTheNotification)
{
    // バッファ不足のときは通知を消費しない、という約束の確認。
    // (消費してしまうと、受け側が作り直しても取り返しがつかない)
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectDouble(
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

    while (RIManager_HasPendingCapability(
               rim::kCapEnvironment) != 1
           && std::chrono::steady_clock::now()
                  < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    ASSERT_EQ(
        RIManager_HasPendingCapability(
            rim::kCapEnvironment),
        1);

    char tiny[1]{};

    EXPECT_EQ(
        RIManager_GetCapability(
            rim::kCapEnvironment,
            tiny,
            sizeof tiny,
            nullptr),
        RI_BUFFER_TOO_SMALL);

    // 通知はまだ残っている
    EXPECT_EQ(
        RIManager_HasPendingCapability(
            rim::kCapEnvironment),
        1);

    rim::EnvironmentCapability capability{};

    EXPECT_EQ(
        RIManager_GetCapability(
            rim::kCapEnvironment,
            &capability,
            sizeof capability,
            nullptr),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}
