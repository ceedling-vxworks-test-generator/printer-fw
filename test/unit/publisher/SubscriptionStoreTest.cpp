#include <gtest/gtest.h>

#include "printer_a.h"

#include "SubscriptionStore.hpp"
#include "NotificationTargetType.hpp"
#include "test/support/NotificationTestHelper.hpp"

namespace rim
{

TEST(
    SubscriptionStoreTest,
    RegisterAndFind)
{
    SubscriptionStore store;

    store.Register(
    {
        1,
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    SubscriptionInfo info{};

    const bool found =
        store.Find(
            1,
            info);

    EXPECT_TRUE(
        found);

    EXPECT_EQ(
        info.id,
        1);

    EXPECT_EQ(
        info.target.type,
        NotificationTargetType::Capability);

    EXPECT_EQ(
        info.target.id,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY));
    
    EXPECT_EQ(
        info.method,
        DeliveryMethod::Callback);
        
    EXPECT_EQ(
        info.trigger,
        NotificationTrigger::OnChange);
}

TEST(
    SubscriptionStoreTest,
    NotFound)
{
    SubscriptionStore store;

    SubscriptionInfo info{};

    EXPECT_FALSE(
        store.Find(
            999,
            info));
}

TEST(
    SubscriptionStoreTest,
    GetSubscriptions)
{
    SubscriptionStore store;

    store.Register(
    {
        1,
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });
    store.Register(
    {
        2,
        test::CapabilityTarget(RI_CAPABILITY_ERROR),
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    store.Register(
    {
        3,
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        DeliveryMethod::Mailbox,
        NotificationTrigger::Periodic
    });

    std::vector<SubscriptionInfo>
        subscriptions;

    store.GetSubscriptions(
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        NotificationTrigger::OnChange,
        subscriptions);

    ASSERT_EQ(
        subscriptions.size(),
        1U);

    EXPECT_EQ(
        subscriptions[0].id,
        1U);

    EXPECT_EQ(
        subscriptions[0].method,
        DeliveryMethod::Callback);

    EXPECT_EQ(
        subscriptions[0].trigger,
        NotificationTrigger::OnChange);
}

TEST(
    SubscriptionStoreTest,
    KeepDeliveryMethodAndTiming)
{
    SubscriptionStore store;

    store.Register(
    {
        1,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        DeliveryMethod::Mailbox,
        NotificationTrigger::Periodic
    });

    SubscriptionInfo info{};

    ASSERT_TRUE(
        store.Find(
            1,
            info));

    EXPECT_EQ(
        info.method,
        DeliveryMethod::Mailbox);

    EXPECT_EQ(
        info.trigger,
        NotificationTrigger::Periodic);
}

TEST(
    SubscriptionStoreTest,
    GetPeriodicSubscriptions)
{
    SubscriptionStore store;

    store.Register(
    {
        1,
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    store.Register(
    {
        3,
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        DeliveryMethod::Mailbox,
        NotificationTrigger::Periodic
    });

    std::vector<SubscriptionInfo>
        subscriptions;

    store.GetSubscriptions(
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        NotificationTrigger::Periodic,
        subscriptions);

    ASSERT_EQ(
        subscriptions.size(),
        1U);

    EXPECT_EQ(
        subscriptions[0].id,
        3U);

    EXPECT_EQ(
        subscriptions[0].method,
        DeliveryMethod::Mailbox);

    EXPECT_EQ(
        subscriptions[0].trigger,
        NotificationTrigger::Periodic);
}

TEST(
    SubscriptionStoreTest,
    GenerateUniqueSubscriptionId)
{
    rim::SubscriptionStore store;

    const auto id1 =
        store.CreateSubscriptionId();

    const auto id2 =
        store.CreateSubscriptionId();

    EXPECT_NE(
        id1,
        id2);
}

} // namespace rim