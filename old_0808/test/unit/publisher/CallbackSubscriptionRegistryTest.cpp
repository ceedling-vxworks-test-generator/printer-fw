#include <gtest/gtest.h>

#include "rim_capability_id.h"

#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriptionStore.hpp"

namespace
{

rim::NotificationTarget EnvironmentTarget()
{
    return {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };
}

// rim::NotificationTarget ErrorTarget()
// {
//     return {
//         rim::NotificationTargetType::Capability,
//         static_cast<std::uint32_t>(
//             RI_CAPABILITY_ERROR)
//     };
// }

rim::NotificationTarget PrintReadyTarget()
{
    return {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    };
}

rim::NotificationTarget ConsumableTarget()
{
    return {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    };
}

rim::NotificationTarget JobTarget()
{
    return {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_JOB)
    };
}

rim::NotificationMessage EnvironmentMessage()
{
    return {
        EnvironmentTarget(),
        rim::NotificationTrigger::OnChange
    };
}

// rim::NotificationMessage ErrorMessage()
// {
//     return {
//         ErrorTarget(),
//         rim::NotificationTrigger::OnChange
//     };
// }

rim::NotificationMessage JobMessage()
{
    return {
        JobTarget(),
        rim::NotificationTrigger::OnChange
    };
}

}

TEST(
    CallbackSubscriptionRegistryTest,
    InvokeCallback)
{
    bool called = false;

    rim::CallbackSubscriptionRegistry
        registry;

    rim::SubscriptionStore
        store;

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
                rim::NotificationTargetType::Capability);

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
        EnvironmentMessage());

    EXPECT_TRUE(
        called);
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

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeRemovesCallback)
{
    rim::CallbackSubscriptionRegistry
        registry;

    rim::SubscriptionStore
        store;

    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    EXPECT_TRUE(
        registry.Unsubscribe(id));

    registry.Notify(
        id,
        EnvironmentMessage());

    EXPECT_FALSE(
        called);
}

// TEST(
//     CallbackSubscriptionRegistryTest,
//     EnvironmentError)
// {
//     rim::CallbackSubscriptionRegistry registry;

//     rim::SubscriptionStore store;

//     bool environmentCalled = false;
//     bool errorCalled = false;

//     const auto environmentId =
//         store.CreateSubscriptionId();

//     registry.Subscribe(
//         environmentId,
//         [&](rim::SubscriptionId subscriptionId,
//             const rim::NotificationMessage& message)
//         {
//             (void)subscriptionId;

//             EXPECT_EQ(
//                 message.target.id,
//                 static_cast<std::uint32_t>(
//                     RI_CAPABILITY_ENVIRONMENT));

//             environmentCalled = true;
//         });

//     const auto errorId =
//         store.CreateSubscriptionId();

//     registry.Subscribe(
//         errorId,
//         [&](rim::SubscriptionId subscriptionId,
//             const rim::NotificationMessage& message)
//         {
//             (void)subscriptionId;

//             EXPECT_EQ(
//                 message.target.id,
//                 static_cast<std::uint32_t>(
//                     RI_CAPABILITY_ERROR));

//             errorCalled = true;
//         });

//     registry.Notify(
//         environmentId,
//         EnvironmentMessage());

//     EXPECT_TRUE(environmentCalled);
//     EXPECT_FALSE(errorCalled);

//     environmentCalled = false;
//     errorCalled = false;

//     registry.Notify(
//         errorId,
//         ErrorMessage());

//     EXPECT_FALSE(environmentCalled);
//     EXPECT_TRUE(errorCalled);
// }

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeJob)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::SubscriptionStore store;

    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    EXPECT_TRUE(
        registry.Unsubscribe(id));

    registry.Notify(
        id,
        JobMessage());

    EXPECT_FALSE(
        called);


    EXPECT_FALSE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    NotifyOnlySpecifiedEnvironmentSubscriber)
{
    rim::CallbackSubscriptionRegistry
        registry;

    rim::SubscriptionStore
        store;

    bool firstCalled = false;
    bool secondCalled = false;

const auto id1 =
    store.CreateSubscriptionId();

    registry.Subscribe(
        id1,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            firstCalled = true;
        });

    const auto id2 =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id2,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            secondCalled = true;
        });

    registry.Notify(
        id1,
        EnvironmentMessage());

    EXPECT_TRUE(
        firstCalled);

    EXPECT_FALSE(
        secondCalled);
}