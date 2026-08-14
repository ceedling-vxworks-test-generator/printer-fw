#include <gtest/gtest.h>

#include "SubscriptionStore.hpp"
#include "NotificationTargetType.hpp"
#include "rim_capability_id.h"

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
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
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
    Remove)
{
    SubscriptionStore store;

    store.Register(
    {
        1,
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    EXPECT_TRUE(
        store.Remove(1));

    SubscriptionInfo info{};

    EXPECT_FALSE(
        store.Find(
            1,
            info));
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
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });
    store.Register(
    {
        2,
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ERROR)
        },
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    store.Register(
    {
        3,
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
        DeliveryMethod::Mailbox,
        NotificationTrigger::Periodic
    });

    std::vector<SubscriptionInfo>
        subscriptions;

    store.GetSubscriptions(
    {
        NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    },
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
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        },
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
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
        DeliveryMethod::Callback,
        NotificationTrigger::OnChange
    });

    store.Register(
    {
        3,
        {
            NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_PRINT_READY)
        },
        DeliveryMethod::Mailbox,
        NotificationTrigger::Periodic
    });

    std::vector<SubscriptionInfo>
        subscriptions;

    store.GetSubscriptions(
    {
        NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    },
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

} // namespace rim