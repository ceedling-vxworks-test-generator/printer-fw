#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <thread>

#include "rim_api.h"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"
#include "DataItem/PrinterADataIds.hpp"

//
// 「現在値の読み出し」と「変化通知」は別経路である、ことの確認。
//
// 通知は1回取り出すと消えるが、現在値は何度でも読める。
// 旧試験は _GetCurrentEnvironment / _GetCurrentError / … と Capability ごとの
// 関数を叩いていたが、現在はどちらも id 引数の1本にまとまっている。
//

namespace
{

constexpr auto kWaitTime =
    std::chrono::milliseconds(
        100);

class EndToEndCApiCurrentStateTest
    : public ::testing::Test
{
protected:

    void SetUp() override
    {
        ASSERT_EQ(
            RIManager_Create(
                &handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Start(
                handle_),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIManager_Stop(
                handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Destroy(
                handle_),
            RI_SUCCESS);
    }

    void WaitPipeline()
    {
        std::this_thread::sleep_for(
            kWaitTime);
    }

    // 現在値を型付きで読む短縮形。
    template <typename T>
    int GetCurrent(
        RICapabilityId id,
        T& out)
    {
        return RIManager_GetCurrentCapability(
            handle_,
            id,
            &out,
            sizeof out,
            nullptr);
    }

    template <typename T>
    int GetNotification(
        RICapabilityId id,
        T& out)
    {
        return RIManager_GetCapability(
            handle_,
            id,
            &out,
            sizeof out,
            nullptr);
    }

    RIM_HANDLE handle_{};
};

} // namespace


TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentEnvironment)
{
    ASSERT_EQ(
        RIManager_TestInjectDouble(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapEnvironment,
            capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        30.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CurrentStateSurvivesNotificationConsumption)
{
    ASSERT_EQ(
        RIManager_TestInjectDouble(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability eventCapability{};

    ASSERT_EQ(
        GetNotification(
            rim::kCapEnvironment,
            eventCapability),
        RI_SUCCESS);

    rim::EnvironmentCapability currentCapability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapEnvironment,
            currentCapability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        currentCapability.temperature,
        30.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    NotificationIsConsumedButCurrentIsNot)
{
    ASSERT_EQ(
        RIManager_TestInjectDouble(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability capability{};

    ASSERT_EQ(
        GetNotification(
            rim::kCapEnvironment,
            capability),
        RI_SUCCESS);

    // 通知は消費済み
    EXPECT_EQ(
        GetNotification(
            rim::kCapEnvironment,
            capability),
        RI_NO_DATA);

    // 現在値は何度でも読める
    EXPECT_EQ(
        GetCurrent(
            rim::kCapEnvironment,
            capability),
        RI_SUCCESS);

    EXPECT_EQ(
        GetCurrent(
            rim::kCapEnvironment,
            capability),
        RI_SUCCESS);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentError)
{
    ASSERT_EQ(
        RIManager_AddError(
            handle_,
            1001),
        RI_SUCCESS);

    WaitPipeline();

    rim::ErrorCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapError,
            capability),
        RI_SUCCESS);

    ASSERT_EQ(
        capability.count,
        1U);

    EXPECT_EQ(
        capability.errors[0].errorCode,
        1001U);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    RemovedErrorDisappearsFromCurrentState)
{
    ASSERT_EQ(
        RIManager_AddError(
            handle_,
            1001),
        RI_SUCCESS);

    WaitPipeline();

    ASSERT_EQ(
        RIManager_RemoveError(
            handle_,
            1001),
        RI_SUCCESS);

    WaitPipeline();

    rim::ErrorCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapError,
            capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability.count,
        0U);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentPrintReady)
{
    ASSERT_EQ(
        RIManager_TestInjectBool(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kUpperDoorOpen),
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kRightDoorOpen),
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kLeftDoorOpen),
            0),
        RI_SUCCESS);

    WaitPipeline();

    rim::PrintReadyCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapPrintReady,
            capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    OpenDoorMakesPrintNotReady)
{
    ASSERT_EQ(
        RIManager_TestInjectBool(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kUpperDoorOpen),
            1),
        RI_SUCCESS);

    WaitPipeline();

    rim::PrintReadyCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapPrintReady,
            capability),
        RI_SUCCESS);

    EXPECT_FALSE(
        capability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentConsumable)
{
    ASSERT_EQ(
        RIManager_TestInjectInt32(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kStapleLevel),
            80),
        RI_SUCCESS);

    WaitPipeline();

    rim::ConsumableCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapConsumable,
            capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability.stapleLevel,
        80);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentJob)
{
    ASSERT_EQ(
        RIManager_TestInjectBool(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kJobActive),
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectInt32(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kJobId),
            123),
        RI_SUCCESS);

    WaitPipeline();

    rim::JobCapability capability{};

    ASSERT_EQ(
        GetCurrent(
            rim::kCapJob,
            capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.jobActive);

    EXPECT_EQ(
        capability.jobId,
        123);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    WrittenSizeIsReported)
{
    ASSERT_EQ(
        RIManager_TestInjectDouble(
            handle_,
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability capability{};

    std::size_t written = 0;

    ASSERT_EQ(
        RIManager_GetCurrentCapability(
            handle_,
            rim::kCapEnvironment,
            &capability,
            sizeof capability,
            &written),
        RI_SUCCESS);

    EXPECT_EQ(
        written,
        sizeof(rim::EnvironmentCapability));
}
