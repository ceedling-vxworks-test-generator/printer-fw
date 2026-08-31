#include <gtest/gtest.h>

#include "rim_api.h"

#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriptionStore.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

class CallbackSubscriptionRegistryTest
    : public ::testing::Test
{
protected:
    rim::CallbackSubscriptionRegistry registry;
    rim::SubscriptionStore store;
};

TEST_F(
    CallbackSubscriptionRegistryTest,
    InvokeCallback)
{
    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            (void)subscriptionId;

            EXPECT_EQ(
                message.target.type,
                rim::RIMIdType::Capability);

            EXPECT_EQ(
                message.target.id,
                static_cast<std::uint32_t>(
                    RI_CAPABILITY_ENVIRONMENT));

            EXPECT_EQ(
                message.trigger,
                rim::NotificationTrigger::OnChange);

            called = true;
        });

    registry.Notify(
        id,
        test::OnChangeMessage(RI_CAPABILITY_ENVIRONMENT));

    EXPECT_TRUE(
        called);
}

TEST_F(
    CallbackSubscriptionRegistryTest,
    NotifyOnlySpecifiedEnvironmentSubscriber)
{

    bool firstCalled = false;
    bool secondCalled = false;

    const auto id1 =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id1,
        [&](rim::SubscriptionId,
            const rim::NotificationMessage&)
        {
            firstCalled = true;
        });

    const auto id2 =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id2,
        [&](rim::SubscriptionId,
            const rim::NotificationMessage&)
        {
            secondCalled = true;
        });

    registry.Notify(
        id1,
        test::OnChangeMessage(RI_CAPABILITY_ENVIRONMENT));

    EXPECT_TRUE(
        firstCalled);

    EXPECT_FALSE(
        secondCalled);
}