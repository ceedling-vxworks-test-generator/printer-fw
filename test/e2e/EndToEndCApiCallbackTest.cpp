#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>

#include "rim_api.h"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"
#include "DataItem/PrinterADataIds.hpp"

//
// C API のコールバック購読。
//
// 旧試験は _SubscribeEnvironment / _SubscribeError / … と Capability ごとに
// 別関数・別コールバック型を使い、ほぼ同じ手順を5回書いていた。
// 現在は購読も投入も id 引数なので、共通の手順に id を渡すだけで済む。
//

namespace
{

// 受信状況。コールバックはワーカスレッドから呼ばれるので atomic。
struct Received
{
    std::atomic<bool>          called{false};
    std::atomic<std::uint64_t> subscriptionId{0U};
    std::atomic<std::uint16_t> capabilityId{0U};
    std::atomic<std::size_t>   size{0U};
};

void OnCapability(
    std::uint64_t subscriptionId,
    RICapabilityId capabilityId,
    const void* data,
    std::size_t size,
    void* userData)
{
    (void)data;

    auto* received =
        static_cast<Received*>(
            userData);

    if (received == nullptr)
    {
        return;
    }

    received->subscriptionId = subscriptionId;
    received->capabilityId   = capabilityId;
    received->size           = size;
    received->called         = true;
}

// called が立つまで待つ(最大1秒)。
bool WaitCalled(
    const Received& received)
{
    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

    while (!received.called &&
           std::chrono::steady_clock::now()
               < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    return received.called;
}

}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeReceiveAndUnsubscribe)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    Received received;

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeCapability(
            rim::kCapEnvironment,
            OnCapability,
            &received,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_TestInjectDouble(
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            30.0),
        RI_SUCCESS);

    EXPECT_TRUE(
        WaitCalled(
            received));

    // コールバックには購読 ID・Capability ID・実サイズが渡る
    EXPECT_EQ(
        received.subscriptionId.load(),
        id);

    EXPECT_EQ(
        received.capabilityId.load(),
        rim::kCapEnvironment);

    EXPECT_EQ(
        received.size.load(),
        sizeof(rim::EnvironmentCapability));

    ASSERT_EQ(
        RIManager_Unsubscribe(
            id),
        RI_SUCCESS);

    // 解除後は呼ばれない
    received.called = false;

    ASSERT_EQ(
        RIManager_TestInjectDouble(
            rim::ToDataId(
                rim::RIMDataId::kTemperatureSensorA),
            35.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    EXPECT_FALSE(
        received.called);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    ReceiveErrorCapability)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    Received received;

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeCapability(
            rim::kCapError,
            OnCapability,
            &received,
            &id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_AddError(
            1001),
        RI_SUCCESS);

    EXPECT_TRUE(
        WaitCalled(
            received));

    EXPECT_EQ(
        received.capabilityId.load(),
        rim::kCapError);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    ReceivePrintReadyCapability)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    Received received;

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeCapability(
            rim::kCapPrintReady,
            OnCapability,
            &received,
            &id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            rim::ToDataId(
                rim::RIMDataId::kUpperDoorOpen),
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            rim::ToDataId(
                rim::RIMDataId::kRightDoorOpen),
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            rim::ToDataId(
                rim::RIMDataId::kLeftDoorOpen),
            0),
        RI_SUCCESS);

    EXPECT_TRUE(
        WaitCalled(
            received));

    EXPECT_EQ(
        received.capabilityId.load(),
        rim::kCapPrintReady);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    ReceiveConsumableCapability)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    Received received;

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeCapability(
            rim::kCapConsumable,
            OnCapability,
            &received,
            &id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectInt32(
            rim::ToDataId(
                rim::RIMDataId::kStapleLevel),
            80),
        RI_SUCCESS);

    EXPECT_TRUE(
        WaitCalled(
            received));

    EXPECT_EQ(
        received.capabilityId.load(),
        rim::kCapConsumable);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    ReceiveJobCapability)
{
    ASSERT_EQ(
        RIManager_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(),
        RI_SUCCESS);

    Received received;

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeCapability(
            rim::kCapJob,
            OnCapability,
            &received,
            &id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectBool(
            rim::ToDataId(
                rim::RIMDataId::kJobActive),
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectInt32(
            rim::ToDataId(
                rim::RIMDataId::kJobId),
            123),
        RI_SUCCESS);

    EXPECT_TRUE(
        WaitCalled(
            received));

    EXPECT_EQ(
        received.capabilityId.load(),
        rim::kCapJob);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(),
        RI_SUCCESS);
}
